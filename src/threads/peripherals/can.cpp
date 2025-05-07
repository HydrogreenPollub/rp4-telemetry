#include "can_ids/can_ids.h"
#include "data.h"
#include <threads/peripherals/can.hpp>

void Can::bind()
{
    this->socket = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (!this->socket) {
        throw std::runtime_error("CAN: Creating socket failed");
    }

    struct sockaddr_can addr;
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(this->socket, SIOCGIFINDEX, &ifr) < 0) {
        throw std::runtime_error("CAN: Call to ioctl failed");
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(this->socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("CAN: Binding the socket failed");
    }

    std::cout << "CAN: Binding the socket was successfull" << std::endl;
}

void Can::operator()()
{
    // Initialize socket connection
    this->bind();

    // Read from CAN bus
    struct can_frame frame;
    while (true) {
        int nbytes = ::read(this->socket, &frame, sizeof(frame));
        if (nbytes < 0) {
            throw std::runtime_error("CAN: Reading failed");
        } else {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n",
                frame.can_id, frame.can_dlc);

            // TODO add more IDs once tested
            switch (frame.can_id) {
            case CAN_ID_IS_EMERGENCY:
                set_isEmergency(*(bool*)frame.data);
                break;

            case CAN_ID_IS_EMERGENCY_BUTTON_PRESSED:
                set_isEmergencyButtonPressed(*(bool*)frame.data);
                break;

            case CAN_ID_IS_EMERGENCY_SWITCH_TOGGLED:
                set_isEmergencySwitchToggled(*(bool*)frame.data);
                break;

            case CAN_ID_IS_HYDROGEN_LEAKING:
                set_isHydrogenLeaking(*(bool*)frame.data);
                break;

            case CAN_ID_IS_SC_RELAY_CLOSED:
                set_isScRelayClosed(*(bool*)frame.data);
                break;

            case CAN_ID_IS_TIME_RESET_BUTTON_PRESSED:
                set_isTimeResetButtonPressed(*(bool*)frame.data);
                break;

            case CAN_ID_IS_HALF_SPEED_BUTTON_PRESSED:
                set_isHalfSpeedButtonPressed(*(bool*)frame.data);
                break;

            case CAN_ID_IS_GAS_BUTTON_PRESSED:
                set_isGasButtonPressed(*(bool*)frame.data);
                break;

            case CAN_ID_FUEL_CELL_MODE:
                set_fuelCellMode(*(enum TSData_FuelCellMode*)frame.data);
                break;

            case CAN_ID_FC_CURRENT:
                set_fcCurrent(*(float*)frame.data);
                break;

            case CAN_ID_FC_SC_CURRENT:
                set_fcScCurrent(*(float*)frame.data);
                break;

            case CAN_ID_SC_MOTOR_CURRENT:
                set_scMotorCurrent(*(float*)frame.data);
                break;

            case CAN_ID_FC_VOLTAGE:
                set_fcVoltage(*(float*)frame.data);
                break;

            case CAN_ID_SC_VOLTAGE:
                set_scVoltage(*(float*)frame.data);
                break;

            case CAN_ID_HYDROGEN_SENSOR_VOLTAGE:
                set_hydrogenSensorVoltage(*(float*)frame.data);
                break;

            case CAN_ID_FUEL_CELL_TEMPERATURE:
                set_fuelCellTemperature(*(float*)frame.data);
                break;

            case CAN_ID_VEHICLE_SPEED:
                set_vehicleSpeed(*(float*)frame.data);
                break;

            case CAN_ID_HYDROGEN_PRESSURE:
                set_hydrogenPressure(*(float*)frame.data);
                break;

            default:
                std::cout << "CAN: can_id " << frame.can_id
                          << " is currently not supported" << std::endl;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
