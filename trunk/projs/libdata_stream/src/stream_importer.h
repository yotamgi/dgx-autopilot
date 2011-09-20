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
#include <boost/numeric/ublas/io.hpp>
#include "generators.h"
#include "protocol.h"

class StreamImporter {
public:
	StreamImporter(std::string address);

	template <typename T>
	boost::shared_ptr<DataGenerator<T> > import_stream(std::string name) {
		return boost::shared_ptr< DataGenerator<T> >(new StreamProxy<T>(this, name));
	}

private:

	template <typename T>
	class StreamProxy : public DataGenerator<T> {
	public:

		T get_data() {
			int sock = m_father->get_socket();

			// ask for the data
			std::string ask;
			ask = protocol::GET_COMMAND + m_name;
			while (write(sock, ask.c_str(), ask.size()) != (int)ask.size()) {
				m_father->connect_to_host();
			}

			// get the serialized data
			const size_t BLOCK_SIZE = 100;
			char buff[BLOCK_SIZE];
			std::string serialized_data;
			while (read(sock, buff, BLOCK_SIZE) == (int)BLOCK_SIZE) {
				serialized_data += std::string(buff);
			}
			serialized_data += std::string(buff);

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

private:

	/**
	 * Connect func.
	 * Connects m_sock to m_address.
	 * It is blocking until it succeeds
	 */
	void connect_to_host();

	int get_socket() { return m_sock; }

	std::string m_host_address;
	int m_sock;
};

#endif /* STREAM_IMPORTER_H_ */
