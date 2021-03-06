#include "RS16Syndrome.h"
#include <iostream>
#include "GaloisField16.h"

namespace ReedSolomon {

	RS16Syndrome::RS16Syndrome(int nParityCodeWords, int nMessages) {

		_nParityCodeWords = nParityCodeWords;
		_nMessages = nMessages;

		syndrome = new uint16_t[nParityCodeWords * nMessages];
		memset(syndrome, 0, nParityCodeWords * nMessages * sizeof(uint16_t));
		temp1 = new uint16_t[nMessages];
	}

	RS16Syndrome::~RS16Syndrome() {
		delete[] syndrome;
		delete[] temp1;
	}

	void RS16Syndrome::Encode(uint16_t* data) const {
		for (int i = 0; i < _nParityCodeWords; ++i) {
			GaloisField16::Multiply(GaloisField16::Exp(i), syndrome + i * _nMessages, temp1, _nMessages);
			GaloisField16::Add(data, temp1, syndrome + i * _nMessages, _nMessages);
		}
	}

	void RS16Syndrome::SSEEncode(uint16_t* data) const {
		for (int i = 0; i < _nParityCodeWords; ++i) {
			GaloisField16::SSEMultiplyStandard(GaloisField16::Exp(i), syndrome + i * _nMessages, temp1, _nMessages);
			GaloisField16::SSEAdd(data, temp1, syndrome + i * _nMessages, _nMessages);
		}
	}

	RS16Syndrome* RS16Syndrome_Construct(int nParityCodeWords, int nMessages) {
		return new RS16Syndrome(nParityCodeWords, nMessages);
	}

	void RS16Syndrome_Destruct(RS16Syndrome* rsc) {
		delete rsc;
	}

	void RS16Syndrome_Encode(RS16Syndrome* rsc, uint16_t* data) {
		rsc->Encode(data);
	}

	void RS16Syndrome_SSEEncode(RS16Syndrome* rsc, uint16_t* data) {
		rsc->SSEEncode(data);
	}

	uint16_t* RS16Syndrome_GetSyndrome(RS16Syndrome* rsc) { return rsc->GetSyndrome(); }

	int RS16Syndrome_GetNParityCodeWords(RS16Syndrome* rsc) { return rsc->GetNParityCodeWords(); }

	int RS16Syndrome_GetNMessages(RS16Syndrome* rsc) { return rsc->GetNMessages(); }
}