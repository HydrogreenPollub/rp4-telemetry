#pragma once

#include <string>

namespace log_paths {

constexpr const char* LOG_DIR = "/home/root/logs";

std::string timestamp_tag();
int next_serial_number(const std::string& prefix);
std::string next_data_csv_path();
std::string next_logs_path();

} // namespace log_paths
