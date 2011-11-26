#ifndef DATA_COLLECTOR_H_
#define DATA_COLLECTOR_H_

/**
 * A class for push kind streams
 */
template <typename T>
class DataCollector {
public:
	virtual ~DataCollector() {}

	typedef T data_t;

	/**
	 * The method for pushing the data to the stream
	 */
	virtual void set_data(T data) = 0;
};


#endif /* DATA_COLLECTOR_H_ */
