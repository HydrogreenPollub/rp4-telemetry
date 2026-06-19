#include <threads/peripherals/can.hpp>
#include <utils/data.hpp>
#include <utils/log.hpp>
#include <utils/status.hpp>

#include <chrono>
#include <cstring>
#include <format>

extern std::atomic<bool> running;

namespace {

static int can_socket = 0;

static void can_socket_bind()
{
    can_socket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (!can_socket) {
        throw std::runtime_error("CAN: Creating socket failed");
    }

    struct sockaddr_can addr;
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0) {
        throw std::runtime_error("CAN: Call to ioctl failed");
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(can_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("CAN: Binding the socket failed");
    }

    int flags = fcntl(can_socket, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("CAN: fcntl failed to get flags");
    }
    if (fcntl(can_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("CAN: fcntl failed to set socket non-blocking");
    }

    log("CAN", "Socket bound to can0");
}

// Repurposed old ts_data slots (protium stack removed, schema unchanged):
//   h2PressureFuelCell          <- MCU_STATE vehicle_type + vehicle_allowed_to_move * 100
//   fanDutyCycle                <- FCCU_STATE fan_duty (%)
//   fuelCellEnergyAccumulated   <- FCCU_THERMAL bme76_humidity (%)
//   blowerDutyCycle             <- FCCU_HYDROGEN flow_rate (Ln/min)
//   protiumState                <- FCCU_STATE running_state

static uint16_t map_mcu_status_to_master_state(uint8_t can_status)
{
    switch (can_status) {
    case CANDEF_MCU_STATE_STATUS_RUNNING_CHOICE:
        return 1;
    case CANDEF_MCU_STATE_STATUS_SHUTTING_DOWN_CHOICE:
        return 2;
    case CANDEF_MCU_STATE_STATUS_INIT_CHOICE:
    case CANDEF_MCU_STATE_STATUS_IDLE_CHOICE:
    case CANDEF_MCU_STATE_STATUS_STARTING_CHOICE:
        return 0;
    default:
        return 3;
    }
}

static void handle_can_frame(const struct can_frame& frame, CanRxStats& stats)
{
    const uint32_t can_id = frame.can_id & CAN_EFF_MASK;
    stats.record(can_id);

    switch (can_id) {
    case CANDEF_MCU_STATE_FRAME_ID: {
        struct candef_mcu_state_t msg {};
        if (candef_mcu_state_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_masterState(map_mcu_status_to_master_state(msg.status));
        set_mainValveEnableOutput(msg.main_valve_enabled != 0);
        set_motorControllerEnableOutput(msg.motor_controller_enabled != 0);
        set_h2PressureFuelCell(static_cast<float>(msg.vehicle_type)
            + static_cast<float>(msg.vehicle_allowed_to_move) * 100.0f);
        break;
    }

    case CANDEF_MCU_INPUTS_FRAME_ID: {
        struct candef_mcu_inputs_t msg {};
        if (candef_mcu_inputs_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        uint8_t packed[1] = {};
        candef_mcu_inputs_pack(packed, &msg, sizeof(packed));
        set_buttonsMasterMask(packed[0]);
        break;
    }

    case CANDEF_MCU_ANALOG_PEDALS_FRAME_ID: {
        struct candef_mcu_analog_pedals_t msg {};
        candef_mcu_analog_pedals_unpack(&msg, frame.data, frame.can_dlc);
        set_accelPedalVoltage(static_cast<float>(candef_mcu_analog_pedals_acceleration_pedal_voltage_decode(msg.acceleration_pedal_voltage)));
        set_brakePedalVoltage(static_cast<float>(candef_mcu_analog_pedals_brake_pedal_voltage_decode(msg.brake_pedal_voltage)));
        break;
    }

    case CANDEF_MCU_ANALOG_FUEL_CELL_FRAME_ID: {
        struct candef_mcu_analog_fuel_cell_t msg {};
        candef_mcu_analog_fuel_cell_unpack(&msg, frame.data, frame.can_dlc);
        set_fuelCellOutputCurrent(static_cast<float>(candef_mcu_analog_fuel_cell_fuel_cell_output_current_decode(msg.fuel_cell_output_current)));
        set_fuelCellOutputVoltage(static_cast<float>(candef_mcu_analog_fuel_cell_fuel_cell_output_voltage_decode(msg.fuel_cell_output_voltage)));
        set_h2LeakageSensorVoltage(static_cast<float>(candef_mcu_analog_fuel_cell_hydrogen_leakage_sensor_voltage_decode(msg.hydrogen_leakage_sensor_voltage)));
        break;
    }

    case CANDEF_MCU_ANALOG_POWERTRAIN_FRAME_ID: {
        struct candef_mcu_analog_powertrain_t msg {};
        candef_mcu_analog_powertrain_unpack(&msg, frame.data, frame.can_dlc);
        set_supercapacitorVoltage(static_cast<float>(candef_mcu_analog_powertrain_supercapacitor_voltage_decode(msg.supercapacitor_voltage)));
        set_supercapacitorCurrent(static_cast<float>(candef_mcu_analog_powertrain_supercapacitor_current_decode(msg.supercapacitor_current)));
        set_motorControllerSupplyCurrent(static_cast<float>(candef_mcu_analog_powertrain_motor_controller_supply_current_decode(msg.motor_controller_supply_current)));
        set_motorControllerSupplyVoltage(static_cast<float>(candef_mcu_analog_powertrain_motor_controller_supply_voltage_decode(msg.motor_controller_supply_voltage)));
        break;
    }

    case CANDEF_MCU_ANALOG_ACCESSORY_FRAME_ID: {
        struct candef_mcu_analog_accessory_t msg {};
        candef_mcu_analog_accessory_unpack(&msg, frame.data, frame.can_dlc);
        set_accessoryBatteryVoltage(static_cast<float>(candef_mcu_analog_accessory_accessory_battery_voltage_decode(msg.accessory_battery_voltage)));
        set_accessoryBatteryCurrent(static_cast<float>(candef_mcu_analog_accessory_accessory_battery_current_decode(msg.accessory_battery_current)));
        break;
    }

    case CANDEF_MCU_ANALOG_DRIVE_FRAME_ID: {
        struct candef_mcu_analog_drive_t msg {};
        candef_mcu_analog_drive_unpack(&msg, frame.data, frame.can_dlc);
        set_sensorRpm(static_cast<float>(candef_mcu_analog_drive_rpm_decode(msg.rpm)));
        set_sensorSpeed(static_cast<float>(candef_mcu_analog_drive_speed_decode(msg.speed)));
        break;
    }

    case CANDEF_MCU_ANALOG_OUTPUTS_FRAME_ID: {
        struct candef_mcu_analog_outputs_t msg {};
        if (candef_mcu_analog_outputs_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_accelOutputVoltage(static_cast<float>(
            candef_mcu_analog_outputs_acceleration_output_voltage_decode(msg.acceleration_output_voltage)));
        set_brakeOutputVoltage(static_cast<float>(
            candef_mcu_analog_outputs_brake_output_voltage_decode(msg.brake_output_voltage)));
        break;
    }

    case CANDEF_FCCU_STATE_FRAME_ID: {
        struct candef_fccu_state_t msg {};
        if (candef_fccu_state_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_temperatureFuelCellLocation1(static_cast<float>(
            candef_fccu_state_fc_temp_c_decode(msg.fc_temp_c)));
        set_fanDutyCycle(static_cast<float>(
            candef_fccu_state_fan_duty_decode(msg.fan_duty)));
        set_protiumState(static_cast<uint16_t>(msg.running_state));
        break;
    }

    case CANDEF_FCCU_HYDROGEN_FRAME_ID: {
        struct candef_fccu_hydrogen_t msg {};
        if (candef_fccu_hydrogen_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_h2PressureLow(static_cast<float>(
            candef_fccu_hydrogen_lp_pressure_decode(msg.lp_pressure)));
        set_h2PressureHigh(static_cast<float>(
            candef_fccu_hydrogen_hp_pressure_decode(msg.hp_pressure)));
        set_blowerDutyCycle(static_cast<float>(
            candef_fccu_hydrogen_flow_rate_decode(msg.flow_rate)));
        break;
    }

    case CANDEF_FCCU_THERMAL_FRAME_ID: {
        struct candef_fccu_thermal_t msg {};
        if (candef_fccu_thermal_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_temperatureFuelCellLocation2(static_cast<float>(
            candef_fccu_thermal_bme76_temp_decode(msg.bme76_temp)));
        set_fuelCellEnergyAccumulated(static_cast<float>(
            candef_fccu_thermal_bme76_humidity_decode(msg.bme76_humidity)));
        break;
    }

    case CANDEF_SWU_LIGHTS_FRAME_ID:
        set_buttonsSteeringWheelMask(frame.data[0]);
        break;

    case CANDEF_SWU_TIME_FRAME_ID: {
        struct candef_swu_time_t msg {};
        if (candef_swu_time_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_lapTime(static_cast<uint64_t>(msg.lap_time_ms));
        break;
    }

    case CANDEF_SWU_STATE_FRAME_ID: {
        struct candef_swu_state_t msg {};
        if (candef_swu_state_unpack(&msg, frame.data, frame.can_dlc) < 0) {
            break;
        }
        set_lapNumber(static_cast<uint8_t>(msg.lap_number));
        break;
    }

    case CANDEF_MCU_FAULTS_FRAME_ID:
    case CANDEF_MCU_ANALOG_UNASSIGNED_FRAME_ID:
        break;

    default:
        break;
    }
}

static void log_can_summary(const CanRxStats& stats)
{
    log("CAN", std::format(
        "1s: rx={} unknown={} | STATE={} FAULTS={} DRIVE={} PT={} FC={} ACC={} "
        "PEDALS={} INPUTS={} FCCU={} SWU_TIME={} SWU_STATE={} | fc_v={:.1f} fc_i={:.2f} "
        "sc_v={:.1f} sc_i={:.2f} mc_v={:.1f} mc_i={:.2f} ab_v={:.1f} state={} lap={} lap_ms={}",
        stats.total,
        stats.unknown,
        stats.mcu_state,
        stats.mcu_faults,
        stats.mcu_drive,
        stats.mcu_powertrain,
        stats.mcu_fuel_cell,
        stats.mcu_accessory,
        stats.mcu_pedals,
        stats.mcu_inputs,
        stats.fccu,
        stats.swu_time,
        stats.swu_state,
        get_fuelCellOutputVoltage(),
        get_fuelCellOutputCurrent(),
        get_supercapacitorVoltage(),
        get_supercapacitorCurrent(),
        get_motorControllerSupplyVoltage(),
        get_motorControllerSupplyCurrent(),
        get_accessoryBatteryVoltage(),
        get_masterState(),
        get_lapNumber(),
        get_lapTime()));
}

} // namespace

void* can(void* arg)
{
    (void)arg;

    can_socket_bind();

    struct can_frame frame;
    CanRxStats stats;
    auto last_report = std::chrono::steady_clock::now();

    while (running.load(std::memory_order_relaxed)) {
        bool received = false;

        while (running.load(std::memory_order_relaxed)) {
            const int nbytes = ::read(can_socket, &frame, sizeof(frame));
            if (nbytes <= 0) {
                break;
            }
            received = true;
            handle_can_frame(frame, stats);
        }

        const auto now = std::chrono::steady_clock::now();
        if (now - last_report >= std::chrono::seconds(1)) {
            status_set_can_rx(stats);
            log_can_summary(stats);
            stats.reset();
            last_report = now;
        }

#ifdef CONFIG_USE_MASTER
        uint8_t buf[8] = {};
        int nbytes = 0;

        struct candef_mcu_analog_drive_t drive_msg;
        candef_mcu_analog_drive_init(&drive_msg);
        drive_msg.rpm = candef_mcu_analog_drive_rpm_encode(get_sensorRpm());
        drive_msg.speed = candef_mcu_analog_drive_speed_encode(get_sensorSpeed());
        candef_mcu_analog_drive_pack(buf, &drive_msg, sizeof(buf));
        frame.can_id = CANDEF_MCU_ANALOG_DRIVE_FRAME_ID;
        frame.can_dlc = CANDEF_MCU_ANALOG_DRIVE_LENGTH;
        std::memcpy(frame.data, buf, CANDEF_MCU_ANALOG_DRIVE_LENGTH);
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        struct candef_mcu_state_t state_msg;
        candef_mcu_state_init(&state_msg);
        state_msg.status = candef_mcu_state_status_encode(get_masterState());
        state_msg.motor_controller_enabled = candef_mcu_state_motor_controller_enabled_encode(get_motorControllerEnableOutput());
        state_msg.main_valve_enabled = candef_mcu_state_main_valve_enabled_encode(get_mainValveEnableOutput());
        candef_mcu_state_pack(buf, &state_msg, sizeof(buf));
        frame.can_id = CANDEF_MCU_STATE_FRAME_ID;
        frame.can_dlc = CANDEF_MCU_STATE_LENGTH;
        std::memcpy(frame.data, buf, CANDEF_MCU_STATE_LENGTH);
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        struct candef_protium_state_t protium_state_msg;
        candef_protium_state_init(&protium_state_msg);
        protium_state_msg.operating_state = candef_protium_state_operating_state_encode(get_protiumState());
        candef_protium_state_pack(buf, &protium_state_msg, sizeof(buf));
        frame.can_id = CANDEF_PROTIUM_STATE_FRAME_ID;
        frame.can_dlc = CANDEF_PROTIUM_STATE_LENGTH;
        std::memcpy(frame.data, buf, CANDEF_PROTIUM_STATE_LENGTH);
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        struct candef_mcu_analog_fuel_cell_t fc_msg;
        candef_mcu_analog_fuel_cell_init(&fc_msg);
        fc_msg.fuel_cell_output_voltage = candef_mcu_analog_fuel_cell_fuel_cell_output_voltage_encode(get_fuelCellOutputVoltage());
        candef_mcu_analog_fuel_cell_pack(buf, &fc_msg, sizeof(buf));
        frame.can_id = CANDEF_MCU_ANALOG_FUEL_CELL_FRAME_ID;
        frame.can_dlc = CANDEF_MCU_ANALOG_FUEL_CELL_LENGTH;
        std::memcpy(frame.data, buf, CANDEF_MCU_ANALOG_FUEL_CELL_LENGTH);
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        struct candef_mcu_analog_powertrain_t pt_msg;
        candef_mcu_analog_powertrain_init(&pt_msg);
        pt_msg.supercapacitor_voltage = candef_mcu_analog_powertrain_supercapacitor_voltage_encode(get_supercapacitorVoltage());
        pt_msg.motor_controller_supply_voltage = candef_mcu_analog_powertrain_motor_controller_supply_voltage_encode(get_motorControllerSupplyVoltage());
        candef_mcu_analog_powertrain_pack(buf, &pt_msg, sizeof(buf));
        frame.can_id = CANDEF_MCU_ANALOG_POWERTRAIN_FRAME_ID;
        frame.can_dlc = CANDEF_MCU_ANALOG_POWERTRAIN_LENGTH;
        std::memcpy(frame.data, buf, CANDEF_MCU_ANALOG_POWERTRAIN_LENGTH);
        nbytes = ::write(can_socket, &frame, sizeof(frame));
#endif // CONFIG_USE_MASTER

        if (!received) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    return nullptr;
}
