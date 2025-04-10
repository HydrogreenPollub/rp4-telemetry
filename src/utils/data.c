#include <utils/data.h>

struct TSData telemetry_data = { 0 };
static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

ssize_t buffer_len = 0;
uint8_t buf[TSDATA_BUFFER_SIZE] = { 0 };
static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

// Define setters for struct using XMacro
#define XSET_TSDATA(FIELD, TYPE) \
void set_##FIELD(TYPE value) { \
    pthread_mutex_lock(&data_mutex); \
    telemetry_data.FIELD = value; \
    pthread_mutex_unlock(&data_mutex); \
}
#include <utils/ts_data.def>
#undef XSET_TSDATA

// Return read only pointer
const uint8_t *read_data(void) {
    pthread_mutex_lock(&buffer_mutex);

    // Create capnproto object
    struct capn c;
    capn_init_malloc(&c);

    // Create pointer to root structure
    capn_ptr cr = capn_root(&c);
    struct capn_segment *cs = cr.seg;

    // Create pointer to data
    TSData_ptr ptr = new_TSData(cs);

    pthread_mutex_lock(&data_mutex);
    write_TSData(&telemetry_data, ptr);
    capn_setp(capn_root(&c), 0, ptr.p);
    buffer_len = capn_write_mem(&c, buf, TSDATA_BUFFER_SIZE, 0);
    pthread_mutex_unlock(&data_mutex);

    capn_free(&c);

    pthread_mutex_unlock(&buffer_mutex);
    return buf;
}