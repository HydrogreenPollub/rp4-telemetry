#include <threads/peripherals/can.h>
#include <threads/peripherals/gps.h>
#include <threads/peripherals/lora.h>
#include <threads/peripherals/rs485.h>
#include <utils/data.h>

#include <atomic>
#include <csignal>
#include <thread>
#include <iostream>

static std::atomic<bool> running(true);

std::thread can_thread;
std::thread gps_thread;
std::thread rs485_thread;
std::thread lora_thread;

void sigaction_handler(int signum)
{
    std::cout << "MAIN: Sigaction received - Closing program...\n";
    running = false; // TODO do we need a more advanced handler?
}

int main(int argc, char** argv)
{
    // Split program into multiple threads
    ::can_thread = std::thread(can, nullptr);
    ::gps_thread = std::thread(gps, nullptr);
    // ::rs485_thread = std::thread(rs485, nullptr);
    ::lora_thread = std::thread(lora, nullptr);

    struct sigaction sig = { };
    sig.sa_handler = sigaction_handler;
    sigaction(SIGINT, &sig, NULL);

    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (::can_thread.joinable()) ::can_thread.join();
    if (::gps_thread.joinable()) ::gps_thread.join();
    if (::rs485_thread.joinable()) ::rs485_thread.join();
    if (::lora_thread.joinable()) ::lora_thread.join();

    return EXIT_SUCCESS;
}
