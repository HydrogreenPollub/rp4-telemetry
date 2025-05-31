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

            // case CAN_ID_FUEL_CELL_TEMPERATURE:
            //     set_fuelCellTemperature(*(float*)frame.data);
            //     break;

            default:
                std::cout << "CAN: can_id " << frame.can_id
                          << " is currently not supported" << std::endl;
                break;
            }
        }

        // Send to CAN bus
        

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
