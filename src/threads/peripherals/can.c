#include <threads/peripherals/can.h>

void *can(void *arg) {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    assert(sock > 0);

    struct sockaddr_can addr = { 0 };
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "can0");
    if(ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("CAN: ioctl call failed\n");
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("CAN: fcntl call failed\n");
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CAN: binding socket failed\n");
    }

    printf("CAN: Started successfully\n");

    while(true) {
        struct can_frame frame = { 0 };

        // TODO maybe allow for setting filters, to allow choosing a particular device
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("CAN: No data available, retrying...\n");
            } else {
                perror("CAN: read error");
            }
        } else {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n", frame.can_id, frame.can_dlc);
            set_fanRpm(*(int32_t *)frame.data);
        }
        
        sleep(1);
    }
}