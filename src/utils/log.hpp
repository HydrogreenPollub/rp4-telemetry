#pragma once
#include <cstdint>
#include <string>

std::string hex_bytes(const uint8_t* data, size_t len);
void log(const char* tag, const std::string& msg);
