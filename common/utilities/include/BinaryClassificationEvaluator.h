// BinaryClassificationEvaluator.h

#pragma once

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;
using std::endl;

namespace utilities
{
	class BinaryClassificationEvaluator
	{
	public:

		struct evaluation
		{
			double loss = 0;
			double error = 0;
		};
		
		BinaryClassificationEvaluator();

		template<typename ExampleIteratorType, typename PredictorType, typename LossFunctionType>
		void Evaluate(ExampleIteratorType& data_iter, const PredictorType& predictor, const LossFunctionType& loss_function);

		/// \returns The average weighted loss
		///
		double GetLastLoss() const;

		/// \returns The weighted error rate
		///
		double GetLastError() const;

		/// Prints losses and errors to an ostream
		void Print(ostream& os) const;

	private:
		vector<evaluation> _evals;
	};

	ostream& operator<<(ostream& os, const BinaryClassificationEvaluator& evaluation);
}

#include "../tcc/BinaryClassificationEvaluator.tcc"
