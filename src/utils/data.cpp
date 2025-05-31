#include <utils/data.hpp>

capnp::MallocMessageBuilder global_message;
std::mutex data_mutex;

uint8_t buffer[TSDATA_BUFFER_SIZE];

void init_data(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.initRoot<TSData>();  // Must be called before read_data()
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
void set_time(uint32_t value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTime(value);
}

void set_timeBeforeTransmit(uint32_t value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setTimeBeforeTransmit(value);
}

void set_accessoryBatteryVoltage(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccessoryBatteryVoltage(value);
}
void set_accessoryBatteryCurrent(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setAccessoryBatteryCurrent(value);
}

void set_fuelCellOutputVoltage(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFuelCellOutputVoltage(value);
}

void set_fuelCellOutputCurrent(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setFuelCellOutputCurrent(value);
}

void set_supercapacitorVoltage(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSupercapacitorVoltage(value);
}

void set_supercapacitorCurrent(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setSupercapacitorCurrent(value);
}

void set_motorControllerSupplyVoltage(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMotorControllerSupplyVoltage(value);
}

void set_motorControllerSupplyCurrent(float value) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.getRoot<TSData>().setMotorControllerSupplyCurrent(value);
}

void set_fuelCellEnergyAccumulated(float value);

void set_h2PressureLow(float value);
void set_h2PressureFuelCell(float value);
void set_h2PressureHigh(float value);
void set_h2LeakageSensorVoltage(float value);

void set_fanDutyCycle(float value);
void set_blowerDutyCycle(float value);

void set_temperatureFuelCellLocation1(float value);
void set_temperatureFuelCellLocation2(float value);

void set_accelPedalVoltage(float value);
void set_brakePedalVoltage(float value);
void set_accelOutputVoltage(float value);
void set_brakeOutputVoltage(float value);

void set_buttonsMasterMask(uint8_t value);
void set_buttonsSteeringWheelMask(uint8_t value);

void set_sensorRpm(float value);
void set_sensorSpeed(float value);

void set_lapNumber(uint8_t value);
void set_lapTime(uint32_t value);

void set_gpsAltitude(float value);
void set_gpsLatitude(float value);
void set_gpsLongitude(float value);
void set_gpsSpeed(float value);

// Enums and bools
void set_masterState(uint16_t value);
void set_protiumState(uint16_t value);

void set_mainValveEnableOutput(bool value);
void set_motorControllerEnableOutput(bool value);

// Getters
uint32_t get_time(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTime();
}

uint32_t get_timeBeforeTransmit(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getTimeBeforeTransmit();
}

float get_accessoryBatteryVoltage(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    return global_message.getRoot<TSData>().getAccessoryBatteryVoltage();
}

float get_accessoryBatteryCurrent(void);

float get_fuelCellOutputVoltage(void);
float get_fuelCellOutputCurrent(void);

float get_supercapacitorVoltage(void);
float get_supercapacitorCurrent(void);

float get_motorControllerSupplyVoltage(void);
float get_motorControllerSupplyCurrent(void);

float get_fuelCellEnergyAccumulated(void);

float get_h2PressureLow(void);
float get_h2PressureFuelCell(void);
float get_h2PressureHigh(void);
float get_h2LeakageSensorVoltage(void);

float get_fanDutyCycle(void);
float get_blowerDutyCycle(void);

float get_temperatureFuelCellLocation1(void);
float get_temperatureFuelCellLocation2(void);

float get_accelPedalVoltage(void);
float get_brakePedalVoltage(void);
float get_accelOutputVoltage(void);
float get_brakeOutputVoltage(void);

uint8_t get_buttonsMasterMask(void);
uint8_t get_buttonsSteeringWheelMask(void);

float get_sensorRpm(void);
float get_sensorSpeed(void);

uint8_t get_lapNumber(void);
uint32_t get_lapTime(void);

float get_gpsAltitude(void);
float get_gpsLatitude(void);
float get_gpsLongitude(void);
float get_gpsSpeed(void);

uint16_t get_masterState(void);
uint16_t get_protiumState(void);

bool get_mainValveEnableOutput(void);
bool get_motorControllerEnableOutput(void);