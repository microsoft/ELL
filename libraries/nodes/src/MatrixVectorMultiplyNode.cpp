////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorMultiplyNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixVectorMultiplyNode.h"

// math
#include "Matrix.h"
#include "MatrixOperations.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    MatrixVectorMultiplyNode<ValueType>::MatrixVectorMultiplyNode()
        : CompilableNode({ &_inputMatrix, &_inputVector }, { &_output }), _inputMatrix(this, {}, inputMatrixPortName), _inputVector(this, {}, inputVectorPortName), _output(this, defaultOutputPortName, 0), _m(0), _n(0), _lda(0), _incx(0)
    {
    }

    template <typename ValueType>
    MatrixVectorMultiplyNode<ValueType>::MatrixVectorMultiplyNode(const model::PortElements<ValueType>& inputMatrix, size_t m, size_t n, size_t matrixStride, const model::PortElements<ValueType>& inputVector)
        : CompilableNode({ &_inputMatrix, &_inputVector }, { &_output }), _inputMatrix(this, inputMatrix, inputMatrixPortName), _inputVector(this, inputVector, inputVectorPortName), _output(this, defaultOutputPortName, m), _m(m), _n(n), _lda(matrixStride), _incx(1)
    {
        if (inputMatrix.Size() != m * n)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }

        if (inputVector.Size() != n)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input sizes must match");
        }
    }

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::Compute() const
    {
        auto inputMatrixValues = inputMatrix.GetValue();
        assert(inputMatrix.Size() == _m * _n);
        auto inputVectorValues = inputVector.GetValue();
        assert(inputVector.Size() == _n);
        std::vector<ValueType> outputVectorValues(_m);

        math::RowMatrixReference<ValueType> inputMatrixRef(inputMatrixValues.data(), _m, _n);
        math::ColumnVectorReference<ValueType> inputVectorRef(inputVectorValues.data(), _n);
        math::ColumnVectorReference<ValueType> outputVectorRef(outputVectorValues.data(), _m);

        math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), inputMatrixRef, inputVectorRef, static_cast<ValueType>(0.0), outputVectorRef);

        _output.SetOutput(outputVectorValues);
    };

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto matrixElements = transformer.TransformPortElements(_inputMatrix.GetPortElements());
        auto vectorElements = transformer.TransformPortElements(_inputVector.GetPortElements());
        auto newNode = transformer.AddNode<MatrixVectorMultiplyNode<ValueType>>(matrixElements, _m, _n, _lda, vectorElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInputMatrix = compiler.EnsurePortEmitted(inputMatrix);
        llvm::Value* pInputVector = compiler.EnsurePortEmitted(inputVector);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        function.CallGEMV<ValueType>((int)_m, (int)_n, pInputMatrix, (int)_lda, pInputVector, _incx, pOutput, 1);
    }

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputMatrixPortName] << _inputMatrix;
        archiver[inputVectorPortName] << _inputVector;
        archiver[defaultOutputPortName] << _output;
        archiver["m"] << _m;
        archiver["n"] << _n;
        archiver["lda"] << _lda;
        archiver["incx"] << _incx;
    }

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputMatrixPortName] >> _inputMatrix;
        archiver[inputVectorPortName] >> _inputVector;
        archiver[defaultOutputPortName] >> _output;
        archiver["m"] >> _m;
        archiver["n"] >> _n;
        archiver["lda"] >> _lda;
        archiver["incx"] >> _incx;
    }

    // Explicitly instantiate versions
    template class MatrixVectorMultiplyNode<float>;
    template class MatrixVectorMultiplyNode<double>;
}
}
