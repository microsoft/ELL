////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     BinaryClassificationEvaluator.cpp (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryClassificationEvaluator.h"

namespace utilities
{
    BinaryClassificationEvaluator::BinaryClassificationEvaluator()
    {}

    double BinaryClassificationEvaluator::GetLastLoss() const
    {
        if(_evals.size() == 0)
        {
            return 0;
        }

        return _evals[_evals.size()-1].loss;
    }

    double BinaryClassificationEvaluator::GetLastError() const
    {
        if(_evals.size() == 0)
        {
            return 0;
        }

        return _evals[_evals.size()-1].error;
    }

    void BinaryClassificationEvaluator::Print(std::ostream & os) const
    {
        os << "binary classification evaluation\nloss\terror\n";
        for(auto& e : _evals)
        {
            os << e.loss << '\t' << e.error << std::endl;
        }
    }

    std::ostream& operator<<(std::ostream & os, const BinaryClassificationEvaluator & evaluator)
    {
        evaluator.Print(os);
        return os;
    }
}
