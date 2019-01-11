////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformData.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <model/include/Model.h>
#include <model/include/OutputPort.h>

template <typename DataVectorType>
DataVectorType RemovePadding(const DataVectorType& data, const ell::utilities::MemoryLayout& layout);

template <typename ElementType>
UnlabeledDataContainer TransformDataWithModel(const UnlabeledDataContainer& dataset, ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
BinaryLabelDataContainer TransformDataInputsWithModel(const BinaryLabelDataContainer& dataset, ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
MultiClassDataContainer TransformDataInputsWithModel(const MultiClassDataContainer& dataset, ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
VectorLabelDataContainer TransformDataInputsWithModel(const VectorLabelDataContainer& dataset, ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

double GetModelAccuracy(ell::model::Model& model, const ell::model::OutputPortBase& output, const BinaryLabelDataContainer& testDataset);
double GetModelAccuracy(ell::model::Model& model, const ell::model::OutputPortBase& output, const MultiClassDataContainer& testDataset);
double GetModelAccuracy(ell::model::Model& model, const ell::model::OutputPortBase& output, const VectorLabelDataContainer& testDataset);
