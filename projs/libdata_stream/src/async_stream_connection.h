#ifndef ASYNC_STREAM_CONNECTION_H_
#define ASYNC_STREAM_CONNECTION_H_

#include <stream/data_pop_stream.h>
#include <stream/data_push_stream.h>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace stream {

class AsyncStreamConnection {
public:

	/**
	 * Classes for getting reed of the bloody template arguments in the streams
	 */
	class AnyStream {
	public:
		virtual void serialize(std::ostream&) = 0;
		virtual void deserialize(std::istream&) = 0;
	};

	template <typename T>
	class AnyPopStream : public AnyStream {
	public:
		AnyPopStream(boost::shared_ptr<stream::DataPopStream<T> > s): m_s(s) {}

		virtual void serialize(std::ostream& os);
		virtual void deserialize(std::istream& is);
	private:
		boost::shared_ptr<stream::DataPopStream<T> > m_s;
	};

	template <typename T>
	class AnyPushStream : public AnyStream {
	public:
		AnyPushStream(boost::shared_ptr<stream::DataPushStream<T> > s): m_s(s) {}

		virtual void serialize(std::ostream& os);
		virtual void deserialize(std::istream& is);
	private:
		boost::shared_ptr<stream::DataPopStream<T> > m_s;
	};
private:
};

} // namespace stream

#include "async_stream_connection.inl"


#endif /* ASYNC_STREAM_CONNECTION_H_ */
