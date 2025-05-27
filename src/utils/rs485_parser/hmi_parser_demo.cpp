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

//             spdlog::info("fuelCellOutputCurrent = {:.4f}", measurements.fuelCellOutputCurrent);
//             spdlog::info("supercapacitorCurrent = {:.4f}", measurements.supercapacitorCurrent);
//             spdlog::info("motorControllerSupply = {:.4f}", measurements.motorControllerSupply);
//             spdlog::info("ai4 = {:.4f}", measurements.ai4);
//             spdlog::info("ai5 = {:.4f}", measurements.ai5);
//             spdlog::info("accelPedalVoltage = {:.4f}", measurements.accelPedalVoltage);
//             spdlog::info("brakePedalVoltage = {:.4f}", measurements.brakePedalVoltage);
//             spdlog::info("accessoryBatteryCurrent = {:.4f}", measurements.accessoryBatteryCurrent);
//             spdlog::info("hydrogenHighPressure = {:.4f}", measurements.hydrogenHighPressure);
//             spdlog::info("hydrogenLeakageSensorVoltage = {:.4f}", measurements.hydrogenLeakageSensorVoltage);
//             spdlog::info("ai10 = {:.4f}", measurements.ai10);
//             spdlog::info("ai11 = {:.4f}", measurements.ai11);
//             spdlog::info("accessoryBatteryVoltage = {:.4f}", measurements.accessoryBatteryVoltage);
//             spdlog::info("fuelCellOutputVoltage = {:.4f}", measurements.fuelCellOutputVoltage);
//             spdlog::info("supercapacitorVoltage = {:.4f}", measurements.supercapacitorVoltage);
//             spdlog::info("motorControllerSupplyVoltage = {:.4f}", measurements.motorControllerSupplyVoltage);
            
//             spdlog::info("DIN = {:2x}", measurements.din);

//             spdlog::info("rpm = {:.4f}", measurements.rpm);
//             spdlog::info("speed = {:.4f}", measurements.speed);

//         };

//         parser.onMasterStatus = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const MasterStatus& status){
//             spdlog::info("Master status @ {}ms ({} cycles)", msClockTickCount, cycleClockTickCount);

//             spdlog::info("state = {}", magic_enum::enum_name(static_cast<MasterStatus::State>(status.state)));
//             spdlog::info("mainValveEnableOutput = {}", status.mainValveEnableOutput);
//             spdlog::info("motorControllerEnableOutput = {}", status.motorControllerEnableOutput);
//             spdlog::info("accelOutputVoltage = {:.4f}", status.accelOutputVoltage);
//             spdlog::info("brakeOutputVoltage = {:.4f}", status.brakeOutputVoltage);
//         };
        

//         parser.onProtiumValues = [](uint32_t msClockTickCount, uint32_t cycleClockTickCount, const ProtiumValues& values){
//             spdlog::info("Protium values @ {}ms ({} cycles)", msClockTickCount, cycleClockTickCount);

//             spdlog::info("FC_V = {:.4f}, FC_A = {:.4f}, FC_W = {:.4f}, Energy = {:.4f}, FCT1 = {:.4f}, FAN = {:.4f}", values.FC_V, values.FC_A, values.FC_W, values.Energy, values.FCT1, values.FAN);
//             spdlog::info("H2P1 = {:.4f}, H2P2 = {:.4f}, TankP = {:.4f}, TankT = {:.4f}, V_Set = {:.4f}, I_Set = {:.4f}", values.H2P1, values.H2P2, values.TankP, values.TankT, values.V_Set, values.I_Set);
//             spdlog::info("UCB_V = {:.4f}, Stasis_selector = {:.4f}, STASIS_V1 = {:.4f}, STASIS_V2 = {:.4f}, Number_of_cell = {}", values.UCB_V, values.Stasis_selector, values.STASIS_V1, values.STASIS_V2, values.Number_of_cell);
//             spdlog::info("FCT2 = {:.4f}, BLW = {:.4f}, BattV = {:.4f}, IP = {:.4f}, TP = {}", values.FCT2, values.BLW, values.BattV, values.IP, values.TP);
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

