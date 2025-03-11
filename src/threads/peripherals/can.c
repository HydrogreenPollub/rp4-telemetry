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
#include <fcntl.h>
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
    
    // Set the socket to non-blocking mode
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

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
        }

        update_x(nbytes);
        update_y(420);

        inform_watchdog((int)arg);

        sleep(1);
    }
}