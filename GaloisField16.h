#pragma once
#include <cstdint>
#include <tmmintrin.h>

namespace ReedSolomon
{
	class TTables;

	class GaloisField16 {

	public:

		static const int ELEMENT_COUNT = 0x10000;
		static const uint16_t MAX_VALUE = 0xFFFF;

		static uint16_t Multiply(uint16_t x, uint16_t y);

		static uint16_t Inverse(uint16_t x);

		static uint16_t Power(uint16_t x, int a);

		inline static uint16_t Add(uint16_t x, uint16_t y) { return x ^ y; }

		inline static uint16_t Exp(int x) { return _instance.expTable[x]; }

		inline static int Log(uint16_t x) { return _instance.logTable[x]; }

		static void Add(uint16_t* a, uint16_t* b, uint16_t* result, int count);
		static void Multiply(uint16_t y, uint16_t* a, uint16_t* result, int count);

		static void SSEAdd(uint16_t* a, uint16_t* b, uint16_t* result, int count);
		static void SSEMultiplyAlternate(uint16_t y, uint16_t* a, uint16_t* result, int count);
		static void SSEMultiplyStandard(uint16_t y, uint16_t* a, uint16_t* result, int count);

	private:

		GaloisField16();
		~GaloisField16();

		static const uint16_t PRIMITIVE_POLYNOMIAL = 0x100B;

		// The maximum integer value for this Galois field.
		// Used to mask integers to bring them within range after arithmetic operations.
		static const uint32_t MASK = 0xFFFF;

		// The highest bit for this Galois field.  
		// Used to determine when we will exceed the maximum integer size with the next shift left.
		static const uint32_t HIGH_BIT = 0x8000;
		static const int32_t BITS = 16;

		class LookupTable {

		public:

			// FOR LITTLE ENDIAN
			LookupTable(uint16_t y);
			~LookupTable();

			__m128i* T0High;
			__m128i* T1High;
			__m128i* T2High;
			__m128i* T3High;

			__m128i* T0Low;
			__m128i* T1Low;
			__m128i* T2Low;
			__m128i* T3Low;

		private:

			uint8_t* T;
		};

		static GaloisField16 _instance;

		int logTable[ELEMENT_COUNT];
		uint16_t expTable[ELEMENT_COUNT];
		LookupTable* _lookupTables[ELEMENT_COUNT];
	};
}

