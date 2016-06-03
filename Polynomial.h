#pragma once
#include <cstdint>

namespace ReedSolomon {

	class Polynomial {

	public:

		static Polynomial Create(uint16_t a0);
		static Polynomial Create(uint16_t a0, uint16_t a1);

		Polynomial(int order);

		Polynomial(const Polynomial& x);

		~Polynomial();

		Polynomial(uint16_t* coefficients, int order);

		void operator=(const Polynomial& x);

		inline uint16_t operator[](int i) const { return _coefficients[i]; }

		inline uint16_t& operator[](int i) { return _coefficients[i]; }

		inline int GetOrder() const { return _order; }

		Polynomial& operator *= (const Polynomial& x);

	private:

		// The index corresponds to the power of 'x'.  So, the polynomial '2x^2 + 3x + 4' would be stored as {4,3,2}
		uint16_t* _coefficients;
		int _order;
	};

	Polynomial operator*(const Polynomial& a, const Polynomial& b);
}
