#!/usr/bin/env python3
"""Exercise CSV rotation on a host, without telemetry hardware.

Run: python3 tests/test_telemetry_csv_rotation.py --source /path/to/rp4-telemetry
The default source is this repository. No source files are modified.
"""
import argparse
import csv
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT


def run(source):
    with tempfile.TemporaryDirectory(prefix="telemetry-csv-test-") as temporary:
        work = Path(temporary)
        include = work / "include"
        (include / "threads/peripherals").mkdir(parents=True)
        (include / "utils").mkdir()
        target = work / "src/threads/peripherals/csv.cpp"
        target.parent.mkdir(parents=True)
        target.write_text((source / "src/threads/peripherals/csv.cpp").read_text())

        # Use the CSV implementation; only replace its 10 ms wait to
        # exercise the full 10 MB threshold quickly and stop deterministically.
        implementation = target.read_text()
        wait = "std::this_thread::sleep_for(std::chrono::milliseconds(10));"
        assert implementation.count(wait) == 1
        target.write_text(implementation.replace(wait, "test_tick();"))
        declarations = re.findall(r"^(\w+) (get_\w+)\(void\);$",
                                  (source / "src/utils/data.hpp").read_text(), re.M)
        assert declarations
        (include / "threads/peripherals/csv.hpp").write_text(
            "#pragma once\n#include <atomic>\n#include <chrono>\n#include <filesystem>\n"
            "#include <fstream>\n#include <iomanip>\n#include <thread>\n"
            "#include <cstdint>\nvoid* csv(void*);\nvoid test_tick();\n"
            "std::filesystem::space_info test_space(const char*, std::error_code&);\n"
            + "".join(f"{kind} {name}(void);\n" for kind, name in declarations))
        (include / "utils/log.hpp").write_text(
            "#include <string>\nvoid log(const char*, const std::string&);\n")
        (include / "utils/log_paths.hpp").write_text(
            "#pragma once\n#include <cstdlib>\n#include <string>\nnamespace log_paths {\n"
            'inline const char* LOG_DIR = std::getenv("CSV_TEST_DIR");\n'
            "std::string next_data_csv_path();\nstd::string real_next_data_csv_path();\n"
            "int next_serial_number(const std::string&);\nstd::string timestamp_tag();\n}\n")
        # Keep the real filename generator, with a wrapper for fault injection.
        paths = (source / "src/utils/log_paths.cpp").read_text()
        (work / "log_paths.cpp").write_text(paths.replace(
            "std::string next_data_csv_path()", "std::string real_next_data_csv_path()"))
        driver = r'''#include <threads/peripherals/csv.hpp>
#include <utils/log_paths.hpp>
#include <iostream>
#include <string>
#include <csignal>
#include <sys/resource.h>
std::atomic<bool> running(true);
std::string mode;
unsigned ticks = 0;
unsigned files = 0;
unsigned samples = 0;
void test_tick() { if (++ticks >= 50000) running.store(false); }
void log(const char*, const std::string& message) { std::cout << message << "\n"; }
std::filesystem::space_info test_space(const char* path, std::error_code& error) {
    if (mode == "space-error") {
        error = std::make_error_code(std::errc::io_error);
        return {};
    }
    if (mode == "low-space" && ticks >= 99) return {0, 0, 0};
    return std::filesystem::space(path, error);
}
namespace log_paths {
std::string next_data_csv_path() {
    ++files;
    if (mode == "open-error" && files == 2)
        return std::string(LOG_DIR) + "/missing/data.csv";
    return real_next_data_csv_path();
}
}
'''
        driver += "".join(
            f"{kind} {name}(void) {{ "
            + ("return ++samples;" if name == "get_timeBeforeTransmit" else "return 0;")
            + " }\n" for kind, name in declarations)
        driver += r'''int main(int argc, char** argv) {
    mode = argc > 1 ? argv[1] : "rotation";
    if (mode == "write-error" || mode == "row-write-error") {
        std::signal(SIGXFSZ, SIG_IGN);
        const rlim_t maximum = mode == "write-error" ? 1 : 1024;
        const struct rlimit limit = {maximum, maximum};
        if (setrlimit(RLIMIT_FSIZE, &limit) != 0) return 1;
    }
    csv(nullptr);
    std::cout << "ticks=" << ticks << " samples=" << samples << "\n";
}
'''
        (work / "driver.cpp").write_text(driver)
        # Inject space errors without filling the host filesystem.
        fault_target = work / "fault_csv.cpp"
        fault_target.write_text(target.read_text().replace(
            "std::filesystem::space(log_paths::LOG_DIR, error)",
            "test_space(log_paths::LOG_DIR, error)"))
        for name, cpp in (("normal", target), ("fault", fault_target)):
            subprocess.run(["g++", "-std=c++20", "-Wall", "-Wextra", "-Werror",
                            "-O2", "-pthread", "-I", str(include), str(cpp),
                            str(work / "log_paths.cpp"), str(work / "driver.cpp"),
                            "-o", str(work / name)], check=True)

        def scenario(mode, binary="fault"):
            directory = work / mode
            directory.mkdir()
            result = subprocess.run([str(work / binary), mode], check=True,
                                    capture_output=True, text=True, timeout=30,
                                    env={**os.environ, "CSV_TEST_DIR": str(directory)})
            files = sorted(directory.glob("data_*.csv"),
                           key=lambda p: int(p.name.split("_")[1]))
            if mode in ("write-error", "row-write-error"):
                return files, [], result.stdout
            rows = []
            header = None
            for path in files:
                assert path.stat().st_size <= 10_000_000, path
                assert path.read_bytes().endswith(b"\n"), path
                with path.open(newline="") as stream:
                    reader = csv.reader(stream)
                    current = next(reader)
                    if header is None:
                        header = current
                    assert current == header
                    for row in reader:
                        assert len(row) == len(header)
                        rows.append(row)
            assert [int(row[0]) for row in rows] == list(range(1, len(rows) + 1))
            assert [int(row[2]) for row in rows] == list(range(1, len(rows) + 1))
            return files, rows, result.stdout

        files, rows, output = scenario("rotation", "normal")
        assert len(files) >= 3 and len(rows) == 50000
        assert output.count("Rotated CSV file") == len(files) - 1
        print(f"PASS: {len(files)} files, 50000 continuous rows, headers and 10 MB limit")
        files, rows, output = scenario("open-error")
        assert len(files) == 1 and rows and "Unable to open file" in output
        print("PASS: failure opening the next file stops CSV and logs the error")
        files, rows, output = scenario("low-space")
        assert len(rows) == 99 and "Less than 20 MiB" in output
        print("PASS: 20 MiB free-space safeguard")
        files, rows, output = scenario("space-error")
        assert not files and "Unable to check free disk space" in output
        print("PASS: filesystem space-query failure")
        files, rows, output = scenario("write-error")
        assert len(files) == 1 and "Unable to write CSV header" in output
        print("PASS: header write failure")
        files, rows, output = scenario("row-write-error")
        assert len(files) == 1 and "Unable to write CSV row" in output
        print("PASS: row write failure")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE)
    run(parser.parse_args().source.resolve())
