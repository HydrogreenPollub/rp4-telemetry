#include <threads/watchdog.h>
#include <threads/peripherals/can.h>
#include <threads/peripherals/lora.h>

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_FILE "/var/log/telemetry.log"

static pthread_t can_thread;
static pthread_t lora_thread;

/**
 * TODO
 * Collect the data into a struct and use another thread to send it as a FLATBUFFER every 1s via LoRa.
 */
int main(int argc, char **argv) {
    // Redirect stdout and stderr to log file and stdin to /dev/null
    int logfd = open(LOG_FILE, O_APPEND | O_WRONLY | O_CREAT, 0666);
    assert(logfd > 0);

    close(STDIN_FILENO);
    open("/dev/null", O_RDONLY);
    dup2(logfd, STDOUT_FILENO);
    dup2(logfd, STDERR_FILENO);
    
    // Disable file buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    // TODO handle sigactions by closing all opened peripherals

    // Split program into multiple threads - we pass in IDs growing from 0 in order
    pthread_create(&can_thread, NULL, can, (void*)0);
    pthread_create(&lora_thread, NULL, lora, (void*)1);

    watchdog();

    pthread_join(can_thread, NULL);
    pthread_join(lora_thread, NULL);

    return EXIT_SUCCESS;
}
