#include <threads/peripherals/gps.h>

#define GPS_DEVICE "/dev/ttyUSB0"

void *gps(void *arg) {
    FILE *gps = fopen(GPS_DEVICE, "r");
    //assert(gps != NULL); // TODO add a way to decide which peripherals are enabled
    
    char line[MINMEA_MAX_SENTENCE_LENGTH];

    while(true) {
        fgets(line, sizeof(line), gps);

        // TODO do we need to parse other types of sentences???
        enum minmea_sentence_id sentence_id = minmea_sentence_id(line, false);
        switch(sentence_id) {
            case MINMEA_SENTENCE_RMC:
                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, line)) {
                    set_gpsLatitude(minmea_tocoord(&frame.latitude));
                    set_gpsLongitude(minmea_tocoord(&frame.longitude));
                    set_gpsSpeed(minmea_tofloat(&frame.speed));
                }
                break;
            default:
                fprintf(stdout, "GPS: Unhandled NMEA sentence - ID %d\n", sentence_id);
                break;
        }

        inform_watchdog(*(uint8_t *)arg);

        // TODO should this thread sleep?
    }
}