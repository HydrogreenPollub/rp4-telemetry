#include <threads/peripherals/can.hpp>
#include <threads/peripherals/csv.hpp>
#include <threads/peripherals/gps.hpp>
#include <threads/peripherals/lora.hpp>
#include <threads/peripherals/rs485.hpp>
#include <utils/data.hpp>
#include <utils/status.hpp>

#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

std::atomic<bool> running(true);

void sigaction_handler(int signum)
{
    (void)signum;
    running.store(false, std::memory_order_relaxed);
}

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string(argv[1]) == "--version") {
        std::cout << "Telemetry version: " << TELEMETRY_VERSION << "\n";
        return EXIT_SUCCESS;
    }

    if (!status_try_acquire_primary_lock()) {
        status_print_cached();
        return EXIT_SUCCESS;
    }

    init_data();

    std::thread can_thread = std::thread(can, nullptr);
    std::thread csv_thread = std::thread(csv, nullptr);
    std::thread gps_thread = std::thread(gps, nullptr);
    std::thread rs485_thread = std::thread(rs485, nullptr);
    std::thread lora_thread = std::thread(lora, nullptr);

    struct sigaction sig = {};
    sig.sa_handler = sigaction_handler;
    sigaction(SIGINT, &sig, NULL);

    while (running.load(std::memory_order_relaxed)) {
        status_publish();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (can_thread.joinable())
        can_thread.join();
    if (csv_thread.joinable())
        csv_thread.join();
    if (gps_thread.joinable())
        gps_thread.join();
    if (rs485_thread.joinable())
        rs485_thread.join();
    if (lora_thread.joinable())
        lora_thread.join();

    status_release_primary_lock();
    return EXIT_SUCCESS;
}
