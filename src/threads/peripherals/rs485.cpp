#include <threads/peripherals/rs485.hpp>

#define GPIO_CHIP "/dev/gpiochip0"
#define RS485_DEVICE "/dev/ttySC0"

#define RS485_TXDEN_1 27
#define RS485_TXDEN_2 22

namespace asio = boost::asio;

void* rs485(void* arg)
{
    (void)arg;

    // Setup gpiod
    const ::gpiod::line::offsets line_offsets = { RS485_TXDEN_1, RS485_TXDEN_2 };

    // Set both TXDEN to OUTPUT
    auto request = ::gpiod::chip(GPIO_CHIP)
                       .prepare_request()
                       .set_consumer("rs485-TXDEN")
                       .add_line_settings(
                           line_offsets,
                           ::gpiod::line_settings().set_direction(
                               ::gpiod::line::direction::OUTPUT))
                       .do_request();
    // TXDEN_1 decides if we're reading or writing
    // 0 - Transmit
    // 1 - Receive
    request.set_value(RS485_TXDEN_1, ::gpiod::line::value::ACTIVE);

    // Create the parser
    HmiParser parser;

    parser.onMasterMeasurements = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterMeasurements& measurements) {
        std::cout << "HMI: Master measurements @ " << msClockTickCount << " ms (" << cycleClockTickCount << " cycles)" << std::endl;
        (void)measurements;
    };

    parser.onMasterStatus = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterStatus& status) {
        std::cout << "HMI: Master status @ " << msClockTickCount << "ms (" << cycleClockTickCount << " cycles)" << std::endl;

        std::cout << "HMI: state = " << static_cast<MasterStatus::State>(status.state) << std::endl;
        std::cout << "HMI: mainValveEnableOutput = " << status.mainValveEnableOutput << std::endl;
        std::cout << "HMI: motorControllerEnableOutput = " << status.motorControllerEnableOutput << std::endl;
        std::cout << "HMI: accelOutputVoltage = " << status.accelOutputVoltage << std::endl;
        std::cout << "HMI: brakeOutputVoltage = " << status.brakeOutputVoltage << std::endl;
    };

    parser.onProtiumValues = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const ProtiumValues& values) {
        std::cout << "HMI: Protium measurements @ " << msClockTickCount << " ms (" << cycleClockTickCount << " cycles)";

        set_fcVoltage(values.FC_V);
        set_fcCurrent(values.FC_A);
    };

    parser.onProtiumOperatingState = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, ProtiumOperatingState currentOperatingState, const ProtiumOperatingStateLogEntry(&operatingStateLogEntries)[8]) {
        std::cout << "HMI: Protium operating state @ " << msClockTickCount << "ms (" << cycleClockTickCount << " cycles)" << std::endl;
        std::cout << "HMI: Current opearting state: " << currentOperatingState << std::endl;
        std::cout << "HMI: Operating state history:" << std::endl;

        std::size_t i = 0;
        for (auto& entry : operatingStateLogEntries) {
            std::cout << "HMI: history[" << i++ << "] @ {" << entry.msClockTickCount << "}ms -> " << static_cast<ProtiumOperatingState>(entry.state);
        }
    };

    // Open the device
    asio::io_context io;
    asio::serial_port serial(io);

    serial.open(RS485_DEVICE);
    serial.set_option(asio::serial_port::baud_rate(921600));
    serial.set_option(asio::serial_port::character_size(8));
    serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

    std::byte buffer[1] = {};

    // TODO maybe add some way to break out of the loop to free the resources...
    while (true) {
        std::size_t bytes_received = serial.read_some(asio::buffer(buffer));

        if (bytes_received != 1) {
            throw std::runtime_error("RS485: Reader failed - read more than 1 bytes");
        }

        parser.processOctet((uint8_t)buffer[0]);
    }
}
