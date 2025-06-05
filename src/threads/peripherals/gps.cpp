#include <threads/peripherals/gps.hpp>

#define GPS_DEVICE "/dev/ttyUSB0"

namespace asio = boost::asio;

void* gps(void* arg)
{
    (void)arg;

    asio::io_context io;
    asio::serial_port serial(io);

    boost::system::error_code ec;
    serial.open(GPS_DEVICE, ec);

    if (ec) {
        std::cout << "GPS: Failed to open device - " << ec.message() << std::endl;
        return nullptr;
    }

#ifdef CONFIG_GPS_9600
    serial.set_option(asio::serial_port::baud_rate(9600));
#elif CONFIG_GPS_115200
    serial.set_option(asio::serial_port::baud_rate(115200));
#endif
    serial.set_option(asio::serial_port::character_size(8));
    serial.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

    while (true) {
        boost::asio::streambuf buf;
        boost::asio::read_until(serial, buf, "\r\n"); // NMEA sentences end with CRLF

        std::istream input(&buf);
        std::string line;
        std::getline(input, line);

        enum minmea_sentence_id sentence_id = minmea_sentence_id(line.c_str(), false);
        switch (sentence_id) {
        // Global Positioning System Fix Data
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line.c_str())) {
                std::cout << "GPS: GGA frame received at time - " << frame.time.hours << ":" << frame.time.minutes << ":" << frame.time.seconds << std::endl;
                set_gpsLatitude(minmea_tocoord(&frame.latitude));
                set_gpsLongitude(minmea_tocoord(&frame.longitude));
                set_gpsAltitude(minmea_tofloat(&frame.altitude));
            }
            break;
        }
        // Geographic position, latitude and longitude
        case MINMEA_SENTENCE_GLL: {
            struct minmea_sentence_gll frame;
            if (minmea_parse_gll(&frame, line.c_str())) {
                set_gpsLatitude(minmea_tocoord(&frame.latitude));
                set_gpsLongitude(minmea_tocoord(&frame.longitude));
            }
            break;
        }
        // GPS Satellites in view
        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, line.c_str())) {
                std::cout << "GPS: Total number of satellites visible - " << frame.total_sats << std::endl;
            }
            break;
        }
        // Recommended minimum specific GPS
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line.c_str())) {
                set_gpsLatitude(minmea_tocoord(&frame.latitude));
                set_gpsLongitude(minmea_tocoord(&frame.longitude));
                set_gpsSpeed(minmea_tofloat(&frame.speed) * 1.852); // Convert to km/h from knots

                // Set time
                struct tm t = {};
                t.tm_year = frame.date.year + 100; // Years since 1900
                t.tm_mon = frame.date.month - 1; // Months since January
                t.tm_mday = frame.date.day;
                t.tm_hour = frame.time.hours;
                t.tm_min = frame.time.minutes;
                t.tm_sec = frame.time.seconds;

                time_t epoch = timegm(&t); // Convert to Unix time (UTC)
                if (epoch != -1) {
                    struct timeval tv;
                    tv.tv_sec = epoch;
                    tv.tv_usec = 0;

                    settimeofday(&tv, nullptr);
                }
            }
            break;
        }
        // Track made good and ground speed
        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg frame;
            if (minmea_parse_vtg(&frame, line.c_str())) {
                set_gpsSpeed(minmea_tofloat(&frame.speed_kph));
            }
            break;
        }
        // Time and date
        case MINMEA_SENTENCE_ZDA: {
            struct minmea_sentence_zda frame;
            if (minmea_parse_zda(&frame, line.c_str())) {
                struct tm t = {};
                t.tm_year = frame.date.year + 100; // Years since 1900
                t.tm_mon = frame.date.month - 1; // Months since January
                t.tm_mday = frame.date.day;
                t.tm_hour = frame.time.hours;
                t.tm_min = frame.time.minutes;
                t.tm_sec = frame.time.seconds;

                time_t epoch = timegm(&t); // Convert to Unix time (UTC)
                if (epoch != -1) {
                    struct timeval tv;
                    tv.tv_sec = epoch;
                    tv.tv_usec = 0;

                    settimeofday(&tv, nullptr);
                }
            }
            break;
        }
        default:
            std::cout << "GPS: Unhandled NMEA sentence - ID " << sentence_id << std::endl;
            break;
        }
    }
}
