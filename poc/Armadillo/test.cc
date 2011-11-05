#include <iostream>
#include <armadillo>
#include <sstream>

std::ostream&
operator<< (std::ostream& o, const arma::fvec::fixed<3>& X)
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


void matrix() {
	arma::fmat::fixed<3,3> m;
	arma::fvec::fixed<3> c;
	c << 1. << 2. << 3.;
	m.col(0) = c;
	m.col(1) = c;
	m.col(2) = c;
	c = m*c;
	std::cout << c << std::endl;
}
int main() {
	matrix();
}
