#include <utils/log.hpp>

#include <chrono>
#include <cstdio>
#include <ctime>
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
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm tm_buf;
    gmtime_r(&t, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &tm_buf);
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "[" << ts << "] [" << tag << "] " << msg << "\n";
}
