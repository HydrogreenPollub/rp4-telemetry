#include <utils/log.hpp>
#include <utils/log_paths.hpp>

#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>

std::string hex_bytes(const uint8_t* data, size_t len)
{
    std::string result;
    char buf[4];
    for (size_t i = 0; i < len; ++i) {
        snprintf(buf, sizeof(buf), "%02X ", data[i]);
        result += buf;
    }
    if (!result.empty())
        result.pop_back();
    return result;
}

void log(const char* tag, const std::string& msg)
{
    static std::mutex mtx;
    static std::ofstream log_file;

    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm tm_buf;
    gmtime_r(&t, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &tm_buf);

    std::lock_guard<std::mutex> lock(mtx);

    if (!log_file.is_open()) {
        std::filesystem::create_directories(log_paths::LOG_DIR);
        log_file.open(log_paths::next_logs_path(), std::ios::app);
    }

    const std::string line = std::string("[") + ts + "] [" + tag + "] " + msg;
    std::cout << line << '\n';
    if (log_file.is_open()) {
        log_file << line << '\n';
    }
}
