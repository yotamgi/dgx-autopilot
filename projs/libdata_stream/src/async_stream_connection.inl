#include "async_stream_connection.h"

namespace stream {

template <typename T>
void AsyncStreamConnection::AnyPopStream::serialize(std::ostream& os) {
	os << m_s->get_data();
}

template <typename T>
void AsyncStreamConnection::AnyPopStream::deserialize(std::istream& is) {
	throw ConnectionExceptioin("The pop can't be deserialized");
}

template <typename T>
void AsyncStreamConnection::AnyPushStream::serialize(std::ostream& os) {
	throw ConnectionExceptioin("The pop can't be deserialized");
}

template <typename T>
void AsyncStreamConnection::AnyPushStream::deserialize(std::istream& is) {
	os << m_s->get_data();
}


} // namespace stream
