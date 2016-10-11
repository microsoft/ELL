////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     lossFunctions.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"
%}

%inline %{

class ELL_LogLoss {
public:
    double Evaluate(double p, double l) { return _loss.Evaluate(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
private:
    emll::lossFunctions::LogLoss _loss;
};

class ELL_HingeLoss {
public:
    double Evaluate(double p, double l) { return _loss.Evaluate(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
private:
    emll::lossFunctions::HingeLoss _loss;
};

class ELL_SquaredLoss {
public:
    double Evaluate(double p, double l) { return _loss.Evaluate(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
private:
    emll::lossFunctions::SquaredLoss _loss;
};
%}
