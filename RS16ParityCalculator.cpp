#pragma once
#include "RS16ParityCalculator.h"
#include "GaloisField16.h"
#include "Polynomial.h"
#include <iostream>

namespace ReedSolomon {

	RS16ParityCalculator::RS16ParityCalculator(int nParityCodeWords, int nMessages) {

		_nParityCodeWords = nParityCodeWords;
		_nMessages = nMessages;

		parity = new uint16_t[nParityCodeWords * nMessages];
		memset(parity, 0, nParityCodeWords * nMessages * sizeof(uint16_t));
		temp1 = new uint16_t[nMessages];
		temp2 = new uint16_t[nMessages];

		Polynomial generatorPolynomial = Polynomial::Create(1);
		for (int i = 0; i < nParityCodeWords; i++) {
			generatorPolynomial *= Polynomial::Create(GaloisField16::Exp(i), 1);
		}

		generator = new uint16_t[nParityCodeWords];
		for (int i = 0; i < nParityCodeWords; ++i) {
			generator[i] = generatorPolynomial[nParityCodeWords - 1 - i];
		}
	}

	RS16ParityCalculator::~RS16ParityCalculator() {
		delete[] parity;
		delete[] generator;
		delete[] temp1;
		delete[] temp2;
	}

	void RS16ParityCalculator::Encode(uint16_t* data) const {
		GaloisField16::Add(parity, data, temp1, _nMessages);
		for (int i = 0; i < _nParityCodeWords - 1; ++i) {
			GaloisField16::Multiply(generator[i], temp1, temp2, _nMessages);
			GaloisField16::Add(temp2, parity + (i + 1) * _nMessages, parity + i * _nMessages, _nMessages);
		}
		GaloisField16::Multiply(generator[_nParityCodeWords - 1], temp1, parity + (_nParityCodeWords - 1)*_nMessages, _nMessages);
	}

	void RS16ParityCalculator::SSEEncodeStandard(uint16_t* data) const {
		GaloisField16::SSEAdd(parity, data, temp1, _nMessages);
		for (int i = 0; i < _nParityCodeWords - 1; ++i) {
			GaloisField16::SSEMultiplyStandard(generator[i], temp1, temp2, _nMessages);
			GaloisField16::SSEAdd(temp2, parity + (i + 1) * _nMessages, parity + i * _nMessages, _nMessages);
		}
		GaloisField16::SSEMultiplyStandard(generator[_nParityCodeWords - 1], temp1, parity + (_nParityCodeWords - 1)*_nMessages, _nMessages);
	}

	void RS16ParityCalculator::SSEEncodeAlternate(uint16_t* data) const {

		GaloisField16::SSEAdd(parity, data, temp1, _nMessages);
		for (int i = 0; i < _nParityCodeWords - 1; ++i) {
			GaloisField16::SSEMultiplyAlternate(generator[i], temp1, temp2, _nMessages);
			GaloisField16::SSEAdd(temp2, parity + (i + 1) * _nMessages, parity + i * _nMessages, _nMessages);
		}
		GaloisField16::SSEMultiplyAlternate(generator[_nParityCodeWords - 1], temp1, parity + (_nParityCodeWords - 1)*_nMessages, _nMessages);
	}

	RS16ParityCalculator* ReedSolomon16Calculate_Construct(int nParityCodeWords, int nMessages) {
		return new RS16ParityCalculator(nParityCodeWords, nMessages);
	}

	void RS16ParityCalculator_Destruct(RS16ParityCalculator* rsc) {
		delete rsc;
	}

	void RS16ParityCalculator_Encode(RS16ParityCalculator* rsc, uint16_t* data) {
		rsc->Encode(data);
	}

	void RS16ParityCalculator_SSEEncodeStandard(RS16ParityCalculator* rsc, uint16_t* data) {
		rsc->SSEEncodeStandard(data);
	}

	void RS16ParityCalculator_SSEEncodeAlternate(RS16ParityCalculator* rsc, uint16_t* data) {
		rsc->SSEEncodeAlternate(data);
	}

	uint16_t* RS16ParityCalculator_GetParity(RS16ParityCalculator* rsc) { return rsc->GetParity(); }

	int RS16ParityCalculator_GetNParityCodeWords(RS16ParityCalculator* rsc) { return rsc->GetNParityCodeWords(); }

	int RS16ParityCalculator_GetNMessages(RS16ParityCalculator* rsc) { return rsc->GetNMessages(); }
}