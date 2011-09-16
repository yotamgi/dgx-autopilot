#ifndef STATIC_FILTER_H_
#define STATIC_FILTER_H_

#include "generators.h"

template <class T, size_t N>
class StaticFilter : public VecFilter<T,N> {
public:

	StaticFilter(VecGenerator<T,N>* gen):
		VecFilter<T,N>(gen)
	{
		m_add[0] = 0.; m_add[1] = 0.; m_add[2]=0.;
		m_mul[0] = 0.; m_mul[1] = 0.; m_mul[2]=0.;
	}

	StaticFilter(VecGenerator<T,N>* gen,
			const typename VecFilter<T,N>::vector_t& add,
			const typename VecFilter<T,N>::vector_t& mul):
		VecFilter<T,N>(gen),
		m_add(add),
		m_mul(mul)
	{}

	typename VecFilter<T,N>::vector_t get_data(){
		typename VecFilter<T,N>::vector_t data = VecFilter<T,N>::m_generator->get_data();

		for (size_t i = 0; i < N; ++i) {
			data[i] *= m_mul[i] + m_add[0];
		}

		return data;
	}

private:

	typename VecFilter<T,N>::vector_t m_add;
	typename VecFilter<T,N>::vector_t m_mul;
};

#endif /* STATIC_FILTER_H_ */
