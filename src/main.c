#include "can.h"
#include "lora.h"
#include "log.h"
#include "watchdog.h"

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_FILE "/var/log/telemetry.log"

/**
 * TODO
 * Split program into multiple threads. There should be a thread for each peripheral.
 * There should also be a watchdog thread that monitors all of the other threads.
 * Collect the data into a struct and use another thread to send it as a FLATBUFFER every 1s via LoRa.
 */
int main(int argc, char **argv) {
    int nochdir = 0;    // Change to "/"
    int noclose = 1;    // don't redirect stdin, stdout and stderr to /dev/null

    // If opening the daemon failed, show error.
    if(daemon(nochdir, noclose)) {
	    perror("Starting the daemon failed");
        return EXIT_FAILURE;
    }

    // Redirect stdout and stderr to log file and stdin to /dev/null
    int logfd = open(LOG_FILE, O_WRONLY | O_CREAT, 0666);
    assert(logfd > 0);

    close(stdin);
    open("/dev/null", O_RDONLY);
    dup2(logfd, stdout);
    dup2(logfd, stderr);

    // Split program into multiple threads
    watchdog();

    return EXIT_SUCCESS;
}
