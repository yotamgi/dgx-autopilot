#ifndef DATA_COLLECTOR_H_
#define DATA_COLLECTOR_H_

namespace stream {

/**
 * A class for push kind streams
 */
template <typename T>
class DataPushStream {
public:
	virtual ~DataPushStream() {}

	typedef T data_t;

	/**
	 * The method for pushing the data to the stream
	 */
	virtual void set_data(T data) = 0;
};


}  // namespace stream

#endif /* DATA_COLLECTOR_H_ */
