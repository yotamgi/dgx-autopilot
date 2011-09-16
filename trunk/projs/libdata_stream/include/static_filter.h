#ifndef STATIC_FILTER_H_
#define STATIC_FILTER_H_

#include "generators.h"

template <class T, size_t N>
class StaticFilter : public VecFilter<T,N> {
public:

	StaticFilter(VecGen<T,N>* gen):
		VecFilter<T,N>(gen),
		m_add((typename VecFilter<T,N>::vector_t){0., 0., 0.}),
		m_mul((typename VecFilter<T,N>::vector_t){1., 1., 1.})
	{}

	StaticFilter(
			const typename VecFilter<T,N>::vector_t& add,
			const typename VecFilter<T,N>::vector_t& mul):
		m_add(add),
		m_mul(mul)
	{}

	typename VecFilter<T,N>::vector_t get_data(){
		typename VecFilter<T,N>::vector_t data = VecFilter<T,N>::m_generator->get_data();

		for (size_t i = 0; i < N; ++i) {
			data[i] * m_mul[i];
		}

		return data+m_add;
	}

private:

	typename VecFilter<T,N>::vector_t m_add;
	typename VecFilter<T,N>::vector_t m_mul;
};

#endif /* STATIC_FILTER_H_ */
