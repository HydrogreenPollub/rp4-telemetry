#include <threads/peripherals/gps.hpp>
#include <utils/log.hpp>
#include <utils/status.hpp>

#include <format>

#include <array>
#include <chrono>
#include <cmath>
#include <optional>
#include <string>
#include <termios.h>

namespace asio = boost::asio;

extern std::atomic<bool> running;

namespace {

constexpr const char* GPS_DEVICE = "/dev/ttyUSB0";
constexpr auto GPS_RECONNECT_INTERVAL = std::chrono::seconds(5);
constexpr auto GPS_BAUD_PROBE_TIMEOUT = std::chrono::seconds(3);
constexpr auto GPS_READ_TIMEOUT = std::chrono::seconds(2);
constexpr auto GPS_STATUS_LOG_INTERVAL = std::chrono::seconds(30);
constexpr std::array<unsigned int, 2> GPS_BAUD_CANDIDATES = {9600, 115200};

void configure_serial_port(asio::serial_port& serial, unsigned int baud)
{
    serial.set_option(asio::serial_port::baud_rate(baud));
    serial.set_option(asio::serial_port::character_size(8));
    serial.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));
    ::tcflush(serial.native_handle(), TCIOFLUSH);
}

bool is_valid_nmea_line(const std::string& line)
{
    if (line.size() < 6 || line[0] != '$') {
        return false;
    }
    if (!minmea_check(line.c_str(), false)) {
        return false;
    }
    const enum minmea_sentence_id sentence_id = minmea_sentence_id(line.c_str(), false);
    return sentence_id != MINMEA_INVALID && sentence_id != MINMEA_UNKNOWN;
}

std::optional<std::string> read_nmea_line(
    asio::serial_port& serial,
    asio::io_context& io,
    asio::streambuf& buf,
    std::chrono::milliseconds timeout,
    boost::system::error_code& out_ec)
{
    std::optional<std::string> line;
    bool finished = false;
    asio::steady_timer timer(io);

    asio::async_read_until(
        serial,
        buf,
        "\r\n",
        [&](const boost::system::error_code& ec, std::size_t) {
            out_ec = ec;
            if (!ec) {
                std::istream input(&buf);
                std::string sentence;
                std::getline(input, sentence);
                if (!sentence.empty() && sentence.back() == '\r') {
                    sentence.pop_back();
                }
                line = std::move(sentence);
            }
            finished = true;
            timer.cancel();
        });

    timer.expires_after(timeout);
    timer.async_wait([&](const boost::system::error_code& ec) {
        if (!finished && !ec) {
            out_ec = asio::error::timed_out;
            serial.cancel();
        }
    });

    io.restart();
    io.run();
    io.restart();

    return line;
}

std::optional<unsigned int> probe_baudrate(asio::serial_port& serial, asio::io_context& io)
{
    asio::streambuf rx_buffer;

    for (unsigned int baud : GPS_BAUD_CANDIDATES) {
        configure_serial_port(serial, baud);
        rx_buffer.consume(rx_buffer.size());

        const auto deadline = std::chrono::steady_clock::now() + GPS_BAUD_PROBE_TIMEOUT;
        while (running.load(std::memory_order_relaxed)
            && std::chrono::steady_clock::now() < deadline) {
            const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now());
            if (remaining <= std::chrono::milliseconds(0)) {
                break;
            }

            const auto chunk = std::min(remaining, std::chrono::milliseconds(500));
            boost::system::error_code ec;
            const auto line = read_nmea_line(serial, io, rx_buffer, chunk, ec);
            if (line && is_valid_nmea_line(*line)) {
                return baud;
            }
            if (ec && ec != asio::error::operation_aborted) {
                break;
            }
        }
    }

    return std::nullopt;
}

bool sync_system_time_from_gps(const struct minmea_date& date, const struct minmea_time& gps_time)
{
    struct tm t = {};
    if (minmea_getdatetime(&t, &date, &gps_time) != 0) {
        return false;
    }

    const time_t epoch = timegm(&t);
    if (epoch == -1) {
        return false;
    }

    const time_t now = ::time(nullptr);
    if (now != -1 && std::abs(now - epoch) < 2) {
        return false;
    }

    struct timeval tv;
    tv.tv_sec = epoch;
    tv.tv_usec = 0;
    if (settimeofday(&tv, nullptr) != 0) {
        return false;
    }

    log("GPS", "System time synced from GPS fix");
    return true;
}

void process_nmea_line(const std::string& line)
{
    const enum minmea_sentence_id sentence_id = minmea_sentence_id(line.c_str(), false);
    switch (sentence_id) {
    case MINMEA_SENTENCE_GGA: {
        struct minmea_sentence_gga frame;
        if (minmea_parse_gga(&frame, line.c_str())) {
            set_gpsLatitude(minmea_tocoord(&frame.latitude));
            set_gpsLongitude(minmea_tocoord(&frame.longitude));
            set_gpsAltitude(minmea_tofloat(&frame.altitude));
        }
        break;
    }
    case MINMEA_SENTENCE_GLL: {
        struct minmea_sentence_gll frame;
        if (minmea_parse_gll(&frame, line.c_str())) {
            set_gpsLatitude(minmea_tocoord(&frame.latitude));
            set_gpsLongitude(minmea_tocoord(&frame.longitude));
        }
        break;
    }
    case MINMEA_SENTENCE_GSV:
        break;
    case MINMEA_SENTENCE_RMC: {
        struct minmea_sentence_rmc frame;
        if (minmea_parse_rmc(&frame, line.c_str())) {
            set_gpsLatitude(minmea_tocoord(&frame.latitude));
            set_gpsLongitude(minmea_tocoord(&frame.longitude));
            set_gpsSpeed(minmea_tofloat(&frame.speed) * 1.852);

            if (frame.valid) {
                sync_system_time_from_gps(frame.date, frame.time);
            }
        }
        break;
    }
    case MINMEA_SENTENCE_VTG: {
        struct minmea_sentence_vtg frame;
        if (minmea_parse_vtg(&frame, line.c_str())) {
            set_gpsSpeed(minmea_tofloat(&frame.speed_kph));
        }
        break;
    }
    case MINMEA_SENTENCE_ZDA:
        break;
    default:
        break;
    }
}

void log_gps_status(unsigned int baud)
{
    log("GPS", std::format(
        "status @ {} baud | lat={:.6f} lon={:.6f} alt={:.1f} spd={:.1f}",
        baud,
        get_gpsLatitude(),
        get_gpsLongitude(),
        get_gpsAltitude(),
        get_gpsSpeed()));
}

} // namespace

void* gps(void* arg)
{
    (void)arg;

    asio::io_context io;
    asio::serial_port serial(io);
    std::optional<unsigned int> active_baud;
    asio::streambuf rx_buffer;
    auto last_status_log = std::chrono::steady_clock::now();

    status_set_gps(false, 0);

    while (running.load(std::memory_order_relaxed)) {
        if (!serial.is_open()) {
            boost::system::error_code ec;
            serial.open(GPS_DEVICE, ec);
            if (ec) {
                log("GPS", "Device not available - " + ec.message() + ", retry in 5s");
                std::this_thread::sleep_for(GPS_RECONNECT_INTERVAL);
                continue;
            }

            active_baud = probe_baudrate(serial, io);
            if (!active_baud) {
                log("GPS", "No valid NMEA at 9600/115200, retry in 5s");
                serial.close();
                std::this_thread::sleep_for(GPS_RECONNECT_INTERVAL);
                continue;
            }

            log("GPS", "Connected at " + std::to_string(*active_baud) + " baud");
            status_set_gps(true, *active_baud);
            rx_buffer.consume(rx_buffer.size());
            last_status_log = std::chrono::steady_clock::now();
        }

        boost::system::error_code ec;
        const auto line = read_nmea_line(
            serial,
            io,
            rx_buffer,
            std::chrono::duration_cast<std::chrono::milliseconds>(GPS_READ_TIMEOUT),
            ec);

        if (!line) {
            if (ec && ec != asio::error::operation_aborted) {
                log("GPS", "Read error - " + ec.message() + ", reconnecting");
                serial.close();
                active_baud.reset();
                rx_buffer.consume(rx_buffer.size());
                status_set_gps(false, 0);
                continue;
            }
        } else {
            process_nmea_line(*line);
        }

        const auto now = std::chrono::steady_clock::now();
        if (active_baud && now - last_status_log >= GPS_STATUS_LOG_INTERVAL) {
            log_gps_status(*active_baud);
            last_status_log = now;
        }
    }

    if (serial.is_open()) {
        serial.close();
    }

    status_set_gps(false, 0);
    return nullptr;
}
