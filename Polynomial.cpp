#include "Polynomial.h"
#include <iostream>
#include "GaloisField16.h"
#include <algorithm>

namespace ReedSolomon {

	Polynomial::Polynomial(int order) {
		_order = order;
		_coefficients = new uint16_t[order + 1];
		memset(_coefficients, 0, (order + 1) * sizeof(uint16_t));
	}

	Polynomial::Polynomial(const Polynomial& x) {
		_order = x._order;
		_coefficients = new uint16_t[_order + 1];
		memcpy(_coefficients, x._coefficients, (_order + 1)*sizeof(uint16_t));
	}

	Polynomial::~Polynomial() {
		delete[] _coefficients;
	}

	Polynomial::Polynomial(uint16_t* coefficients, int order) {
		_order = order;
		_coefficients = new uint16_t[order + 1];
		memcpy(_coefficients, coefficients, (order + 1)*sizeof(uint16_t));
	}

	void Polynomial::operator=(const Polynomial& x) {
		if (_order != x._order) {
			delete[] _coefficients;
			_order = x._order;
			_coefficients = new uint16_t[_order + 1];
		}
		memcpy(_coefficients, x._coefficients, (_order + 1)*sizeof(uint16_t));
	}

	Polynomial Polynomial::Create(uint16_t a0) {
		Polynomial p(0);
		p[0] = a0;
		return p;
	}

	Polynomial Polynomial::Create(uint16_t a0, uint16_t a1) {
		Polynomial p(1);
		p[0] = a0;
		p[1] = a1;
		return p;
	}

	Polynomial& Polynomial::operator *= (const Polynomial& x) {

		if (x.GetOrder() == 0) {
			for (int i = 0; i < _order; ++i) _coefficients[i] = GaloisField16::Multiply(_coefficients[i], x[0]);
		}
		else {
			int newOrder = _order + x.GetOrder();
			uint16_t* resultCoefficients = new uint16_t[newOrder + 1];
			for (int i = newOrder; i >= 0; --i) {
				resultCoefficients[i] = 0;

				int low = std::max<int>(0, i - x.GetOrder());
				int high = std::min<int>(i, _order);

				for (int ai = low; ai <= high; ai++) {
					resultCoefficients[i] = GaloisField16::Add(resultCoefficients[i], GaloisField16::Multiply(_coefficients[ai], x[i - ai]));
				}
			}
			delete[] _coefficients;
			_coefficients = resultCoefficients;
			_order = newOrder;
		}

		return *this;
	}

	static Polynomial operator*(const Polynomial& a, const Polynomial& b) {

		Polynomial result(a.GetOrder() + b.GetOrder());
		for (int i = result.GetOrder(); i >= 0; --i) {

			int low = std::max<int>(0, i - b.GetOrder());
			int high = std::min<int>(i, a.GetOrder());

			for (int ai = low; ai <= high; ai++) result[i] = GaloisField16::Add(result[i], GaloisField16::Multiply(a[ai], b[i - ai]));
		}

		return result;
	}
}