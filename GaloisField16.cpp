#include "GaloisField16.h"
#include <stdexcept>

namespace SRFS {

	GaloisField16 GaloisField16::_instance = GaloisField16();

	GaloisField16::GaloisField16() {

		// The starting value for log table construnction.  We shift this left with each subsequent entry, using the primitive polynomial
		// to handle an overflow.
		uint16_t value = 0x01u;
		for (int exponent = 0; exponent < MASK; exponent++) {
			expTable[exponent] = value;
			logTable[value] = exponent;
			// Check for overflow on the next shift left
			if ((value & HIGH_BIT) != 0) value = ((value << 1) & MASK) ^ PRIMITIVE_POLYNOMIAL;
			else value = value << 1;
		}
		// Add in the last value.
		expTable[MASK] = expTable[0];
		// The log of zero is undefined, but I do not want unitialized data.
		logTable[0] = 0;

		// The T-Tables will be filled out when they're first called.
		for (int i = 0; i <= MAX_VALUE; i++) _lookupTables[i] = nullptr;
	}

	GaloisField16::~GaloisField16() {
		for (int i = 0; i <= MAX_VALUE; i++) {
			if (_lookupTables[i] != nullptr) delete _lookupTables[i];
		}
	}

	void GaloisField16::Add(uint16_t* a, uint16_t* b, uint16_t* result, int count) {
		for (int i = 0; i < count; i++) {
			*result = Add(*a, *b);
			a++;
			b++;
			result++;
		}
	}

	uint16_t GaloisField16::Multiply(uint16_t x, uint16_t y) {
		if ((x == 0) || (y == 0)) return 0;

		int sum = _instance.logTable[x] + _instance.logTable[y];
		sum = (sum >> BITS) + (sum & MASK);
		return _instance.expTable[sum];
	}

	void GaloisField16::Multiply(uint16_t x, uint16_t* y, uint16_t* result, int count) {
		if (x == 0) {
			// Set everything to zero
			while (count-- > 0) *(result++) = 0;
		}
		else {
			uint32_t logx = _instance.logTable[x];
			uint32_t sum = 0;
			while (count-- > 0) {
				if (*y == 0) {
					*result = 0;
				}
				else {
					sum = logx + _instance.logTable[*y];
					*result = _instance.expTable[((sum >> BITS) + (sum & MASK))];
				}
				result++;
				y++;
			}
		}
	}

	void GaloisField16::SSEAdd(uint16_t* a, uint16_t* b, uint16_t* result, int count) {
		if (count % 8 != 0) throw std::invalid_argument("count must be a multiple of 8");
		for (int i = 0; i < count; i += 8) {
			__m128i xmm1 = _mm_loadu_si128((__m128i*)a);
			__m128i xmm2 = _mm_loadu_si128((__m128i*)b);
			__m128i r = _mm_xor_si128(xmm1, xmm2);
			_mm_storeu_si128((__m128i*)result, r);

			a += 8;
			b += 8;
			result += 8;
		}
	}

	void GaloisField16::SSEMultiplyStandard(uint16_t y, uint16_t* a, uint16_t* result, int count) {
		if (count % 16 != 0) throw std::invalid_argument("count must be a multiple of 16");

		LookupTable* table = _instance._lookupTables[y];
		if (table == nullptr) {
			table = new LookupTable(y);
			_instance._lookupTables[y] = table;
		}

		__m128i mask = _mm_set1_epi16(0xff);
		__m128i mask1 = _mm_set1_epi8(0x0f);
		__m128i mask2 = _mm_set1_epi8((char)0xf0);

		for (int i = 0; i < count; i += 16) {
			__m128i ab = _mm_loadu_si128((__m128i*)a);
			a += 8;
			__m128i cd = _mm_loadu_si128((__m128i*)a);
			a += 8;
			__m128i ab_h = _mm_srli_epi16(ab, 8);
			__m128i ab_l = _mm_and_si128(ab, mask);
			__m128i cd_h = _mm_srli_epi16(cd, 8);
			__m128i cd_l = _mm_and_si128(cd, mask);

			__m128i low = _mm_packus_epi16(ab_l, cd_l);
			__m128i high = _mm_packus_epi16(ab_h, cd_h);

			// Low order 
			// 0 b0 0 c0 0 d0 0 e0
			__m128i nybble = _mm_and_si128(low, mask1);
			__m128i resultHigh = _mm_shuffle_epi8(*table->T0High, nybble);
			__m128i resultLow = _mm_shuffle_epi8(*table->T0Low, nybble);

			// 0 b1 0 c1 0 d1 0 e1
			__m128i high_nybble = _mm_and_si128(low, mask2);
			nybble = _mm_srli_epi64(high_nybble, 4);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T1High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T1Low, nybble));

			// High order
			// 0 b2 0 c2 0 d2 0 e2
			nybble = _mm_and_si128(high, mask1);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T2High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T2Low, nybble));

			// 0 b3 0 c3 0 d3 0 e3
			high_nybble = _mm_and_si128(high, mask2);
			nybble = _mm_srli_epi64(high_nybble, 4);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T3High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T3Low, nybble));

			ab = _mm_unpacklo_epi8(resultLow, resultHigh);
			cd = _mm_unpackhi_epi8(resultLow, resultHigh);

			_mm_storeu_si128((__m128i*)result, ab);
			result += 8;
			_mm_storeu_si128((__m128i*)result, cd);
			result += 8;
		}
	}

	void GaloisField16::SSEMultiplyAlternate(uint16_t y, uint16_t* a, uint16_t* result, int count) {
		if (count % 16 != 0) throw std::invalid_argument("count must be a multiple of 16");

		LookupTable* table = _instance._lookupTables[y];
		if (table == nullptr) {
			table = new LookupTable(y);
			_instance._lookupTables[y] = table;
		}

		__m128i mask = _mm_set1_epi16(0xff);
		__m128i mask1 = _mm_set1_epi8(0x0f);
		__m128i mask2 = _mm_set1_epi8((char)0xf0);

		for (int i = 0; i < count; i += 16) {
			__m128i low = _mm_loadu_si128((__m128i*)a);
			a += 8;
			__m128i high = _mm_loadu_si128((__m128i*)a);
			a += 8;

			// Low order 
			// 0 b0 0 c0 0 d0 0 e0
			__m128i nybble = _mm_and_si128(low, mask1);
			__m128i resultHigh = _mm_shuffle_epi8(*table->T0High, nybble);
			__m128i resultLow = _mm_shuffle_epi8(*table->T0Low, nybble);

			// 0 b1 0 c1 0 d1 0 e1
			__m128i high_nybble = _mm_and_si128(low, mask2);
			nybble = _mm_srli_epi64(high_nybble, 4);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T1High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T1Low, nybble));

			// High order
			// 0 b2 0 c2 0 d2 0 e2
			nybble = _mm_and_si128(high, mask1);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T2High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T2Low, nybble));

			// 0 b3 0 c3 0 d3 0 e3
			high_nybble = _mm_and_si128(high, mask2);
			nybble = _mm_srli_epi64(high_nybble, 4);
			resultHigh = _mm_xor_si128(resultHigh, _mm_shuffle_epi8(*table->T3High, nybble));
			resultLow = _mm_xor_si128(resultLow, _mm_shuffle_epi8(*table->T3Low, nybble));

			_mm_storeu_si128((__m128i*)result, resultLow);
			result += 8;
			_mm_storeu_si128((__m128i*)result, resultHigh);
			result += 8;
		}
	}

	uint16_t GaloisField16::Power(uint16_t x, int a) {
		if (x == 0) {
			if (a == 0) throw std::invalid_argument("0 raised to the 0 power is undefined.");
			return 0;
		}
		if (a == 0) return 1;
		return _instance.expTable[(_instance.logTable[x] * a) % MAX_VALUE];
	}

	uint16_t GaloisField16::Inverse(uint16_t x) {
		if (x == 0) throw std::invalid_argument("Cannot take the inverse of zero");
		return _instance.expTable[MAX_VALUE - _instance.logTable[x]];
	}

	GaloisField16::LookupTable::LookupTable(uint16_t y) {

		T = (uint8_t*)_aligned_malloc(128, 16);

		uint16_t product;
		uint8_t* pLow = (uint8_t*)(&product);
		uint8_t* pHigh = pLow + 1;

		uint8_t* pT0l = T;
		uint8_t* pT0h = T + 16;
		uint8_t* pT1l = T + 32;
		uint8_t* pT1h = T + 48;
		uint8_t* pT2l = T + 64;
		uint8_t* pT2h = T + 80;
		uint8_t* pT3l = T + 96;
		uint8_t* pT3h = T + 112;

		T0Low = (__m128i*)pT0l;
		T0High = (__m128i*)pT0h;
		T1Low = (__m128i*)pT1l;
		T1High = (__m128i*)pT1h;
		T2Low = (__m128i*)pT2l;
		T2High = (__m128i*)pT2h;
		T3Low = (__m128i*)pT3l;
		T3High = (__m128i*)pT3h;

		for (uint16_t i = 0; i < 16; i++) {
			uint16_t v = i;
			product = GaloisField16::Multiply(y, v);
			*pT0l = *pLow;
			*pT0h = *pHigh;

			v <<= 4;
			product = GaloisField16::Multiply(y, v);
			*pT1l = *pLow;
			*pT1h = *pHigh;

			v <<= 4;
			product = GaloisField16::Multiply(y, v);
			*pT2l = *pLow;
			*pT2h = *pHigh;

			v <<= 4;
			product = GaloisField16::Multiply(y, v);
			*pT3l = *pLow;
			*pT3h = *pHigh;

			pT0l++;
			pT0h++;
			pT1l++;
			pT1h++;
			pT2l++;
			pT2h++;
			pT3l++;
			pT3h++;
		}
	}

	GaloisField16::LookupTable::~LookupTable() {
		_aligned_free(T);
	}
}