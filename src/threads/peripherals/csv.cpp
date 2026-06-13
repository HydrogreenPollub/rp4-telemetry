#include <threads/peripherals/csv.hpp>
#include <utils/log.hpp>
#include <utils/log_paths.hpp>

extern std::atomic<bool> running;

void* csv(void* arg)
{
    (void)arg;

    const std::string filename = log_paths::next_data_csv_path();

    std::ofstream output(filename, std::ios::app);

    if (!output.is_open()) {
        log("CSV", "Unable to open file - " + filename);
        return nullptr;
    }

    // TODO come up with a cleaner way of doing this
    output << std::fixed << std::setprecision(10);
    output << "id,time,timeBeforeTransmit,accessoryBatteryVoltage,accessoryBatteryCurrent,"
           << "fuelCellOutputVoltage,fuelCellOutputCurrent,supercapacitorVoltage,supercapacitorCurrent,"
           << "motorControllerSupplyVoltage,motorControllerSupplyCurrent,fuelCellEnergyAccumulated,"
           << "h2PressureLow,h2PressureFuelCell,h2PressureHigh,h2LeakageSensorVoltage,"
           << "fanDutyCycle,blowerDutyCycle,temperatureFuelCellLocation1,temperatureFuelCellLocation2,"
           << "accelPedalVoltage,brakePedalVoltage,accelOutputVoltage,brakeOutputVoltage,"
           << "buttonsMasterMask,buttonsSteeringWheelMask,sensorRpm,sensorSpeed,lapNumber,lapTime,"
           << "gpsAltitude,gpsLatitude,gpsLongitude,gpsSpeed,"
           << "masterState,protiumState,mainValveEnableOutput,motorControllerEnableOutput"
           << std::endl;

    constexpr uintmax_t MAX_FILE_SIZE  = 50ULL * 1024 * 1024; // 50 MB
    constexpr uintmax_t MIN_FREE_SPACE = 20ULL * 1024 * 1024; // 20 MB
    int checkInterval = 0;
    uint64_t row_id = 0;

    while (running.load(std::memory_order_relaxed)) {
        if (++checkInterval >= 100) { // check every ~1 s (100 x 10 ms)
            checkInterval = 0;
            if (std::filesystem::file_size(filename) >= MAX_FILE_SIZE) {
                log("CSV", "50 MB file size limit reached, stopping");
                break;
            }
            if (std::filesystem::space(log_paths::LOG_DIR).available < MIN_FREE_SPACE) {
                log("CSV", "Less than 20 MB disk space free, stopping");
                break;
            }
        }

        auto now = std::chrono::system_clock::now();
        const double timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();

        output << ++row_id << ",";
        output << std::fixed << std::setprecision(3) << timestamp << ",";
        output << std::setprecision(10);
        output << get_timeBeforeTransmit() << ","
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
               << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // TODO somehow exit loop to clean up resources
    output.close();
    return nullptr;
}