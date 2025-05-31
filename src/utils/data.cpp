#include <utils/data.hpp>

capnp::MallocMessageBuilder global_message;
std::mutex data_mutex;

auto telemetry_data = global_message.initRoot<TSData>();

std::vector<uint8_t> read_data(void)
{
    kj::Array<capnp::word> words = capnp::messageToFlatArray(global_message);
    kj::ArrayPtr<capnp::word> wordPtr = words.asPtr();

    auto bytes = kj::arrayPtr(reinterpret_cast<const uint8_t*>(wordPtr.begin()), wordPtr.size() * sizeof(capnp::word));
    return std::vector<uint8_t>(bytes.begin(), bytes.end());
}
