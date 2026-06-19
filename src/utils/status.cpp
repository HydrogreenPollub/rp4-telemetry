#include <utils/status.hpp>

#include <utils/can-definition/candef.h>
#include <utils/data.hpp>

#include <chrono>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <format>
#include <iostream>
#include <mutex>
#include <sys/file.h>
#include <unistd.h>

namespace {

constexpr const char* LOG_DIR = "/home/root/logs";
constexpr const char* LOCK_PATH = "/home/root/logs/telemetry.lock";
constexpr const char* STATUS_PATH = "/home/root/logs/telemetry_status.txt";

int g_lock_fd = -1;
std::mutex g_status_mutex;
CanRxStats g_can_rx {};
bool g_gps_connected = false;
unsigned int g_gps_baud = 0;
std::chrono::steady_clock::time_point g_started_at = std::chrono::steady_clock::now();

void ensure_log_dir()
{
    std::filesystem::create_directories(LOG_DIR);
}

std::string format_snapshot(const CanRxStats& can, bool gps_connected, unsigned int gps_baud)
{
    const auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - g_started_at);

    return std::format(
        "Telemetry {} (uptime {}s)\n"
        "CAN last 1s: rx={} unknown={} | STATE={} FAULTS={} DRIVE={} PT={} FC={} ACC={} "
        "PEDALS={} INPUTS={} FCCU={} SWU_TIME={} SWU_STATE={}\n"
        "Values: fc_v={:.1f}V fc_i={:.2f}A sc_v={:.1f}V sc_i={:.2f}A mc_v={:.1f}V mc_i={:.2f}A "
        "ab_v={:.1f}V | master={} protium={} rpm={:.0f} speed={:.1f} | lap={} lap_ms={}\n"
        "GPS: {}\n",
        TELEMETRY_VERSION,
        uptime.count(),
        can.total,
        can.unknown,
        can.mcu_state,
        can.mcu_faults,
        can.mcu_drive,
        can.mcu_powertrain,
        can.mcu_fuel_cell,
        can.mcu_accessory,
        can.mcu_pedals,
        can.mcu_inputs,
        can.fccu,
        can.swu_time,
        can.swu_state,
        get_fuelCellOutputVoltage(),
        get_fuelCellOutputCurrent(),
        get_supercapacitorVoltage(),
        get_supercapacitorCurrent(),
        get_motorControllerSupplyVoltage(),
        get_motorControllerSupplyCurrent(),
        get_accessoryBatteryVoltage(),
        get_masterState(),
        get_protiumState(),
        get_sensorRpm(),
        get_sensorSpeed(),
        get_lapNumber(),
        get_lapTime(),
        gps_connected
            ? std::format("connected @ {} baud | lat={:.6f} lon={:.6f} spd={:.1f}",
                gps_baud,
                get_gpsLatitude(),
                get_gpsLongitude(),
                get_gpsSpeed())
            : std::string("not connected"));
}

} // namespace

void CanRxStats::record(uint32_t can_id)
{
    ++total;
    switch (can_id) {
    case CANDEF_MCU_STATE_FRAME_ID:
        ++mcu_state;
        break;
    case CANDEF_MCU_FAULTS_FRAME_ID:
        ++mcu_faults;
        break;
    case CANDEF_MCU_INPUTS_FRAME_ID:
        ++mcu_inputs;
        break;
    case CANDEF_MCU_ANALOG_PEDALS_FRAME_ID:
        ++mcu_pedals;
        break;
    case CANDEF_MCU_ANALOG_FUEL_CELL_FRAME_ID:
        ++mcu_fuel_cell;
        break;
    case CANDEF_MCU_ANALOG_POWERTRAIN_FRAME_ID:
        ++mcu_powertrain;
        break;
    case CANDEF_MCU_ANALOG_ACCESSORY_FRAME_ID:
        ++mcu_accessory;
        break;
    case CANDEF_MCU_ANALOG_DRIVE_FRAME_ID:
        ++mcu_drive;
        break;
    case CANDEF_MCU_ANALOG_OUTPUTS_FRAME_ID:
        ++mcu_pedals;
        break;
    case CANDEF_MCU_ANALOG_UNASSIGNED_FRAME_ID:
        ++mcu_unassigned;
        break;
    case CANDEF_FCCU_STATE_FRAME_ID:
    case CANDEF_FCCU_HYDROGEN_FRAME_ID:
    case CANDEF_FCCU_ENVIRONMENT_FRAME_ID:
        ++fccu;
        break;
    case CANDEF_FCCU_POWER_FRAME_ID:
    case CANDEF_FCCU_CURRENTS_FRAME_ID:
        break;
    case CANDEF_SWU_LIGHTS_FRAME_ID:
        ++swu_lights;
        break;
    case CANDEF_SWU_TIME_FRAME_ID:
        ++swu_time;
        break;
    case CANDEF_SWU_STATE_FRAME_ID:
        ++swu_state;
        break;
    case CANDEF_MCU_TIME_SYNC_FRAME_ID:
    case CANDEF_CCU_STATUS_FRAME_ID:
    case CANDEF_MCU_LIGHTING_FRAME_ID:
    case CANDEF_LCU_STATUS_FRAME_ID:
        ++other;
        break;
    default:
        ++unknown;
        break;
    }
}

void CanRxStats::reset()
{
    *this = {};
}

bool status_try_acquire_primary_lock()
{
    ensure_log_dir();

    g_lock_fd = ::open(LOCK_PATH, O_CREAT | O_RDWR, 0644);
    if (g_lock_fd < 0) {
        return false;
    }

    if (::flock(g_lock_fd, LOCK_EX | LOCK_NB) != 0) {
        ::close(g_lock_fd);
        g_lock_fd = -1;
        return false;
    }

    g_started_at = std::chrono::steady_clock::now();
    return true;
}

void status_release_primary_lock()
{
    if (g_lock_fd >= 0) {
        ::flock(g_lock_fd, LOCK_UN);
        ::close(g_lock_fd);
        g_lock_fd = -1;
    }
}

void status_set_can_rx(const CanRxStats& stats)
{
    std::lock_guard<std::mutex> lock(g_status_mutex);
    g_can_rx = stats;
}

void status_set_gps(bool connected, unsigned int baud)
{
    std::lock_guard<std::mutex> lock(g_status_mutex);
    g_gps_connected = connected;
    g_gps_baud = baud;
}

void status_publish()
{
    CanRxStats can;
    bool gps_connected;
    unsigned int gps_baud;

    {
        std::lock_guard<std::mutex> lock(g_status_mutex);
        can = g_can_rx;
        gps_connected = g_gps_connected;
        gps_baud = g_gps_baud;
    }

    ensure_log_dir();
    std::ofstream output(STATUS_PATH, std::ios::trunc);
    if (!output.is_open()) {
        return;
    }

    output << format_snapshot(can, gps_connected, gps_baud);
}

void status_print_cached()
{
    ensure_log_dir();

    std::ifstream input(STATUS_PATH);
    if (input.is_open()) {
        std::cout << input.rdbuf();
        return;
    }

    std::cout << "Telemetry is already running (status file not ready yet).\n";
}
