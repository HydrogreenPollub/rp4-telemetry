#include <threads/peripherals/gps.hpp>

#define GPS_DEVICE "/dev/ttyUSB0"

namespace asio = boost::asio;

void* gps(void* arg)
{
    (void)arg;

    asio::io_context io;
    asio::serial_port serial(io);

    serial.open(LORA_DEVICE);
    serial.set_option(asio::serial_port::baud_rate(115200));
    serial.set_option(asio::serial_port::character_size(8));
    serial.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

    while (true) {
        boost::asio::streambuf buf;
        boost::asio::read_until(serial, buf, "\r\n");  // NMEA sentences end with CRLF

        std::istream input(&buf);
        std::string line;
        std::getline(input, line);

        // TODO do we need to parse other types of sentences???
        enum minmea_sentence_id sentence_id = minmea_sentence_id(line, false);
        switch (sentence_id) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                set_gpsLatitude(minmea_tocoord(&frame.latitude));
                set_gpsLongitude(minmea_tocoord(&frame.longitude));
                set_gpsSpeed(minmea_tofloat(&frame.speed));
                // TODO add altitude and time
            }
            break;
        }
        default:
            std::cout << "GPS: Unhandled NMEA sentence - ID " << sentence_id << std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
