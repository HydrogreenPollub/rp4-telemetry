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
        // Read CAN bus
        int nbytes = ::read(this->socket, &frame, sizeof(frame));
        if (nbytes < 0) {
            throw std::runtime_error("CAN: Reading failed");
        } else {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n",
                frame.can_id, frame.can_dlc);

            // TODO add more IDs once tested
            switch (frame.can_id) {
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

        // Send to CAN bus
        frame.can_id = CAN_ID_SENSOR_RPM;
        frame.can_dlc = sizeof(float);
        float sensorRpm = get_sensorRpm();
        std::memcpy(frame.data, &sensorRpm, sizeof(float));
        nbytes = ::write(this->socket, &frame, sizeof(frame));

        frame.can_id = CAN_ID_MASTER_STATE;
        frame.can_dlc = sizeof(uint16_t);
        uint16_t masterState = get_masterState();
        std::memcpy(frame.data, &masterState, sizeof(uint16_t));
        nbytes = ::write(this->socket, &frame, sizeof(frame));

        frame.can_id = CAN_ID_PROTIUM_STATE;
        frame.can_id = CAN_ID_SENSOR_SPEED;
        frame.can_id = CAN_ID_FC_OUTPUT_VOLTAGE;
        frame.can_id = CAN_ID_SC_VOLTAGE;
        frame.can_id = CAN_ID_MC_SUPPLY_VOLTAGE;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
