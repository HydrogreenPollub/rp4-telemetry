#include <threads/peripherals/can.hpp>

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

    // Set the socket to non-blocking mode
    int flags = fcntl(can_socket, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("CAN: fcntl failed to get flags");
    }
    if (fcntl(can_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("CAN: fcntl failed to set socket non-blocking");
    }

    std::cout << "CAN: Binding the socket was successfull" << std::endl;
}

void* can(void* arg)
{
    (void)arg;

    // Initialize socket connection
    can_socket_bind();

    // Read from CAN bus
    struct can_frame frame;
    while (true) {
        // Read CAN bus
        int nbytes = ::read(can_socket, &frame, sizeof(frame));
        if (nbytes > 0) {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n",
                frame.can_id, frame.can_dlc);

            switch (frame.can_id) {
#ifndef CONFIG_USE_MASTER
            // Voltages and currents
            case CAN_ID_ACC_BATTERY_VOLTAGE:
                set_accessoryBatteryVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_ACC_BATTERY_CURRENT:
                set_accessoryBatteryCurrent(*(uint8_t*)frame.data);
                break;

            case CAN_ID_FC_OUTPUT_VOLTAGE:
                set_fuelCellOutputVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_FC_OUTPUT_CURRENT:
                set_fuelCellOutputCurrent(*(uint8_t*)frame.data);
                break;

            case CAN_ID_SC_VOLTAGE:
                set_supercapacitorVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_SC_CURRENT:
                set_supercapacitorCurrent(*(uint8_t*)frame.data);
                break;

            case CAN_ID_FC_ENERGY_ACCUMULATED:
                set_fuelCellEnergyAccumulated(*(uint8_t*)frame.data);
                break;

            // Hydrogen
            case CAN_ID_H2_PRESSURE_LOW:
                set_h2PressureLow(*(uint8_t*)frame.data);
                break;

            case CAN_ID_H2_PRESSURE_FUEL_CELL:
                set_h2PressureFuelCell(*(uint8_t*)frame.data);
                break;

            case CAN_ID_H2_PRESSURE_HIGH:
                set_h2PressureHigh(*(uint8_t*)frame.data);
                break;

            case CAN_ID_H2_LEAKAGE_SENSOR_VOLTAGE:
                set_h2LeakageSensorVoltage(*(uint8_t*)frame.data);
                break;

            // Fans
            case CAN_ID_FAN_DUTY_CYCLE:
                set_fanDutyCycle(*(uint8_t*)frame.data);
                break;

            case CAN_ID_BLOWER_DUTY_CYCLE:
                set_blowerDutyCycle(*(uint8_t*)frame.data);
                break;

            // Temperature sensors
            case CAN_ID_TEMP_FC_LOC1:
                set_temperatureFuelCellLocation1(*(uint8_t*)frame.data);
                break;

            case CAN_ID_TEMP_FC_LOC2:
                set_temperatureFuelCellLocation2(*(uint8_t*)frame.data);
                break;

            // Pedals
            case CAN_ID_ACCEL_PEDAL_VOLTAGE:
                set_accelPedalVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_BRAKE_PEDAL_VOLTAGE:
                set_brakePedalVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_ACCEL_OUTPUT_VOLTAGE:
                set_accelOutputVoltage(*(uint8_t*)frame.data);
                break;

            case CAN_ID_BRAKE_OUTPUT_VOLTAGE:
                set_brakeOutputVoltage(*(uint8_t*)frame.data);
                break;

            // RPM and speed
            case CAN_ID_SENSOR_RPM:
                set_sensorRpm(*(uint8_t*)frame.data);
                break;

            case CAN_ID_SENSOR_SPEED:
                set_sensorSpeed(*(uint8_t*)frame.data);
                break;

            // Other
            case CAN_ID_MAIN_VALVE_ENABLE_OUTPUT:
                set_mainValveEnableOutput(*(uint8_t*)frame.data);
                break;

            case CAN_ID_MC_ENABLE_OUTPUT:
                set_motorControllerEnableOutput(*(uint8_t*)frame.data);
                break;

#endif // CONFIG_USE_MASTER
            case CAN_ID_BUTTONS_STEERING_MASK:
                set_buttonsSteeringWheelMask(*(uint8_t*)frame.data);
                break;

            case CAN_ID_LAP_NUMBER:
                set_lapNumber(*(uint8_t*)frame.data);
                break;

            case CAN_ID_LAP_TIME:
                set_lapTime(*(uint32_t*)frame.data);
                break;

            default:
                std::cout << "CAN: can_id " << frame.can_id
                          << " is currently not supported" << std::endl;
                break;
            }
        }

#ifdef CONFIG_USE_MASTER
        // Send to CAN bus
        frame.can_id = CAN_ID_SENSOR_RPM;
        frame.can_dlc = sizeof(float);
        float sensorRpm = get_sensorRpm();
        std::memcpy(frame.data, &sensorRpm, sizeof(float));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_MASTER_STATE;
        frame.can_dlc = sizeof(uint16_t);
        uint16_t masterState = get_masterState();
        std::memcpy(frame.data, &masterState, sizeof(uint16_t));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_PROTIUM_STATE;
        frame.can_dlc = sizeof(uint16_t);
        uint16_t protiumState = get_protiumState();
        std::memcpy(frame.data, &protiumState, sizeof(uint16_t));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_SENSOR_SPEED;
        frame.can_dlc = sizeof(float);
        float sensorSpeed = get_sensorSpeed();
        std::memcpy(frame.data, &sensorSpeed, sizeof(float));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_FC_OUTPUT_VOLTAGE;
        frame.can_dlc = sizeof(float);
        float fuelCellOutputVoltage = get_fuelCellOutputVoltage();
        std::memcpy(frame.data, &fuelCellOutputVoltage, sizeof(float));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_SC_VOLTAGE;
        frame.can_dlc = sizeof(float);
        float supercapacitorVoltage = get_supercapacitorVoltage();
        std::memcpy(frame.data, &supercapacitorVoltage, sizeof(float));
        nbytes = ::write(can_socket, &frame, sizeof(frame));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        frame.can_id = CAN_ID_MC_SUPPLY_VOLTAGE;
        frame.can_dlc = sizeof(float);
        float motorControllerSupplyVoltage = get_motorControllerSupplyVoltage();
        std::memcpy(frame.data, &motorControllerSupplyVoltage, sizeof(float));
        nbytes = ::write(can_socket, &frame, sizeof(frame));
#endif // CONFIG_USE_MASTER

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
