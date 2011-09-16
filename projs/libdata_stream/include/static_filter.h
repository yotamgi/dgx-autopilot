#ifndef STATIC_FILTER_H_
#define STATIC_FILTER_H_

template <class T, N>
class StaticFilter : public VecFilter<T,N> {
public:

	StaticFilter::StaticFilter():
		m_add((typename VecFilter<T,N>::vector_t){0., 0., 0.}),
		m_mul((typename VecFilter<T,N>::vector_t){1., 1., 1.})
	{}

	StaticFilter::StaticFilter(
			const typename VecFilter<T,N>::vector_t& add,
			const typename VecFilter<T,N>::vector_t& mul):
		m_add(add),
		m_mul(mul)
	{}

	typename VecFilter<T,N>::vector_t get_data(){
		typename VecFilter<T,N>::vector_t data = m_gen->get_data();

		for (int i = 0; i < N; ++i) {
			data[i] * m_mul[i];
		}

		return data+m_add;
	}

private:

	VecFilter<T,N>::vector_t m_add;
	VecFilter<T,N>::vector_t m_mul;
};

#endif /* STATIC_FILTER_H_ */
