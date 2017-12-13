////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     functions.i (interfaces)
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

namespace ELL_API
{

class LogLoss {
public:
    LogLoss() = default;
    double Evaluate(double p, double l) { return _loss(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
#ifndef SWIG
    LogLoss(const ell::functions::LogLoss& other) { _loss = other; }
#endif
private:
    ell::functions::LogLoss _loss;
};

class HingeLoss {
public:
    HingeLoss() = default;
    double Evaluate(double p, double l) { return _loss(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
#ifndef SWIG
    HingeLoss(const ell::functions::HingeLoss& other) { _loss = other; }
#endif
private:
    ell::functions::HingeLoss _loss;
};

class SquaredLoss {
public:
    SquaredLoss() = default;
    double Evaluate(double p, double l) { return _loss(p,l); }
    double GetDerivative(double p, double l) { return _loss.GetDerivative(p, l); }
    double BregmanGenerator(double v) { return _loss.BregmanGenerator(v); }
#ifndef SWIG
    SquaredLoss(const ell::functions::SquaredLoss& other) { _loss = other; }
#endif
private:
    ell::functions::SquaredLoss _loss;
};

} // end namespace
%}
