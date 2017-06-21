// This file will be generated

#include "ELL_step10.h"
#include "ClockInterface.h"

extern "C" {
bool SteppableMap_10_50_DataCallback(double* buffer)
{
    return _predictor.Invoke(buffer);
}
}

Step10Predictor& Step10Predictor::GetInstance(ell::api::common::CallbackBase<double>& callback, std::vector<double>& buffer)
{
    _predictor.InitializeOnce(callback, buffer);
    return _predictor;
}

std::vector<double> Step10Predictor::Step()
{
    Step10(_input, _output);
    return std::vector<double>(_output, _output + _outputSize);
}
