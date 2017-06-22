//
// ELL SWIG header for module ELL_step10
//

#pragma once

#ifndef SWIG
#include "CallbackInterface.h"
#include "ClockInterface.h"
//
// ELL header for module ELL_step10
//

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
//
// Types
//


//
// Functions
//

// Input size: 2
// Output size: 10
void Step10_Predict(double*, double*);

int8_t SteppableMap_10_50_DataCallback(double*);

int32_t ELL_step10_GetInputSize();

int32_t ELL_step10_GetOutputSize();

int32_t ELL_step10_GetNumNodes();

void Step10(double*, double*);

double Step10_WaitTimeForNextPredict();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SWIG

void Step10_Predict(const std::vector<double>& input, std::vector<double>& output);

#ifndef SWIG
void Step10_Predict(const std::vector<double>& input, std::vector<double>& output)
{
    Step10_Predict(const_cast<double*>(&input[0]), &output[0]);
}
#endif // SWIG

class ELL_step10Predictor : public ell::api::common::CallbackForwarder<double>
{
public:
    ELL_step10Predictor() = default;
    virtual ~ELL_step10Predictor() = default;
    std::vector<double> Step();
    double WaitTimeForNextStep();
    static ELL_step10Predictor& GetInstance(ell::api::common::CallbackBase<double>& callback, std::vector<double>& buffer);

private:
    static constexpr size_t inputSize = 2;
    static constexpr size_t outputSize = 10;
    double _input[inputSize];
    double _output[outputSize];
};

#ifndef SWIG
static ELL_step10Predictor _predictor;
#ifdef __cplusplus
extern "C"
{
#endif
int8_t SteppableMap_10_50_DataCallback (double* buffer)
{
    return static_cast<int8_t>(_predictor.Invoke(buffer));
}
#ifdef __cplusplus
} // extern "C"
#endif

std::vector<double> ELL_step10Predictor::Step()
{
    Step10(_input, _output);
    return std::vector<double>(_output, _output + outputSize);
}

double ELL_step10Predictor::WaitTimeForNextStep()
{
    return Step10_WaitTimeForNextPredict();
}

ELL_step10Predictor& ELL_step10Predictor::GetInstance(ell::api::common::CallbackBase<double>& callback, std::vector<double>& buffer)
{
    _predictor.InitializeOnce(callback, buffer);
    return _predictor;
}

#endif // SWIG

