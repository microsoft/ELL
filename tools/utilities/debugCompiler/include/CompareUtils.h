//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareUtils.h (compare)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VectorStats.h"

// utilities
#include "Exception.h"
#include "Files.h"

// model
#include "DynamicMap.h"
#include "Node.h"
#include "PortElements.h"

// nodes
#include "NeuralNetworkPredictorNode.h"

// predictors
#include "IPredictor.h"
#include "NeuralNetworkPredictor.h"

// stl
#include <algorithm>
#include <cmath>
#include <memory>
#include <ostream>
#include <type_traits>
#include <vector>

typedef std::array<size_t, 3> Shape; // rows, cols, numChannels

//
// General utility functions
//
std::string SanitizeTypeName(std::string layerType);
std::string GetLayerFilename(const std::string& layerType, int index);
std::string GetNodeOutputFilename(const std::string& layerType, int index, const std::string& suffix);
std::string GetNodeFilename(const std::string& nodeType, int index);

void PrintPortElements(std::ostream& os, const ell::model::PortElementsBase& elements);
Shape GetInputShape(const ell::model::DynamicMap& map);

class IOState
{
public:
    IOState(std::ostream& _stream);
    ~IOState();

private:
    std::ostream& _stream;
    std::ios_base::fmtflags _flags;
    std::streamsize _precision;
    std::streamsize _width;
};

//
// Result data-related
//
struct OutputInfo
{
    std::string name;
    std::vector<double> outputs;

    OutputInfo() = default;

    template <typename ValueType>
    OutputInfo(const std::string& name, const std::vector<ValueType>& outputs)
        : name(name), outputs(outputs.begin(), outputs.end())
    {
    }
};

//
// Sink-node-related
//
std::string GetSinkNodeLabel(const ell::model::Node* node);

//
// Neural-net-related functions
//
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node);

template <typename ValueType>
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node);

bool IsNeuralNetworkLayerNode(const ell::model::Node* node);

template <typename ValueType>
bool IsNeuralNetworkLayerNode(const ell::model::Node* node);

// Non-templated version, checks all types
bool HasNeuralNetworkPredictor(const ell::model::DynamicMap& map);

template <typename ValueType>
bool HasNeuralNetworkPredictor(const ell::model::DynamicMap& map);

template <typename ValueType>
ell::predictors::NeuralNetworkPredictor<ValueType> GetNeuralNetworkPredictor(ell::model::DynamicMap& map);

//
// Report-writing
//
template <typename ValueType>
void WriteVector(const std::string& filename, const std::vector<ValueType>& vec);

void WriteReportHeader(std::ostream& reportStream, const std::string& modelName);

void WriteModelReport(std::ostream& reportStream, const OutputInfo& layerOutput, const OutputInfo& nodeOutput);

void WriteLayerReport(std::ostream& reportStream, int index, const ell::model::Node* layerNode, const OutputInfo& layerOutput, const OutputInfo& nodeOutput);

template <typename FunctionType>
void WriteStatsRow(std::ostream& reportStream, const VectorStats& layerStats, const VectorStats& nodeStats, const VectorStats& diffStats, const std::string& header, FunctionType getValueFunction);

void WriteReportFooter(std::ostream& reportStream);

#include "../tcc/CompareUtils.tcc"