#include "csv_can_data.hpp"
#include <threads/peripherals/csv.hpp>
#include <utils/log.hpp>
#include <utils/log_paths.hpp>

#include <algorithm>
#include <cmath>
#include <format>
#include <pthread.h>
#include <sstream>
#include <system_error>

extern std::atomic<bool> running;

namespace {

constexpr uintmax_t MAX_FILE_SIZE = 10'000'000; // 10 MB, including header
constexpr uintmax_t MIN_FREE_SPACE = 20ULL * 1024 * 1024; // 20 MiB

const std::string header = "time,V_STACK,V_BUS,V_ACC,I_STACK,I_FC,I_SC,I_MC,I_ACC,Q_N,T_FC,T_AMBIENT,RH_AMBIENT,p_AMBIENT,p_LP,p_HP,n_FAN_FC,c_H2_LEAKAGE,v_SENSOR,v_GPS,v_LATITUDE,v_LONGITUDE,buttons_mask,emergencies\n";

bool has_free_space()
{
    std::error_code error;
    const auto info = std::filesystem::space(log_paths::LOG_DIR, error);
    if (error) {
        log("CSV", "Unable to check free disk space - " + error.message());
        return false;
    }
    if (info.available < MIN_FREE_SPACE) {
        log("CSV", "Less than 20 MiB disk space free, stopping");
        return false;
    }
    return true;
}

bool open_file(std::ofstream& output, std::string& filename, uintmax_t& file_size)
{
    try {
        filename = log_paths::next_data_csv_path();
    } catch (const std::filesystem::filesystem_error& error) {
        log("CSV", "Unable to create CSV path - " + std::string(error.what()));
        return false;
    }
    if (!has_free_space()) {
        return false;
    }
    output.open(filename, std::ios::app);
    if (!output.is_open()) {
        log("CSV", "Unable to open file - " + filename);
        return false;
    }
    std::error_code error;
    file_size = std::filesystem::file_size(filename, error);
    if (error) {
        log("CSV", "Unable to check file size - " + error.message());
        return false;
    }
    if (file_size + header.size() > MAX_FILE_SIZE) {
        log("CSV", "Existing file too large - " + filename);
        return false;
    }
    output << header << std::flush;
    if (!output) {
        log("CSV", "Unable to write CSV header - " + filename);
        return false;
    }
    file_size += header.size();
    return true;
}

} // namespace

void* csv(void* arg)
{
    (void)arg;

    std::string filename;
    std::ofstream output;
    uintmax_t file_size = 0;
    if (!open_file(output, filename, file_size)) {
        return nullptr;
    }

    const sched_param priority{.sched_priority = 1};
    const int sched_error = pthread_setschedparam(pthread_self(), SCHED_FIFO, &priority);
    if (sched_error == 0) {
        log("CSV", "SCHED_FIFO priority 1 enabled");
    } else {
        log("CSV", "Could not set SCHED_FIFO: " + std::generic_category().message(sched_error));
    }

    int checkInterval = 0;
    constexpr auto period = std::chrono::milliseconds(10);
    auto next_row = std::chrono::steady_clock::now();

    auto previous_row = next_row;
    bool have_previous_row = false;
    unsigned intervals = 0;
    double total_interval_us = 0;
    double max_jitter_us = 0;
    double max_wakeup_us = 0;
    uint64_t missed_intervals = 0;

    while (running.load(std::memory_order_relaxed)) {
        const auto woke = std::chrono::steady_clock::now();
        if (have_previous_row) {
            max_wakeup_us = std::max(max_wakeup_us, std::chrono::duration<double, std::micro>(woke - next_row).count());
        }
        if (++checkInterval >= 100) { // check every ~1 s (100 x 10 ms)
            checkInterval = 0;
            if (!has_free_space()) {
                break;
            }
        }

        const auto row_start = std::chrono::steady_clock::now();
        const auto now = std::chrono::system_clock::now();
        if (have_previous_row) {
            const double interval_us = std::chrono::duration<double, std::micro>(row_start - previous_row).count();
            total_interval_us += interval_us;
            max_jitter_us = std::max(max_jitter_us,
                std::abs(interval_us - std::chrono::duration<double, std::micro>(period).count()));
            ++intervals;
        }
        previous_row = row_start;
        have_previous_row = true;
        const double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();

        const auto can = [] {
            const std::lock_guard lock(csv_can_mutex);
            return csv_can_data;
        }();

        std::ostringstream row;
        row << std::fixed << std::setprecision(3) << timestamp << ','
               << std::setprecision(10)
               << get_fuelCellOutputVoltage() << ','
               << get_supercapacitorVoltage() << ','
               << get_accessoryBatteryVoltage() << ','
               << can.stack_current << ','
               << can.fc_current << ','
               << get_supercapacitorCurrent() << ','
               << get_motorControllerSupplyCurrent() << ','
               << get_accessoryBatteryCurrent() << ','
               << can.flow << ','
               << can.fc_temp << ','
               << can.ambient_temp << ','
               << can.humidity << ','
               << can.ambient_pressure << ','
               << can.lp << ','
               << can.hp << ','
               << can.fan_duty << ','
               << get_h2LeakageSensorVoltage() << ','
               << get_sensorSpeed() << ','
               << get_gpsSpeed() << ','
               << get_gpsLatitude() << ','
               << get_gpsLongitude() << ','
               << static_cast<unsigned>(get_buttonsMasterMask()) << ','
               << can.emergencies << '\n';

        const std::string line = row.str();
        if (header.size() + line.size() > MAX_FILE_SIZE) {
            log("CSV", "CSV row exceeds file size limit, stopping");
            break;
        }
        if (file_size + line.size() > MAX_FILE_SIZE) {
            output.close();
            if (!output) {
                log("CSV", "Unable to close file - " + filename);
                break;
            }
            if (!open_file(output, filename, file_size)) {
                break;
            }
            log("CSV", "Rotated CSV file - " + filename);
        }
        output << line << std::flush;
        if (!output) {
            log("CSV", "Unable to write CSV row - " + filename);
            break;
        }
        file_size += line.size();

        if (intervals == 100) {
            log("CSV", std::format(
                "mean interval={:.1f} us | max jitter={:.1f} us | max wakeup delay={:.1f} us | missed={}",
                total_interval_us / intervals, max_jitter_us, max_wakeup_us, missed_intervals));
            intervals = 0;
            total_interval_us = 0;
            max_jitter_us = 0;
            max_wakeup_us = 0;
            missed_intervals = 0;
        }

        // Skip missed deadlines to keep one row per sample interval.
        next_row += period;
        const auto completed = std::chrono::steady_clock::now();
        if (next_row < completed) {
            const auto missed = 1 + (completed - next_row) / period;
            missed_intervals += missed;
            next_row += period * missed;
        }
        std::this_thread::sleep_until(next_row);
    }

    if (output.is_open()) {
        output.close();
        if (!output) {
            log("CSV", "Unable to close file - " + filename);
        }
    }
    return nullptr;
}