#include <utils/data.hpp>

capnp::MallocMessageBuilder global_message;
std::mutex data_mutex;

void init_data(void) {
    std::lock_guard<std::mutex> lock(data_mutex);
    global_message.initRoot<TSData>();  // Must be called before read_data()
}

std::vector<uint8_t> read_data(void)
{
    std::lock_guard<std::mutex> lock(data_mutex);

    kj::Array<capnp::word> words = capnp::messageToFlatArray(global_message);
    
    // Get a byte pointer to the start and end of the word array
    const uint8_t* begin = reinterpret_cast<const uint8_t*>(words.begin());
    const uint8_t* end = begin + (words.size() * sizeof(capnp::word));

    // Copy into a vector using std::copy
    std::vector<uint8_t> buffer;
    buffer.reserve(end - begin);
    std::copy(begin, end, std::back_inserter(buffer));

    return buffer;
}
