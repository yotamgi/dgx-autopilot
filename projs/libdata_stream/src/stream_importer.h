/*
 * stream_importer.h
 *
 *  Created on: Sep 19, 2011
 *      Author: yotam
 */

#ifndef STREAM_IMPORTER_H_
#define STREAM_IMPORTER_H_

#include <boost/shared_ptr.hpp>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <boost/numeric/ublas/io.hpp>
#include "generators.h"
#include "protocol.h"

namespace stream {

class StreamImporter {
public:
	StreamImporter(std::string address);
	~StreamImporter();

	template <typename T>
	boost::shared_ptr<DataGenerator<T> > import_stream(std::string name) {
		return boost::shared_ptr< DataGenerator<T> >(new StreamProxy<T>(this, name));
	}

	std::vector<std::string> list_avail();

private:

	template <typename T>
	class StreamProxy : public DataGenerator<T> {
	public:

		T get_data() {

			// ask for the data
			m_father->write_sock(std::string(&protocol::GET_COMMAND, 1) + m_name);

			// get the serialized data
			std::string serialized_data = m_father->read_sock();
			if (serialized_data == protocol::NOT_EXIST_COMMAND) {
				throw std::runtime_error("The stream asked is not exported");
			}

			// deserialize
			std::stringstream ss(serialized_data);
			T data;
			ss >> data;
			if (ss.fail()) {
				throw std::runtime_error("The exported stream does not match");
			}
			return data;
		}

		bool is_data_losed() { return false; }

	private:
		StreamProxy(StreamImporter* importer, std::string name):
			m_father(importer), m_name(name) {}

		StreamImporter* m_father;
		std::string m_name;

		friend class StreamImporter;
	};

protected:

	/**
	 * Connect func.
	 * Connects m_sock to m_address.
	 * It is blocking until it succeeds
	 */
	void connect_to_host();

	void write_sock(std::string);
	std::string read_sock();

private:

	std::string m_host_address;
	int m_sock;
};

} // namespace stream

#endif /* STREAM_IMPORTER_H_ */
