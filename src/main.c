#include <threads/peripherals/can.h>
#include <threads/peripherals/gps.h>
#include <threads/peripherals/lora.h>
#include <utils/data.h>

#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOG_FILE "/var/log/telemetry.log"

static bool running = true;

static pthread_t can_thread;
static pthread_t gps_thread;
static pthread_t lora_thread;

void sigaction_handler(int signum)
{
    fprintf(stdout, "MAIN: Sigaction received - Closing program...\n");
    running = false; // TODO do we need a more advanced handler?
}

int main(int argc, char** argv)
{
    // Redirect stdout and stderr to log file and stdin to /dev/null
    int logfd = open(LOG_FILE, O_APPEND | O_WRONLY | O_CREAT, 0666);
    assert(logfd > 0);

    dup2(logfd, STDOUT_FILENO);
    dup2(logfd, STDERR_FILENO);

    // Switch to line buffering
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    // Split program into multiple threads
    pthread_create(&can_thread, NULL, can, NULL);
    pthread_create(&gps_thread, NULL, gps, NULL);
    pthread_create(&lora_thread, NULL, lora, NULL);

    struct sigaction sig = { 0 };
    sig.sa_handler = sigaction_handler;
    sigaction(SIGINT, &sig, NULL);

    while (running) {
        sleep(1);
    }

    pthread_join(can_thread, NULL);
    pthread_join(gps_thread, NULL);
    pthread_join(lora_thread, NULL);

    return EXIT_SUCCESS;
}
