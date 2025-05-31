#include <utils/data.hpp>

capnp::MallocMessageBuilder global_message;
std::mutex data_mutex;

uint8_t buffer[TSDATA_BUFFER_SIZE];

void init_data(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.initRoot<TSData>();  // Must be called before read_data()
}

const uint8_t* read_data(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    
    kj::Array<capnp::word> dataArr = capnp::messageToFlatArray(global_message);
    kj::ArrayPtr<kj::byte> bytes = dataArr.asBytes();

    std::memcpy(buffer, bytes.begin(), bytes.size());

    return buffer;
}
