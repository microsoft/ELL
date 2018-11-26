#pragma once

#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/Node.h>

#include <nodes/include/NeuralNetworkPredictorNode.h>

#include <predictors/neural/include/ActivationLayer.h>
#include <predictors/neural/include/BinaryConvolutionalLayer.h>
#include <predictors/neural/include/ConvolutionalLayer.h>
#include <predictors/neural/include/PoolingLayer.h>
#include <predictors/neural/include/ScalingLayer.h>

#include <memory>
#include <vector>

namespace ell
{
struct NameValue
{
    std::string name;
    std::string value;
};
} // namespace ell

#include "../tcc/LayerInspector.tcc"
