#pragma once
#include "RS16Syndrome.h"
#include "SquareMatrix.h"

namespace ReedSolomon {

	class RS16Repair {

	public:

		RS16Repair(const RS16Syndrome& rss, int nCodeWords, int* errorLocations, int errorCount);
		~RS16Repair();

		inline int GetNCodeWords() const { return _nCodeWords; }

		void Correction(int errorLocationOffset, uint16_t* data) const;

	private:

		const RS16Syndrome& _rss;
		int _nCodeWords;
		SquareMatrix _correctionMatrix;
		int* errorOrders;
		int errorCount;
	};

	extern "C" {
		__declspec(dllexport) RS16Repair* RS16Repair_Construct(const RS16Repair* rss, int nCodeWords, int* errorLocations, int errorCount);
		__declspec(dllexport) void RS16Repair_Destruct(RS16Repair* rsr);
		__declspec(dllexport) void RS16Repair_Correction(RS16Repair* rsr, int errorLocationOffset, uint16_t* data);
		__declspec(dllexport) int RS16Repair_GetNCodeWords(RS16Repair* rsr);
	}
}