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
#ifndef SWIG
    ELL_LogLoss(const emll::lossFunctions::LogLoss& other) { _loss = other; }
#endif
private:
    emll::lossFunctions::LogLoss _loss;
};

class ELL_HingeLoss {
public:
    double Evaluate(double p, double l) { return _loss.Evaluate(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
    ELL_HingeLoss() {}
#ifndef SWIG
    ELL_HingeLoss(const emll::lossFunctions::HingeLoss& other) { _loss = other; }
#endif
private:
    emll::lossFunctions::HingeLoss _loss;
};

class ELL_SquaredLoss {
public:
    double Evaluate(double p, double l) { return _loss.Evaluate(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
    double BregmanGenerator(double v) { return _loss.BregmanGenerator(v); }
    ELL_SquaredLoss() {}
#ifndef SWIG
    ELL_SquaredLoss(const emll::lossFunctions::SquaredLoss& other) { _loss = other; }
#endif
private:
    emll::lossFunctions::SquaredLoss _loss;
};

%}
