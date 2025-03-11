#include <threads/watchdog.h>
#include <threads/peripherals/can.h>
#include <utils/data.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

void *can(void *arg) {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    assert(sock > 0);

    struct sockaddr_can addr = { 0 };
    struct ifreq ifr;

    strcpy(ifr.ifr_name, "can0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    printf("CAN: Started successfully\n");

    while(true) {
        struct can_frame frame = { 0 };

        // TODO maybe allow for setting filters, to allow choosing a particular device
        int nbytes = read(sock, &frame, sizeof(frame));
    
        if(nbytes > 0) {
            printf("CAN: Reading packet from bus: can_id = 0x%X, can_dlc = %d\n", frame.can_id, frame.can_dlc);
        }

        update_x(nbytes);
        update_y(420);

        inform_watchdog((int)arg);

        sleep(1);
    }
}