#include <threads/peripherals/can.hpp>
#include <threads/peripherals/csv.hpp>
#include <threads/peripherals/gps.hpp>
#include <threads/peripherals/lora.hpp>
#include <threads/peripherals/rs485.hpp>
#include <utils/data.hpp>
#include <utils/rtc_watchdog/DEV_Config.h>
#include <utils/rtc_watchdog/DS3231.h>

#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

static std::atomic<bool> running(true);

void sigaction_handler(int signum)
{
    (void)signum;
    std::cout << "MAIN: Sigaction received - Closing program...\n";
    running = false; // TODO do we need a more advanced handler?
}

int main()
{
    // Initialize capnp
    init_data();

    // Initialize RTC
    if (DEV_ModuleInit() == 1)
        return 1;

    DEV_I2C_Init(DS3231_Address);

    // TODO maybe add temperature readout from DS3231
    DS3231_SET_Hour_Mode(24);
    DS3231_SET_Calendar(2137, 12, 31);
    DS3231_SET_Day(7);
    DS3231_SET_Time(21, 37, 00);

    // Split program into multiple threads
    std::thread can_thread = std::thread(Can {});
    std::thread csv_thread = std::thread(csv, nullptr);
    std::thread gps_thread = std::thread(gps, nullptr);
    std::thread rs485_thread = std::thread(rs485, nullptr);
    std::thread lora_thread = std::thread(lora, nullptr);

    struct sigaction sig = {};
    sig.sa_handler = sigaction_handler;
    sigaction(SIGINT, &sig, NULL);

    while (running) {
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

    return EXIT_SUCCESS;
}
