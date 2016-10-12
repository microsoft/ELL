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
    ELL_LogLoss(double s = 1.0) { _loss = emll::lossFunctions::LogLoss(s); }
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
    double BregmanGenerator(double v) { return _loss.BregmanGenerator(v); }
private:
    emll::lossFunctions::SquaredLoss _loss;
};

%}
