#include <iostream>
#include <armadillo>
#include <sstream>

std::ostream&
operator<< (std::ostream& o, const typename arma::Col<float>::fixed<3>& X)
{
	X.save(o, arma::arma_ascii);
	return o;
}

std::istream&
operator>> (std::istream& o, typename arma::Col<float>::fixed<3>& X)
{
	X.load(o, arma::arma_ascii);
	return o;
}


void simple() {
	arma::fvec::fixed<3> a;
	a << 1. << 2. << 3.;

	std::cout << a << std::endl;
	arma::fvec::fixed<3> b = a*3.;

	std::cout << b << std::endl;
}

void ident() {
	arma::fmat::fixed<3,3> a;
	a.eye(3, 3);
	std::cout << a << std::endl;
}

void serialie() {
	std::stringstream ss;
	arma::fvec::fixed<3> a, b;
	a << 1. << 2. << 3.;
	ss<<a;
	ss>>b;
	std::cout << b;
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
	serialie();
}
