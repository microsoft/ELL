////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCTNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DCTNode.h"

#include "MatrixVectorProductNode.h"

#include <dsp/include/DCT.h>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DCTNode<ValueType>::DCTNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _dctCoeffs(0, 0)
    {
    }

    template <typename ValueType>
    DCTNode<ValueType>::DCTNode(const model::OutputPort<ValueType>& input, size_t numFilters) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, numFilters),
        _dctCoeffs(0, 0)
    {
        _dctCoeffs = dsp::GetDCTMatrix<ValueType>(numFilters, _input.Size());
    }

    template <typename ValueType>
    void DCTNode<ValueType>::Compute() const
    {
        math::ColumnVector<ValueType> x(_input.GetValue());
        auto result = dsp::DCT(_dctCoeffs, x);
        _output.SetOutput(result.ToArray());
    };

    template <typename ValueType>
    void DCTNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<DCTNode<ValueType>>(newInputs, _dctCoeffs.NumRows());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DCTNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        const auto& result = MatrixVectorProduct(newInputs, _dctCoeffs);
        transformer.MapNodeOutput(output, result);
        return true;
    }

    template <typename ValueType>
    void DCTNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["numFilters"] << _dctCoeffs.NumRows();
    }

    template <typename ValueType>
    void DCTNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        size_t numFilters = 0;
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["numFilters"] >> numFilters;
        _dctCoeffs = dsp::GetDCTMatrix<ValueType>(numFilters, _input.Size());
        _output.SetSize(numFilters);
    }

    // Explicit instantiations
    template class DCTNode<float>;
    template class DCTNode<double>;
} // namespace nodes
} // namespace ell
