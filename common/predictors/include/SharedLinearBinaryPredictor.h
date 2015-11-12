// SharedLinearBinaryPredictor.h

#pragma once

#include "types.h"

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include "DoubleVector.h"
using namespace linear;

namespace predictors
{

	class SharedLinearBinaryPredictor
	{
	public:
		SharedLinearBinaryPredictor(uint dim);

		/// \returns the underlying DoubleVector
		///
		DoubleVector& GetVector();

		/// \returns the const reference underlying DoubleVector
		///
		const DoubleVector& GetVector() const;

		/// \returns the underlying bias
		///
		double& GetBias();

		/// \returns the underlying bias
		///
		double GetBias() const;

		/// \returns the output of the predictor for a given example
		///
		template<typename DatavectorType>
		double Predict(const DatavectorType& example) const;

	private:
		struct BiasedVector
		{
			BiasedVector(uint dim);
			DoubleVector w;
			double b;
		};

		shared_ptr<BiasedVector> _sp_predictor;
	};
}

#include "../tcc/SharedLinearBinaryPredictor.tcc"
