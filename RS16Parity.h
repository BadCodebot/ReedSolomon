#pragma once
#include <cstdint>

namespace ReedSolomon {

	class RS16ParityCalculator {

	public:

		RS16ParityCalculator(int nParityCodeWords, int nMessages);
		~RS16ParityCalculator();

		void Encode(uint16_t* data) const;
		void SSEEncodeStandard(uint16_t* data) const;
		void SSEEncodeAlternate(uint16_t* data) const;

		uint16_t* GetParity() const { return parity; }

		int GetNParityCodeWords() const { return _nParityCodeWords; }
		
		int GetNMessages() const { return _nMessages; }

	private:

		int _nParityCodeWords;
		int _nMessages;

		uint16_t* generator;
		uint16_t* parity;
		uint16_t* temp1;
		uint16_t* temp2;
	};

	extern "C" {
		__declspec(dllexport) RS16ParityCalculator* ReedSolomon16Calculate_Construct(int nParityCodeWords, int nMessages);
		__declspec(dllexport) void RS16ParityCalculator_Destruct(RS16ParityCalculator* rsc);
		__declspec(dllexport) void RS16ParityCalculator_Encode(RS16ParityCalculator* rsc, uint16_t* data);
		__declspec(dllexport) void RS16ParityCalculator_SSEEncodeStandard(RS16ParityCalculator* rsc, uint16_t* data);
		__declspec(dllexport) void RS16ParityCalculator_SSEEncodeAlternate(RS16ParityCalculator* rsc, uint16_t* data);
		__declspec(dllexport) uint16_t* RS16ParityCalculator_GetParity(RS16ParityCalculator* rsc);
		__declspec(dllexport) int RS16ParityCalculator_GetNParityCodeWords(RS16ParityCalculator* rsc);
		__declspec(dllexport) int RS16ParityCalculator_GetNMessages(RS16ParityCalculator* rsc);
	}
}
