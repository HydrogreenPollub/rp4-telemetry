#include <threads/peripherals/gps.h>

#define GPS_DEVICE "/dev/ttyUSB0"

void* gps(void* arg)
{
    (void)arg;
    FILE* gps = fopen(GPS_DEVICE, "r");
    if (gps == NULL) {
        perror("GPS: Couldn't open device file...\n");
        return NULL;
    }

    char line[MINMEA_MAX_SENTENCE_LENGTH] = { 0 };

    while (true) {
        fgets(line, sizeof(line), gps);

        // TODO do we need to parse other types of sentences???
        enum minmea_sentence_id sentence_id = minmea_sentence_id(line, false);
        switch (sentence_id) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                set_gpsLatitude(minmea_tocoord(&frame.latitude));
                set_gpsLongitude(minmea_tocoord(&frame.longitude));
                set_gpsSpeed(minmea_tofloat(&frame.speed));
            }
            break;
        }
        default:
            printf("GPS: Unhandled NMEA sentence - ID %d\n", sentence_id);
            break;
        }

        usleep(200000); // 200ms
    }
}
