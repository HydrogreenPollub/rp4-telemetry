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

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("CAN: binding socket failed\n");
    }

    printf("CAN: Started successfully\n");

    struct can_frame frame = { 0 };
    while(true) {
        // TODO maybe allow for setting filters, to allow choosing a particular device
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("CAN: read error");
        } else {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n", frame.can_id, frame.can_dlc);

            // TODO add more IDs once tested
            switch(frame.can_id) {
                case CAN_ID_IS_EMERGENCY:
                    set_isEmergency(*(bool *)frame.data);
                    break;

                case CAN_ID_FC_CURRENT:
                    set_fcCurrent(*(int32_t *)frame.data);             
                    break;

                default:
                    printf("CAN: can_id %d is currently not supported\n", frame.can_id);
                    break;
            }
        }
        
        usleep(500);
    }
}