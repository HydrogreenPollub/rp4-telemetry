#include <threads/peripherals/csv.hpp>

void* csv(void* arg)
{
    (void)arg;

    std::string logDir = std::string("/home/root/logs");
    std::filesystem::create_directories(logDir);

    // Find the next available numbered file: data_1.csv, data_2.csv, etc.
    int fileIndex = 1;
    std::string filename;
    do {
        filename = logDir + "/data_" + std::to_string(fileIndex) + ".csv";
        fileIndex++;
    } while (std::filesystem::exists(filename));

    std::ofstream output(filename, std::ios::app);

    if (!output.is_open()) {
        std::cout << "CSV: Unable to open csv file - " << filename << std::endl;
        return nullptr;
    }

    // TODO come up with a cleaner way of doing this
    output << std::fixed << std::setprecision(10);
    output << "time,timeBeforeTransmit,accessoryBatteryVoltage,accessoryBatteryCurrent,"
           << "fuelCellOutputVoltage,fuelCellOutputCurrent,supercapacitorVoltage,supercapacitorCurrent,"
           << "motorControllerSupplyVoltage,motorControllerSupplyCurrent,fuelCellEnergyAccumulated,"
           << "h2PressureLow,h2PressureFuelCell,h2PressureHigh,h2LeakageSensorVoltage,"
           << "fanDutyCycle,blowerDutyCycle,temperatureFuelCellLocation1,temperatureFuelCellLocation2,"
           << "accelPedalVoltage,brakePedalVoltage,accelOutputVoltage,brakeOutputVoltage,"
           << "buttonsMasterMask,buttonsSteeringWheelMask,sensorRpm,sensorSpeed,lapNumber,lapTime,"
           << "gpsAltitude,gpsLatitude,gpsLongitude,gpsSpeed,"
           << "masterState,protiumState,mainValveEnableOutput,motorControllerEnableOutput"
           << std::endl;

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);

        output << static_cast<int>(timestamp) << ","
               << get_timeBeforeTransmit() << ","
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