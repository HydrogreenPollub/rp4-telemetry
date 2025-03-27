#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#include <utils/proto/capnp_c.h>
#include <utils/proto/ts_data.capnp.h>

#define TSDATA_BUFFER_SIZE 1024

// Provides setter prototypes for all of the fields
#define XSET_TSDATA(FIELD, TYPE) void set_##FIELD(TYPE value);
#include <utils/ts_data.def>
#undef XSET_TSDATA

// void set_isEmergency(bool state);
// void set_isEmergencyButtonPressed(bool state);
// void set_isEmergencySwitchToggled(bool state);
// void set_isHydrogenLeaking(bool state);
// void set_isScRelayClosed(bool state);
// void set_isTimeResetButtonPressed(bool state);
// void set_isHalfSpeedButtonPressed(bool state);
// void set_isGasButtonPressed(bool state);

// void set_fuelCellMode(enum TSData_FuelCellMode mode);

// void set_fcCurrent(float fcCurrent);
// void set_fcScCurrent(float fcScCurrent);
// void set_scMotorCurrent(float scMotorCurrent);
// void set_fcVoltage(float fcVoltage);
// void set_scVoltage(float scVoltage);
// void set_hydrogenSensorVoltage(float hydrogenSensorVoltage);
// void set_fuelCellTemperature(float fuelCellTemperature);
// void set_vehicleSpeed(float vehicleSpeed);

// TODO replace all these functions with an X macro
void set_fanRpm(int32_t fanRpm);
void set_motorPwm(int32_t motorPwm);

const uint8_t *read_data(void);

#endif