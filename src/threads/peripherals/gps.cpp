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

    serial.set_option(asio::serial_port::baud_rate(115200));
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

        // TODO add altitude and time to tsdata
        enum minmea_sentence_id sentence_id = minmea_sentence_id(line.c_str(), false);
        switch (sentence_id) {
        // Global Positioning System Fix Data
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line.c_str())) {
                std::cout << "GPS: GGA frame received at time - " << frame.time.hours << ":" << frame.time.minutes << ":" << frame.time.seconds << std::endl;

                // set_gpsLatitude(minmea_tocoord(&frame.latitude));
                // set_gpsLongitude(minmea_tocoord(&frame.longitude));
                // TODO add time
            }
            break;
        }
        // Geographic position, latitude and longitude
        case MINMEA_SENTENCE_GLL: {
            struct minmea_sentence_gll frame;
            if (minmea_parse_gll(&frame, line.c_str())) {
                // set_gpsLatitude(minmea_tocoord(&frame.latitude));
                // set_gpsLongitude(minmea_tocoord(&frame.longitude));
                // TODO add time
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
                // set_gpsLatitude(minmea_tocoord(&frame.latitude));
                // set_gpsLongitude(minmea_tocoord(&frame.longitude));
                // set_gpsSpeed(minmea_tofloat(&frame.speed) * 1.852); // Convert to km/h from knots
            }
            break;
        }
        // Track made good and ground speed
        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg frame;
            if (minmea_parse_vtg(&frame, line.c_str())) {
                //set_gpsSpeed(minmea_tofloat(&frame.speed_kph));
            }
            break;
        }
        default:
            std::cout << "GPS: Unhandled NMEA sentence - ID " << sentence_id << std::endl;
            break;
        }
    }
}
