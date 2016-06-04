#include "RS16Repair.h"
#include "GaloisField16.h"
#include <stdexcept>
#include <iostream>

namespace ReedSolomon {

	RS16Repair::RS16Repair(const RS16Syndrome& rss, int nCodeWords, int* errorLocations, int errorCount)
		: _rss(rss), _nCodeWords(nCodeWords), _correctionMatrix(errorCount), errorCount(errorCount) {

		// Make sure we don't have too many errors
		if (errorCount > rss.GetNParityCodeWords()) throw std::invalid_argument("Too many errors");

		// convert the error codeword indexes to the corresponding polynomial order
		errorOrders = new int[errorCount];
		for (int i = 0; i < errorCount; i++) errorOrders[i] = nCodeWords - 1 - errorLocations[i];

		// Create the correction matrix
		for (int r = 0; r < errorCount; r++) {
			for (int c = 0; c < errorCount; c++) {
				_correctionMatrix[r][c] = GaloisField16::Power(2, r * errorOrders[c]);
			}
		}
		_correctionMatrix.Invert();
	}

	RS16Repair::~RS16Repair() {
		delete[] errorOrders;
	}

	void RS16Repair::Correction(int errorLocationOffset, uint16_t* data) const {
		for (int i = 0; i < _rss.GetNMessages(); i++) {
			data[i] = 0;
			for (int j = 0; j < errorCount; j++) {
				data[i] = GaloisField16::Add(data[i], GaloisField16::Multiply(*(_rss.GetSyndrome() + i + j * _rss.GetNMessages()), _correctionMatrix[errorLocationOffset][j]));
			}
		}
	}

	RS16Repair* RS16Repair_Construct(const RS16Syndrome* rss, int nCodeWords, int* errorLocations, int errorCount) {
		return new RS16Repair(*rss, nCodeWords, errorLocations, errorCount);
	}

	void RS16Repair_Destruct(RS16Repair* rsr) {
		delete rsr;
	}

	void RS16Repair_Correction(RS16Repair* rsr, int errorLocationOffset, uint16_t* data) {
		rsr->Correction(errorLocationOffset, data);
	}

	int RS16Repair_GetNCodeWords(RS16Repair* rsr) {
		return rsr->GetNCodeWords();
	}
}