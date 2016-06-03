#include "RS16SyndromeCalculator.h"
#include <iostream>
#include "GaloisField16.h"

namespace ReedSolomon {

	RS16SyndromeCalculator::RS16SyndromeCalculator(int nParityCodeWords, int nMessages) {

		_nParityCodeWords = nParityCodeWords;
		_nMessages = nMessages;

		syndrome = new uint16_t[nParityCodeWords * nMessages];
		memset(syndrome, 0, nParityCodeWords * nMessages * sizeof(uint16_t));
		temp1 = new uint16_t[nMessages];
	}

	RS16SyndromeCalculator::~RS16SyndromeCalculator() {
		delete[] syndrome;
		delete[] temp1;
	}

	void RS16SyndromeCalculator::Encode(uint16_t* data) const {
		for (int i = 0; i < _nParityCodeWords; ++i) {
			GaloisField16::Multiply(GaloisField16::Exp(i), syndrome + i * _nMessages, temp1, _nMessages);
			GaloisField16::Add(data, temp1, syndrome + i * _nMessages, _nMessages);
		}
	}

	void RS16SyndromeCalculator::SSEEncode(uint16_t* data) const {
		for (int i = 0; i < _nParityCodeWords; ++i) {
			GaloisField16::SSEMultiplyStandard(GaloisField16::Exp(i), syndrome + i * _nMessages, temp1, _nMessages);
			GaloisField16::SSEAdd(data, temp1, syndrome + i * _nMessages, _nMessages);
		}
	}

	RS16SyndromeCalculator* RS16SyndromeCalculator_Construct(int nParityCodeWords, int nMessages) {
		return new RS16SyndromeCalculator(nParityCodeWords, nMessages);
	}

	void RS16SyndromeCalculator_Destruct(RS16SyndromeCalculator* rsc) {
		delete rsc;
	}

	void RS16SyndromeCalculator_Encode(RS16SyndromeCalculator* rsc, uint16_t* data) {
		rsc->Encode(data);
	}

	void RS16SyndromeCalculator_SSEEncode(RS16SyndromeCalculator* rsc, uint16_t* data) {
		rsc->SSEEncode(data);
	}

	uint16_t* RS16SyndromeCalculator_GetSyndrome(RS16SyndromeCalculator* rsc) { return rsc->GetSyndrome(); }

	int RS16SyndromeCalculator_GetNParityCodeWords(RS16SyndromeCalculator* rsc) { return rsc->GetNParityCodeWords(); }

	int RS16SyndromeCalculator_GetNMessages(RS16SyndromeCalculator* rsc) { return rsc->GetNMessages(); }
}