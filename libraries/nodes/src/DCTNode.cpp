////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCTNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DCTNode.h"

// nodes
#include "MatrixVectorProductNode.h"

// dsp
#include "DCT.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DCTNode<ValueType>::DCTNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _dctCoeffs(0, 0)
    {
    }

    template <typename ValueType>
    DCTNode<ValueType>::DCTNode(const model::PortElements<ValueType>& input, size_t numFilters)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, numFilters), _dctCoeffs(0, 0)
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
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DCTNode<ValueType>>(newPortElements, _dctCoeffs.NumRows());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DCTNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<MatrixVectorProductNode<ValueType, math::MatrixLayout::rowMajor>>(newPortElements, _dctCoeffs);
        transformer.MapNodeOutput(output, newNode->output);
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
} // nodes
} // ell
