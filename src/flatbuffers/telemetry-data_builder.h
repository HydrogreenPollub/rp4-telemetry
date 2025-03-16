#ifndef TELEMETRY-DATA_BUILDER_H
#define TELEMETRY-DATA_BUILDER_H

/* Generated by flatcc 0.6.2 FlatBuffers schema compiler for C by dvide.com */

#ifndef TELEMETRY-DATA_READER_H
#include "telemetry-data_reader.h"
#endif
#ifndef FLATBUFFERS_COMMON_BUILDER_H
#include "flatbuffers_common_builder.h"
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif

#define __FuelCellMode_formal_args , FuelCellMode_enum_t v0
#define __FuelCellMode_call_args , v0
__flatbuffers_build_scalar(flatbuffers_, FuelCellMode, FuelCellMode_enum_t)

#define __TSData_formal_args ,\
  flatbuffers_bool_t v0, flatbuffers_bool_t v1, flatbuffers_bool_t v2, flatbuffers_bool_t v3,\
  flatbuffers_bool_t v4, flatbuffers_bool_t v5, flatbuffers_bool_t v6, flatbuffers_bool_t v7,\
  FuelCellMode_enum_t v8, float v9, float v10, float v11,\
  float v12, float v13, float v14, float v15,\
  int32_t v16, float v17, int32_t v18, float v19,\
  float v20, float v21, float v22, float v23,\
  float v24, float v25, float v26, float v27, float v28, uint8_t v29
#define __TSData_call_args ,\
  v0, v1, v2, v3,\
  v4, v5, v6, v7,\
  v8, v9, v10, v11,\
  v12, v13, v14, v15,\
  v16, v17, v18, v19,\
  v20, v21, v22, v23,\
  v24, v25, v26, v27, v28, v29
static inline TSData_t *TSData_assign(TSData_t *p,
  flatbuffers_bool_t v0, flatbuffers_bool_t v1, flatbuffers_bool_t v2, flatbuffers_bool_t v3,
  flatbuffers_bool_t v4, flatbuffers_bool_t v5, flatbuffers_bool_t v6, flatbuffers_bool_t v7,
  FuelCellMode_enum_t v8, float v9, float v10, float v11,
  float v12, float v13, float v14, float v15,
  int32_t v16, float v17, int32_t v18, float v19,
  float v20, float v21, float v22, float v23,
  float v24, float v25, float v26, float v27, float v28, uint8_t v29)
{ p->isEmergency = v0; p->isEmergencyButtonPressed = v1; p->isEmergencySwitchToggled = v2; p->isHydrogenLeaking = v3;
  p->isScRelayClosed = v4; p->isTimeResetButtonPressed = v5; p->isHalfSpeedButtonPressed = v6; p->isGasButtonPressed = v7;
  p->fuelCellMode = v8; p->fcCurrent = v9; p->fcScCurrent = v10; p->scMotorCurrent = v11;
  p->fcVoltage = v12; p->scVoltage = v13; p->hydrogenSensorVoltage = v14; p->fuelCellTemperature = v15;
  p->fanRpm = v16; p->vehicleSpeed = v17; p->motorPwm = v18; p->hydrogenPressure = v19;
  p->gpsLatitude = v20; p->gpsLongitude = v21; p->gpsAltitude = v22; p->gpsSpeed = v23;
  p->motorSpeed = v24; p->motorCurrent = v25; p->fcCurrentRaw = v26; p->fcVoltageRaw = v27;
  p->mcCurrent = v28; p->lapNumber = v29;
  return p; }
static inline TSData_t *TSData_copy(TSData_t *p, const TSData_t *p2)
{ p->isEmergency = p2->isEmergency; p->isEmergencyButtonPressed = p2->isEmergencyButtonPressed; p->isEmergencySwitchToggled = p2->isEmergencySwitchToggled; p->isHydrogenLeaking = p2->isHydrogenLeaking;
  p->isScRelayClosed = p2->isScRelayClosed; p->isTimeResetButtonPressed = p2->isTimeResetButtonPressed; p->isHalfSpeedButtonPressed = p2->isHalfSpeedButtonPressed; p->isGasButtonPressed = p2->isGasButtonPressed;
  p->fuelCellMode = p2->fuelCellMode; p->fcCurrent = p2->fcCurrent; p->fcScCurrent = p2->fcScCurrent; p->scMotorCurrent = p2->scMotorCurrent;
  p->fcVoltage = p2->fcVoltage; p->scVoltage = p2->scVoltage; p->hydrogenSensorVoltage = p2->hydrogenSensorVoltage; p->fuelCellTemperature = p2->fuelCellTemperature;
  p->fanRpm = p2->fanRpm; p->vehicleSpeed = p2->vehicleSpeed; p->motorPwm = p2->motorPwm; p->hydrogenPressure = p2->hydrogenPressure;
  p->gpsLatitude = p2->gpsLatitude; p->gpsLongitude = p2->gpsLongitude; p->gpsAltitude = p2->gpsAltitude; p->gpsSpeed = p2->gpsSpeed;
  p->motorSpeed = p2->motorSpeed; p->motorCurrent = p2->motorCurrent; p->fcCurrentRaw = p2->fcCurrentRaw; p->fcVoltageRaw = p2->fcVoltageRaw;
  p->mcCurrent = p2->mcCurrent; p->lapNumber = p2->lapNumber;
  return p; }
static inline TSData_t *TSData_assign_to_pe(TSData_t *p,
  flatbuffers_bool_t v0, flatbuffers_bool_t v1, flatbuffers_bool_t v2, flatbuffers_bool_t v3,
  flatbuffers_bool_t v4, flatbuffers_bool_t v5, flatbuffers_bool_t v6, flatbuffers_bool_t v7,
  FuelCellMode_enum_t v8, float v9, float v10, float v11,
  float v12, float v13, float v14, float v15,
  int32_t v16, float v17, int32_t v18, float v19,
  float v20, float v21, float v22, float v23,
  float v24, float v25, float v26, float v27, float v28, uint8_t v29)
{ p->isEmergency = v0; p->isEmergencyButtonPressed = v1; p->isEmergencySwitchToggled = v2; p->isHydrogenLeaking = v3;
  p->isScRelayClosed = v4; p->isTimeResetButtonPressed = v5; p->isHalfSpeedButtonPressed = v6; p->isGasButtonPressed = v7;
  p->fuelCellMode = v8; flatbuffers_float_assign_to_pe(&p->fcCurrent, v9); flatbuffers_float_assign_to_pe(&p->fcScCurrent, v10); flatbuffers_float_assign_to_pe(&p->scMotorCurrent, v11);
  flatbuffers_float_assign_to_pe(&p->fcVoltage, v12); flatbuffers_float_assign_to_pe(&p->scVoltage, v13); flatbuffers_float_assign_to_pe(&p->hydrogenSensorVoltage, v14); flatbuffers_float_assign_to_pe(&p->fuelCellTemperature, v15);
  flatbuffers_int32_assign_to_pe(&p->fanRpm, v16); flatbuffers_float_assign_to_pe(&p->vehicleSpeed, v17); flatbuffers_int32_assign_to_pe(&p->motorPwm, v18); flatbuffers_float_assign_to_pe(&p->hydrogenPressure, v19);
  flatbuffers_float_assign_to_pe(&p->gpsLatitude, v20); flatbuffers_float_assign_to_pe(&p->gpsLongitude, v21); flatbuffers_float_assign_to_pe(&p->gpsAltitude, v22); flatbuffers_float_assign_to_pe(&p->gpsSpeed, v23);
  flatbuffers_float_assign_to_pe(&p->motorSpeed, v24); flatbuffers_float_assign_to_pe(&p->motorCurrent, v25); flatbuffers_float_assign_to_pe(&p->fcCurrentRaw, v26); flatbuffers_float_assign_to_pe(&p->fcVoltageRaw, v27);
  flatbuffers_float_assign_to_pe(&p->mcCurrent, v28); p->lapNumber = v29;
  return p; }
static inline TSData_t *TSData_copy_to_pe(TSData_t *p, const TSData_t *p2)
{ p->isEmergency = p2->isEmergency; p->isEmergencyButtonPressed = p2->isEmergencyButtonPressed; p->isEmergencySwitchToggled = p2->isEmergencySwitchToggled; p->isHydrogenLeaking = p2->isHydrogenLeaking;
  p->isScRelayClosed = p2->isScRelayClosed; p->isTimeResetButtonPressed = p2->isTimeResetButtonPressed; p->isHalfSpeedButtonPressed = p2->isHalfSpeedButtonPressed; p->isGasButtonPressed = p2->isGasButtonPressed;
  p->fuelCellMode = p2->fuelCellMode; flatbuffers_float_copy_to_pe(&p->fcCurrent, &p2->fcCurrent); flatbuffers_float_copy_to_pe(&p->fcScCurrent, &p2->fcScCurrent); flatbuffers_float_copy_to_pe(&p->scMotorCurrent, &p2->scMotorCurrent);
  flatbuffers_float_copy_to_pe(&p->fcVoltage, &p2->fcVoltage); flatbuffers_float_copy_to_pe(&p->scVoltage, &p2->scVoltage); flatbuffers_float_copy_to_pe(&p->hydrogenSensorVoltage, &p2->hydrogenSensorVoltage); flatbuffers_float_copy_to_pe(&p->fuelCellTemperature, &p2->fuelCellTemperature);
  flatbuffers_int32_copy_to_pe(&p->fanRpm, &p2->fanRpm); flatbuffers_float_copy_to_pe(&p->vehicleSpeed, &p2->vehicleSpeed); flatbuffers_int32_copy_to_pe(&p->motorPwm, &p2->motorPwm); flatbuffers_float_copy_to_pe(&p->hydrogenPressure, &p2->hydrogenPressure);
  flatbuffers_float_copy_to_pe(&p->gpsLatitude, &p2->gpsLatitude); flatbuffers_float_copy_to_pe(&p->gpsLongitude, &p2->gpsLongitude); flatbuffers_float_copy_to_pe(&p->gpsAltitude, &p2->gpsAltitude); flatbuffers_float_copy_to_pe(&p->gpsSpeed, &p2->gpsSpeed);
  flatbuffers_float_copy_to_pe(&p->motorSpeed, &p2->motorSpeed); flatbuffers_float_copy_to_pe(&p->motorCurrent, &p2->motorCurrent); flatbuffers_float_copy_to_pe(&p->fcCurrentRaw, &p2->fcCurrentRaw); flatbuffers_float_copy_to_pe(&p->fcVoltageRaw, &p2->fcVoltageRaw);
  flatbuffers_float_copy_to_pe(&p->mcCurrent, &p2->mcCurrent); p->lapNumber = p2->lapNumber;
  return p; }
static inline TSData_t *TSData_assign_from_pe(TSData_t *p,
  flatbuffers_bool_t v0, flatbuffers_bool_t v1, flatbuffers_bool_t v2, flatbuffers_bool_t v3,
  flatbuffers_bool_t v4, flatbuffers_bool_t v5, flatbuffers_bool_t v6, flatbuffers_bool_t v7,
  FuelCellMode_enum_t v8, float v9, float v10, float v11,
  float v12, float v13, float v14, float v15,
  int32_t v16, float v17, int32_t v18, float v19,
  float v20, float v21, float v22, float v23,
  float v24, float v25, float v26, float v27, float v28, uint8_t v29)
{ p->isEmergency = v0; p->isEmergencyButtonPressed = v1; p->isEmergencySwitchToggled = v2; p->isHydrogenLeaking = v3;
  p->isScRelayClosed = v4; p->isTimeResetButtonPressed = v5; p->isHalfSpeedButtonPressed = v6; p->isGasButtonPressed = v7;
  p->fuelCellMode = v8; flatbuffers_float_assign_from_pe(&p->fcCurrent, v9); flatbuffers_float_assign_from_pe(&p->fcScCurrent, v10); flatbuffers_float_assign_from_pe(&p->scMotorCurrent, v11);
  flatbuffers_float_assign_from_pe(&p->fcVoltage, v12); flatbuffers_float_assign_from_pe(&p->scVoltage, v13); flatbuffers_float_assign_from_pe(&p->hydrogenSensorVoltage, v14); flatbuffers_float_assign_from_pe(&p->fuelCellTemperature, v15);
  flatbuffers_int32_assign_from_pe(&p->fanRpm, v16); flatbuffers_float_assign_from_pe(&p->vehicleSpeed, v17); flatbuffers_int32_assign_from_pe(&p->motorPwm, v18); flatbuffers_float_assign_from_pe(&p->hydrogenPressure, v19);
  flatbuffers_float_assign_from_pe(&p->gpsLatitude, v20); flatbuffers_float_assign_from_pe(&p->gpsLongitude, v21); flatbuffers_float_assign_from_pe(&p->gpsAltitude, v22); flatbuffers_float_assign_from_pe(&p->gpsSpeed, v23);
  flatbuffers_float_assign_from_pe(&p->motorSpeed, v24); flatbuffers_float_assign_from_pe(&p->motorCurrent, v25); flatbuffers_float_assign_from_pe(&p->fcCurrentRaw, v26); flatbuffers_float_assign_from_pe(&p->fcVoltageRaw, v27);
  flatbuffers_float_assign_from_pe(&p->mcCurrent, v28); p->lapNumber = v29;
  return p; }
static inline TSData_t *TSData_copy_from_pe(TSData_t *p, const TSData_t *p2)
{ p->isEmergency = p2->isEmergency; p->isEmergencyButtonPressed = p2->isEmergencyButtonPressed; p->isEmergencySwitchToggled = p2->isEmergencySwitchToggled; p->isHydrogenLeaking = p2->isHydrogenLeaking;
  p->isScRelayClosed = p2->isScRelayClosed; p->isTimeResetButtonPressed = p2->isTimeResetButtonPressed; p->isHalfSpeedButtonPressed = p2->isHalfSpeedButtonPressed; p->isGasButtonPressed = p2->isGasButtonPressed;
  p->fuelCellMode = p2->fuelCellMode; flatbuffers_float_copy_from_pe(&p->fcCurrent, &p2->fcCurrent); flatbuffers_float_copy_from_pe(&p->fcScCurrent, &p2->fcScCurrent); flatbuffers_float_copy_from_pe(&p->scMotorCurrent, &p2->scMotorCurrent);
  flatbuffers_float_copy_from_pe(&p->fcVoltage, &p2->fcVoltage); flatbuffers_float_copy_from_pe(&p->scVoltage, &p2->scVoltage); flatbuffers_float_copy_from_pe(&p->hydrogenSensorVoltage, &p2->hydrogenSensorVoltage); flatbuffers_float_copy_from_pe(&p->fuelCellTemperature, &p2->fuelCellTemperature);
  flatbuffers_int32_copy_from_pe(&p->fanRpm, &p2->fanRpm); flatbuffers_float_copy_from_pe(&p->vehicleSpeed, &p2->vehicleSpeed); flatbuffers_int32_copy_from_pe(&p->motorPwm, &p2->motorPwm); flatbuffers_float_copy_from_pe(&p->hydrogenPressure, &p2->hydrogenPressure);
  flatbuffers_float_copy_from_pe(&p->gpsLatitude, &p2->gpsLatitude); flatbuffers_float_copy_from_pe(&p->gpsLongitude, &p2->gpsLongitude); flatbuffers_float_copy_from_pe(&p->gpsAltitude, &p2->gpsAltitude); flatbuffers_float_copy_from_pe(&p->gpsSpeed, &p2->gpsSpeed);
  flatbuffers_float_copy_from_pe(&p->motorSpeed, &p2->motorSpeed); flatbuffers_float_copy_from_pe(&p->motorCurrent, &p2->motorCurrent); flatbuffers_float_copy_from_pe(&p->fcCurrentRaw, &p2->fcCurrentRaw); flatbuffers_float_copy_from_pe(&p->fcVoltageRaw, &p2->fcVoltageRaw);
  flatbuffers_float_copy_from_pe(&p->mcCurrent, &p2->mcCurrent); p->lapNumber = p2->lapNumber;
  return p; }
__flatbuffers_build_struct(flatbuffers_, TSData, 96, 4, TSData_file_identifier, TSData_type_identifier)
__flatbuffers_define_fixed_array_primitives(flatbuffers_, TSData, TSData_t)

#include "flatcc/flatcc_epilogue.h"
#endif /* TELEMETRY-DATA_BUILDER_H */
