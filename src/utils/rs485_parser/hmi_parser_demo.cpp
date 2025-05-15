// #include <csignal>
// #include <atomic>

// #include <asio.hpp>

// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>

// #include <magic_enum/magic_enum.hpp>

// #include "hmi_parser.hpp"

// using namespace std::literals;

// std::atomic<int> stopSignal = 0;
// static_assert(decltype(stopSignal)::is_always_lock_free);

// void signalHandler(int signal)
// {
//     stopSignal = signal;
// }


// int main(int argc, char* argv[])
// {
//     try{
//         std::shared_ptr consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
//         std::shared_ptr fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("hmi-parser-log.txt", false);

//         spdlog::logger logger(std::string(), {fileSink, consoleSink});
//         spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
//         spdlog::flush_on(spdlog::level::trace);        

//         for (int sig=1; sig<NSIG; ++sig)
//             std::signal(sig, signalHandler);

//         if (argc != 2){
//             spdlog::error("No port name specified (example usage: hmi-parser-demo.exe COM1   hmi-parser-demo /dev/ttySC0)");
//             return EXIT_FAILURE;
//         }

        
//         HmiParser parser;


//         parser.onMasterMeasurements = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterMeasurements& measurements){
//             spdlog::info("Master measurements @ {}ms ({} cycles)", msClockTickCount, cycleClockTickCount);

//             spdlog::info("ai1 = {:.4f}, ai2 = {:.4f}, ai3 = {:.4f}, ai4 = {:.4f}, ai5 = {:.4f}, ai6 = {:.4f}, ai7 = {:.4f}, ISupply = {:.4f}", 
//                 measurements.ai1, measurements.ai2, measurements.ai3, measurements.ai4, measurements.ai5, measurements.ai6, measurements.ai7, measurements.isupply);

//             spdlog::info("ai9 = {:.4f}, ai10 = {:.4f}, ai11 = {:.4f}, ai12 = {:.4f}, VSupply = {:.4f}, hvi1 = {:.4f}, hvi2 = {:.4f}, hvi3 = {:.4f}", 
//                 measurements.ai9, measurements.ai10, measurements.ai11, measurements.ai12, measurements.vsupply, measurements.hvi1, measurements.hvi2, measurements.hvi3);

//             spdlog::info("DIN = {:2x}", measurements.din);

//         };


//         parser.onProtiumValues = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const ProtiumValues& values){
//             spdlog::info("Protium values @ {}ms ({} cycles)", msClockTickCount, cycleClockTickCount);

//             spdlog::info("FC_V = {:.4f}, FC_A = {:.4f}, FC_W = {:.4f}, Energy = {:.4f}, FCT1 = {:.4f}, FAN = {:.4f}", values.FC_V, values.FC_A, values.FC_W, values.Energy, values.FCT1, values.FAN);
//             spdlog::info("H2P1 = {:.4f}, H2P2 = {:.4f}, TankP = {:.4f}, TankT = {:.4f}, V_Set = {:.4f}, I_Set = {:.4f}", values.H2P1, values.H2P2, values.TankP, values.TankT, values.V_Set, values.I_Set);
//             spdlog::info("UCB_V = {:.4f}, Stasis_selector = {:.4f}, STASIS_V1 = {:.4f}, STASIS_V2 = {:.4f}, Number_of_cell = {}", values.UCB_V, values.Stasis_selector, values.STASIS_V1, values.STASIS_V2, values.Number_of_cell);
//         };


//         parser.onProtiumOperatingState = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, ProtiumOperatingState currentOperatingState, const ProtiumOperatingStateLogEntry (&operatingStateLogEntries)[8]){
//             spdlog::info("Protium operating state @ {}ms ({} cycles)", msClockTickCount, cycleClockTickCount);
//             spdlog::info("Current opearting state: {}", magic_enum::enum_name(currentOperatingState));
//             spdlog::info("Operating state history:");

//             std::size_t i=0;
//             for (auto& entry: operatingStateLogEntries){
//                 spdlog::info(" history[{}] @ {:10}ms -> {}", i++, entry.msClockTickCount, magic_enum::enum_name(static_cast<ProtiumOperatingState>(entry.state)));
//             }
//         };


//         asio::io_context io;
//         asio::serial_port serial(io);

//         serial.open(std::string(argv[1]));
//         serial.set_option(asio::serial_port::baud_rate(115200*8));
//         serial.set_option(asio::serial_port::character_size(8));
//         serial.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
//         serial.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));

//         std::byte buffer[1] = {};
//         asio::error_code ec = {};

//         while(!stopSignal){
//             std::size_t bytesTransferred = serial.read_some(asio::buffer(buffer), ec);

//             if (ec){
//                 spdlog::error("Port read error: {}", ec.message());
//                 return EXIT_FAILURE;
//             }

//             if (bytesTransferred != 1){
//                 spdlog::error("Read error");
//                 return EXIT_FAILURE;
//             }

//             parser.processOctet((uint8_t)buffer[0]);
//         }

//         return EXIT_SUCCESS;
//     }
//     catch(std::exception& e){
//         spdlog::critical("Exception: {}", e.what());
//     }
//     catch(...){
//         spdlog::critical("Unknown exception!");
//     }

//     return EXIT_FAILURE;    
// }

