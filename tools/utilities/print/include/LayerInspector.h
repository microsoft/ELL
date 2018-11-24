#pragma once

// model
#include "InputPort.h"
#include "Model.h"
#include "NeuralNetworkPredictorNode.h"
#include "Node.h"

// stl
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