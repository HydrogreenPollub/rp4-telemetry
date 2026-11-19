#include "csv_can_data.hpp"
#include <threads/peripherals/csv.hpp>
#include <utils/log.hpp>
#include <utils/log_paths.hpp>

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

    int checkInterval = 0;
    constexpr auto period = std::chrono::milliseconds(10);
    auto next_row = std::chrono::steady_clock::now();

    while (running.load(std::memory_order_relaxed)) {
        if (++checkInterval >= 100) { // check every ~1 s (100 x 10 ms)
            checkInterval = 0;
            if (!has_free_space()) {
                break;
            }
        }

        const auto now = std::chrono::system_clock::now();
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

        // Skip missed deadlines to keep one row per sample interval.
        next_row += period;
        const auto completed = std::chrono::steady_clock::now();
        if (next_row < completed) {
            const auto missed = 1 + (completed - next_row) / period;
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