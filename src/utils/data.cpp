#include <utils/data.hpp>

capnp::MallocMessageBuilder global_message;
std::mutex data_mutex;

uint8_t buffer[TSDATA_BUFFER_SIZE];

void init_data(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.initRoot<TSData>(); // Must be called before read_data()
}

const uint8_t* read_data(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);

    kj::Array<capnp::word> dataArr = capnp::messageToFlatArray(global_message);
    kj::ArrayPtr<kj::byte> bytes = dataArr.asBytes();

    std::memcpy(buffer, bytes.begin(), bytes.size());

    return buffer;
}

// Setters
void set_time(uint64_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTime(value);
}

void set_timeBeforeTransmit(uint64_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTimeBeforeTransmit(value);
}

void set_accessoryBatteryVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccessoryBatteryVoltage(value);
}
void set_accessoryBatteryCurrent(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccessoryBatteryCurrent(value);
}

void set_fuelCellOutputVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFuelCellOutputVoltage(value);
}

void set_fuelCellOutputCurrent(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFuelCellOutputCurrent(value);
}

void set_supercapacitorVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSupercapacitorVoltage(value);
}

void set_supercapacitorCurrent(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSupercapacitorCurrent(value);
}

void set_motorControllerSupplyVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMotorControllerSupplyVoltage(value);
}

void set_motorControllerSupplyCurrent(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMotorControllerSupplyCurrent(value);
}

void set_fuelCellEnergyAccumulated(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFuelCellEnergyAccumulated(value);
}

void set_h2PressureLow(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setH2PressureLow(value);
}

void set_h2PressureFuelCell(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setH2PressureFuelCell(value);
}

void set_h2PressureHigh(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setH2PressureHigh(value);
}

void set_h2LeakageSensorVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setH2LeakageSensorVoltage(value);
}

void set_fanDutyCycle(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFanDutyCycle(value);
}

void set_blowerDutyCycle(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setBlowerDutyCycle(value);
}

void set_temperatureFuelCellLocation1(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTemperatureFuelCellLocation1(value);
}

void set_temperatureFuelCellLocation2(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTemperatureFuelCellLocation2(value);
}

void set_accelPedalVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccelPedalVoltage(value);
}

void set_brakePedalVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setBrakePedalVoltage(value);
}

void set_accelOutputVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccelOutputVoltage(value);
}

void set_brakeOutputVoltage(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setBrakeOutputVoltage(value);
}

void set_buttonsMasterMask(uint8_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setButtonsMasterMask(value);
}

void set_buttonsSteeringWheelMask(uint8_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setButtonsSteeringWheelMask(value);
}

void set_sensorRpm(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSensorRpm(value);
}

void set_sensorSpeed(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSensorSpeed(value);
}

void set_lapNumber(uint8_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setLapNumber(value);
}

void set_lapTime(uint64_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setLapTime(value);
}

void set_gpsAltitude(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setGpsAltitude(value);
}

void set_gpsLatitude(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setGpsLatitude(value);
}

void set_gpsLongitude(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setGpsLongitude(value);
}

void set_gpsSpeed(float value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setGpsSpeed(value);
}

// Enums and bools
void set_masterState(uint16_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMasterState(static_cast<TSData::MasterOperatingState>(value));
}

void set_protiumState(uint16_t value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setProtiumState(static_cast<TSData::ProtiumOperatingState>(value));
}

void set_mainValveEnableOutput(bool value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMainValveEnableOutput(value);
}

void set_motorControllerEnableOutput(bool value)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMotorControllerEnableOutput(value);
}

// Getters
uint64_t get_time(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTime();
}

uint64_t get_timeBeforeTransmit(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTimeBeforeTransmit();
}

float get_accessoryBatteryVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getAccessoryBatteryVoltage();
}

float get_accessoryBatteryCurrent(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getAccessoryBatteryCurrent();
}

float get_fuelCellOutputVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getFuelCellOutputVoltage();
}

float get_fuelCellOutputCurrent(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getFuelCellOutputCurrent();
}

float get_supercapacitorVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getSupercapacitorVoltage();
}

float get_supercapacitorCurrent(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getSupercapacitorCurrent();
}

float get_motorControllerSupplyVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getMotorControllerSupplyVoltage();
}

float get_motorControllerSupplyCurrent(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getMotorControllerSupplyCurrent();
}

float get_fuelCellEnergyAccumulated(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getFuelCellEnergyAccumulated();
}

float get_h2PressureLow(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getH2PressureLow();
}

float get_h2PressureFuelCell(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getH2PressureFuelCell();
}

float get_h2PressureHigh(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getH2PressureHigh();
}

float get_h2LeakageSensorVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getH2LeakageSensorVoltage();
}

float get_fanDutyCycle(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getFanDutyCycle();
}

float get_blowerDutyCycle(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getBlowerDutyCycle();
}

float get_temperatureFuelCellLocation1(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTemperatureFuelCellLocation1();
}

float get_temperatureFuelCellLocation2(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTemperatureFuelCellLocation2();
}

float get_accelPedalVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getAccelPedalVoltage();
}

float get_brakePedalVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getBrakePedalVoltage();
}

float get_accelOutputVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getAccelOutputVoltage();
}

float get_brakeOutputVoltage(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getBrakeOutputVoltage();
}

uint8_t get_buttonsMasterMask(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getButtonsMasterMask();
}

uint8_t get_buttonsSteeringWheelMask(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getButtonsSteeringWheelMask();
}

float get_sensorRpm(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getSensorRpm();
}

float get_sensorSpeed(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getSensorSpeed();
}

uint8_t get_lapNumber(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getLapNumber();
}

uint64_t get_lapTime(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getLapTime();
}

float get_gpsAltitude(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getGpsAltitude();
}

float get_gpsLatitude(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getGpsLatitude();
}

float get_gpsLongitude(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getGpsLongitude();
}

float get_gpsSpeed(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getGpsSpeed();
}

uint16_t get_masterState(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return static_cast<uint16_t>(global_message.getRoot<TSData>().getMasterState());
}

uint16_t get_protiumState(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return static_cast<uint16_t>(global_message.getRoot<TSData>().getProtiumState());
}

bool get_mainValveEnableOutput(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getMainValveEnableOutput();
}

bool get_motorControllerEnableOutput(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getMotorControllerEnableOutput();
}