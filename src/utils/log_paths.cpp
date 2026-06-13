#include <utils/log_paths.hpp>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <string>

namespace log_paths {

std::string timestamp_tag()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm tm_buf {};
    localtime_r(&t, &tm_buf);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tm_buf);
    return buf;
}

int next_serial_number(const std::string& prefix)
{
    int max_number = 0;

    if (!std::filesystem::exists(LOG_DIR)) {
        return 1;
    }

    const std::string needle = prefix + "_";
    for (const auto& entry : std::filesystem::directory_iterator(LOG_DIR)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string name = entry.path().filename().string();
        if (name.rfind(needle, 0) != 0) {
            continue;
        }

        const std::string rest = name.substr(needle.size());
        const std::size_t sep = rest.find('_');
        if (sep == std::string::npos || sep == 0) {
            continue;
        }

        const std::string number_part = rest.substr(0, sep);
        if (!std::all_of(number_part.begin(), number_part.end(), ::isdigit)) {
            continue;
        }

        const int n = std::stoi(number_part);
        max_number = std::max(max_number, n);
    }

    return max_number + 1;
}

std::string next_data_csv_path()
{
    std::filesystem::create_directories(LOG_DIR);
    const int number = next_serial_number("data");
    return std::string(LOG_DIR) + "/data_" + std::to_string(number) + "_"
        + timestamp_tag() + ".csv";
}

std::string next_logs_path()
{
    std::filesystem::create_directories(LOG_DIR);
    const int number = next_serial_number("logs");
    return std::string(LOG_DIR) + "/logs_" + std::to_string(number) + "_"
        + timestamp_tag() + ".log";
}

} // namespace log_paths
