#pragma once

#include <cstdint>
#include <string>

struct CanRxStats {
    uint32_t total = 0;
    uint32_t unknown = 0;
    uint32_t mcu_state = 0;
    uint32_t mcu_faults = 0;
    uint32_t mcu_inputs = 0;
    uint32_t mcu_pedals = 0;
    uint32_t mcu_fuel_cell = 0;
    uint32_t mcu_powertrain = 0;
    uint32_t mcu_accessory = 0;
    uint32_t mcu_drive = 0;
    uint32_t mcu_unassigned = 0;
    uint32_t fccu = 0;
    uint32_t swu_lights = 0;
    uint32_t swu_time = 0;
    uint32_t swu_state = 0;
    uint32_t other = 0;

    void record(uint32_t can_id);
    void reset();
};

bool status_try_acquire_primary_lock();
void status_release_primary_lock();

void status_set_can_rx(const CanRxStats& stats);
void status_set_gps(bool connected, unsigned int baud);

void status_publish();
void status_print_cached();
