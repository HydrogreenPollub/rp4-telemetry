#include <threads/peripherals/lora.hpp>

#define LORA_DEVICE "/dev/ttyS0"
constexpr size_t FRAME_SIZE = TSDATA_BUFFER_SIZE + 4;

namespace asio = boost::asio;

void* lora(void* arg)
{
    (void)arg;

    asio::io_context io;
    asio::serial_port serial(io);

    serial.open(LORA_DEVICE);
    serial.set_option(asio::serial_port::baud_rate(9600));
    serial.set_option(asio::serial_port::character_size(8));
    serial.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

    // while (true) {
    //     boost::system::error_code ec;
    //     serial.open(LORA_DEVICE, ec);
    //     if (!ec) {
    //         std::cout << "LORA: Serial port opened successfully.\n";
    //         break;
    //     } else {
    //         std::cerr << "LORA: Failed to open serial port: " << ec.message() << ". Retrying...\n";
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //     }
    // }

    std::vector<uint8_t> frame(FRAME_SIZE);

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        set_time(static_cast<uint64_t>(timestamp));
        set_timeBeforeTransmit(static_cast<uint64_t>(timestamp));

        const uint8_t* data = read_data();

        // Frame structure: [0xFF][data][0xEE]
        frame[0] = 0xFF;
        std::copy(data, data + TSDATA_BUFFER_SIZE, frame.begin() + 1);
        frame[TSDATA_BUFFER_SIZE + 1] = 0xEE;
        frame[TSDATA_BUFFER_SIZE + 2] = '\n';
        frame[TSDATA_BUFFER_SIZE + 3] = '\r';

        printf("LORA: Sending buffer - ");
        for (int i = 0; i < TSDATA_BUFFER_SIZE; ++i) {
            printf("%02X ", data[i]);
        }
        printf("\n");

        serial.write_some(asio::buffer(frame));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return nullptr;
}
