// This file will be generated
#pragma once

#ifndef SWIG

#include "CallbackInterface.h"

extern "C" {
void Step10(double* input, double* output);
bool SteppableMap_10_50_DataCallback(double* buffer);
}

#endif // SWIG

class Step10Predictor : public ell::api::common::CallbackForwarder<double>
{
public:
    Step10Predictor() = default;

    virtual ~Step10Predictor() = default;

    std::vector<double> Step();

    static Step10Predictor& GetInstance(ell::api::common::CallbackBase<double>& callback, std::vector<double>& buffer);

private:
    static constexpr size_t _outputSize = 10;

    double _input[2];
    double _output[_outputSize];
};

#ifndef SWIG

static Step10Predictor _predictor;

#endif // SWIG
