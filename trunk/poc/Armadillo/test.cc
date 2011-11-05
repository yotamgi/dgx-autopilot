#include <iostream>
#include <armadillo>
#include <sstream>

template <size_t N>
std::ostream&
operator<< (std::ostream& o, const typename arma::Col<float>::template fixed<N>& X)
{
	o << X[0] << ", " << X[1] << ", " << X[2];
	return o;
}


void simple() {
	arma::fvec::fixed<3> a;
	a << 1. << 2. << 3.;

	std::cout << a << std::endl;
	arma::fvec::fixed<3> b = a*3.;

	std::cout << b << std::endl;
}

template <typename mat>
mat func(const mat& m) {
	return m/2.;
}

void ident() {
	arma::fmat::fixed<3,3> a;
	a.eye(3, 3);
	std::cout << a << std::endl;
	a.col(0) = func(a.col(0));
	std::cout << a << std::endl;
}


void matrix() {
	arma::fmat::fixed<3,3> m;
	arma::fvec::fixed<3> c;
	c << 1. << 2. << 3.;
	m.col(0) = c;
	m.col(1) = c;
	m.col(2) = c;
	c = m*c;
	std::cout << c << std::endl;
	std::cout << m << std::endl;
}
int main() {
	ident();
}
