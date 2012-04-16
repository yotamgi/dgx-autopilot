#include "async_stream_connection.h"

namespace stream {

//
// The specific stream methods
//

static bool istream_fail(const std::istream& is) {
	return !is.good();
}

const char SEPERATOR = '%';

template <typename T>
void AsyncStreamConnection::SendPopStream<T>::serialize(std::ostream& os) {
	os << m_s->get_data() << SEPERATOR;
}

template <typename T>
void AsyncStreamConnection::SendPushStream<T>::set_data(const T& data) {
	m_curr_data = data;
	m_counter++;
}

template <typename T>
void AsyncStreamConnection::SendPushStream<T>::serialize(std::ostream& os) {
	os << m_counter << "," << m_curr_data << SEPERATOR;
}

template <typename T>
void AsyncStreamConnection::RecvPopStream<T>::deserialize(std::istream& is) {
	char sep;
	is >> m_curr_data >> sep;
	if ((istream_fail(is)) || (sep != SEPERATOR)) {
		throw ConnectionExceptioin("AsyncStreamConnection could not parse the stream as pop stream of the correct type");
	}
}

template <typename T>
T AsyncStreamConnection::RecvPopStream<T>::get_data() {
	return m_curr_data;
}

template <typename T>
void AsyncStreamConnection::RecvPushStream<T>::deserialize(std::istream& is) {

	// parse the message;
	size_t curr_counter;
	char comma, sep;
	T data;
	is >> curr_counter >> comma >> data >> sep;
	if (istream_fail(is) || (sep != SEPERATOR) || (comma != ',')) {
		throw ConnectionExceptioin("AsyncStreamConnection could not parse the strean as a push stream of the correct type");
	}

	// push the stream if new data arrived
	if (curr_counter != m_counter) {
		m_s->set_data(data);
		m_counter = curr_counter;
	}
}

} // namespace stream
