#include <threads/peripherals/csv.hpp>

// TODO come up with a better location
#define CSV_FILE "/var/log/tsdata.csv"

void* csv(void* arg)
{
    (void)arg;

    std::ofstream output(CSV_FILE);

    if (!output.is_open()) {
        std::cout << "CSV: Unable to open csv file - " << CSV_FILE << std::endl;
        return nullptr;
    }

    // TODO come up with a cleaner way of doing this
    output << "isEmergency,isEmergencyButtonPressed,isEmergencySwitchToggled,"
           << "isHydrogenLeaking,isScRelayClosed,isTimeResetButtonPressed,"
           << "isHalfSpeedButtonPressed,isGasButtonPressed,fuelCellMode,"
           << "fcCurrent,fcScCurrent,scMotorCurrent,fcVoltage,scVoltage,"
           << "hydrogenSensorVoltage,fuelCellTemperature,fanRpm,vehicleSpeed,"
           << "motorPwm,hydrogenPressure,gpsLatitude,gpsLongitude,gpsAltitude,"
           << "gpsSpeed,motorSpeed,motorCurrent,fcCurrentRaw,fcVoltageRaw,"
           << "mcCurrent,lapNumber" << std::endl;

    while (true) {
        // TODO check if this is thread safe, or if we should create getters
        output << telemetry_data.isEmergency << ","
               << telemetry_data.isEmergencyButtonPressed << ","
               << telemetry_data.isEmergencySwitchToggled << ","
               << telemetry_data.isHydrogenLeaking << ","
               << telemetry_data.isScRelayClosed << ","
               << telemetry_data.isTimeResetButtonPressed << ","
               << telemetry_data.isHalfSpeedButtonPressed << ","
               << telemetry_data.isGasButtonPressed << ","
               << static_cast<int>(telemetry_data.fuelCellMode) << ","
               << std::fixed << std::setprecision(2) << telemetry_data.fcCurrent << ","
               << telemetry_data.fcScCurrent << ","
               << telemetry_data.scMotorCurrent << ","
               << telemetry_data.fcVoltage << ","
               << telemetry_data.scVoltage << ","
               << telemetry_data.hydrogenSensorVoltage << ","
               << telemetry_data.fuelCellTemperature << ","
               << telemetry_data.fanRpm << ","
               << telemetry_data.vehicleSpeed << ","
               << telemetry_data.motorPwm << ","
               << telemetry_data.hydrogenPressure << ","
               << telemetry_data.gpsLatitude << ","
               << telemetry_data.gpsLongitude << ","
               << telemetry_data.gpsAltitude << ","
               << telemetry_data.gpsSpeed << ","
               << telemetry_data.motorSpeed << ","
               << telemetry_data.motorCurrent << ","
               << telemetry_data.fcCurrentRaw << ","
               << telemetry_data.fcVoltageRaw << ","
               << telemetry_data.mcCurrent << ","
               << static_cast<int>(telemetry_data.lapNumber)
               << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // TODO somehow exit loop to clean up resources
    output.close();
    return nullptr;
}