#ifndef GANERATOR_H_
#define GANERATOR_H_

#include <boost/array.hpp>
#include <ostream>
#include <istream>
#include <iterator>

namespace stream {

/**
 * Represents a any type of data generator. It may be a sensor, or a combination
 * of some.
 */
template <typename data_t>
class DataGenerator {
public:

	typedef data_t type;

	virtual ~DataGenerator() {}

	/**
	 * Suppose to return the current value/sample of the data.
	 */
	virtual data_t get_data() = 0;

};

/**
 * Specific Vector type Data Generator.
 * It defines the typedef vector_t which is the type of the vector it generates.
 */
template <typename T, size_t N>
class VecGenerator : public DataGenerator< boost::array<T,N> >
{
public:
	typedef boost::array<T,N> vector_t;
};

} // namespace stream

/**
 * Global vector operators
 */
template <typename T, size_t N>
std::ostream& operator << (std::ostream& os, const boost::array<T,N> &v) {
	os << '[';
	std::copy(v.begin(), v.end()-1, std::ostream_iterator<T>(os, ", "));
	os << v[N-1] << "]";
	return os;
}

template <typename T, size_t N>
std::istream& operator >> (std::istream &is, boost::array<T,N> &v) {

    char ch;
    if (is >> ch && ch != '[') {
        is.putback (ch);
        is.setstate (std::ios_base::failbit);
    };
    for (size_t i=0; i<N-1; i++) {
    	is >> v[i] >> ch;
    	if (ch != ',') {
    		is.putback (ch);
    		is.setstate (std::ios_base::failbit);
    		break;
    	}
    }
	is >> v[N-1] >> ch;
	if (ch != ']') {
		is.putback (ch);
		is.setstate (std::ios_base::failbit);
	}

    return is;
}



#endif /* GANERATOR_H_ */
