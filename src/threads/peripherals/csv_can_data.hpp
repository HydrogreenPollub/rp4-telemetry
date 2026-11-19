#pragma once

#include <limits>
#include <mutex>

struct CsvCanData {
    static constexpr double missing = std::numeric_limits<double>::quiet_NaN();
    double stack_current = missing;
    double fc_current = missing;
    double flow = missing;
    double fc_temp = missing;
    double ambient_temp = missing;
    double humidity = missing;
    double ambient_pressure = missing;
    double lp = missing;
    double hp = missing;
    double fan_duty = missing;
    unsigned emergencies = 0;
};

inline CsvCanData csv_can_data;
inline std::mutex csv_can_mutex;
