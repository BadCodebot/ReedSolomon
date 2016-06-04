#pragma once
#include <cstdint>

namespace ReedSolomon {

	class RS16Syndrome {

	public:

		RS16Syndrome(int nParityCodeWords, int nMessages);
		~RS16Syndrome();

		void Encode(uint16_t* data) const;
		void SSEEncode(uint16_t* data) const;

		uint16_t* GetSyndrome() const { return syndrome; }
		int GetNParityCodeWords() const { return _nParityCodeWords; }
		int GetNMessages() const { return _nMessages; }

	private:

		int _nParityCodeWords;
		int _nMessages;

		uint16_t* syndrome;
		uint16_t* temp1;
	};

	extern "C" {
		__declspec(dllexport) RS16Syndrome* RS16SyndromeCalculator_Construct(int nParityCodeWords, int nMessages);
		__declspec(dllexport) void RS16SyndromeCalculator_Destruct(RS16Syndrome* rsc);
		__declspec(dllexport) void RS16SyndromeCalculator_Encode(RS16Syndrome* rsc, uint16_t* data);
		__declspec(dllexport) void RS16SyndromeCalculator_SSEEncode(RS16Syndrome* rsc, uint16_t* data);
		__declspec(dllexport) uint16_t* RS16SyndromeCalculator_GetSyndrome(RS16Syndrome* rsc);
		__declspec(dllexport) int RS16SyndromeCalculator_GetNParityCodeWords(RS16Syndrome* rsc);
		__declspec(dllexport) int RS16SyndromeCalculator_GetNMessages(RS16Syndrome* rsc);
	}
}
