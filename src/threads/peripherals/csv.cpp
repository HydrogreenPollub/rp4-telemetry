#include <threads/peripherals/csv.hpp>
#include <utils/log.hpp>
#include <utils/log_paths.hpp>

#include <sstream>
#include <system_error>

extern std::atomic<bool> running;

namespace {

constexpr uintmax_t MAX_FILE_SIZE = 10'000'000; // 10 MB, including header
constexpr uintmax_t MIN_FREE_SPACE = 20ULL * 1024 * 1024; // 20 MiB

const std::string header = "id,time,timeBeforeTransmit,accessoryBatteryVoltage,accessoryBatteryCurrent,"
                           "fuelCellOutputVoltage,fuelCellOutputCurrent,supercapacitorVoltage,supercapacitorCurrent,"
                           "motorControllerSupplyVoltage,motorControllerSupplyCurrent,fuelCellEnergyAccumulated,"
                           "h2PressureLow,h2PressureFuelCell,h2PressureHigh,h2LeakageSensorVoltage,"
                           "fanDutyCycle,blowerDutyCycle,temperatureFuelCellLocation1,temperatureFuelCellLocation2,"
                           "accelPedalVoltage,brakePedalVoltage,accelOutputVoltage,brakeOutputVoltage,"
                           "buttonsMasterMask,buttonsSteeringWheelMask,sensorRpm,sensorSpeed,lapNumber,lapTime,"
                           "gpsAltitude,gpsLatitude,gpsLongitude,gpsSpeed,"
                           "masterState,protiumState,mainValveEnableOutput,motorControllerEnableOutput"
                           "\n";

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
    uint64_t row_id = 0;

    while (running.load(std::memory_order_relaxed)) {
        if (++checkInterval >= 100) { // check every ~1 s (100 x 10 ms)
            checkInterval = 0;
            if (!has_free_space()) {
                break;
            }
        }

        auto now = std::chrono::system_clock::now();
        const double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();

        std::ostringstream row;
        row << ++row_id << ",";
        row << std::fixed << std::setprecision(3) << timestamp << ",";
        row << std::setprecision(10);
        row << get_timeBeforeTransmit() << ","
            << get_accessoryBatteryVoltage() << ","
            << get_accessoryBatteryCurrent() << ","
            << get_fuelCellOutputVoltage() << ","
            << get_fuelCellOutputCurrent() << ","
            << get_supercapacitorVoltage() << ","
            << get_supercapacitorCurrent() << ","
            << get_motorControllerSupplyVoltage() << ","
            << get_motorControllerSupplyCurrent() << ","
            << get_fuelCellEnergyAccumulated() << ","
            << get_h2PressureLow() << ","
            << get_h2PressureFuelCell() << ","
            << get_h2PressureHigh() << ","
            << get_h2LeakageSensorVoltage() << ","
            << get_fanDutyCycle() << ","
            << get_blowerDutyCycle() << ","
            << get_temperatureFuelCellLocation1() << ","
            << get_temperatureFuelCellLocation2() << ","
            << get_accelPedalVoltage() << ","
            << get_brakePedalVoltage() << ","
            << get_accelOutputVoltage() << ","
            << get_brakeOutputVoltage() << ","
            << static_cast<int>(get_buttonsMasterMask()) << ","
            << static_cast<int>(get_buttonsSteeringWheelMask()) << ","
            << get_sensorRpm() << ","
            << get_sensorSpeed() << ","
            << static_cast<int>(get_lapNumber()) << ","
            << get_lapTime() << ","
            << get_gpsAltitude() << ","
            << get_gpsLatitude() << ","
            << get_gpsLongitude() << ","
            << get_gpsSpeed() << ","
            << static_cast<int>(get_masterState()) << ","
            << static_cast<int>(get_protiumState()) << ","
            << (get_mainValveEnableOutput() ? "true" : "false") << ","
            << (get_motorControllerEnableOutput() ? "true" : "false")
            << '\n';

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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (output.is_open()) {
        output.close();
        if (!output) {
            log("CSV", "Unable to close file - " + filename);
        }
    }
    return nullptr;
}