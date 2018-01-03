////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixMatrixMultiplyNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixMatrixMultiplyNode.h"

// math
#include "Matrix.h"
#include "MatrixOperations.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    MatrixMatrixMultiplyNode<ValueType>::MatrixMatrixMultiplyNode()
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, defaultInput1PortName), _input2(this, {}, defaultInput2PortName), _output(this, defaultOutputPortName, 0), _m(0), _n(0), _k(0), _lda(0), _ldb(0), _ldc(0), _transpose1(false), _transpose2(false)
    {
    }

    template<typename ValueType>
    MatrixMatrixMultiplyNode<ValueType>::MatrixMatrixMultiplyNode(const model::PortElements<ValueType>& input1, size_t m, size_t n, size_t k, size_t matrix1Stride, const model::PortElements<ValueType>& input2, size_t matrix2Stride, size_t outputMatrixStride)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _output(this, defaultOutputPortName, m * n), _m(m), _n(n), _k(k), _lda(matrix1Stride), _ldb(matrix2Stride), _ldc(outputMatrixStride), _transpose1(false), _transpose2(false)
    {
        if (input1.Size() != m * k)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input matrix 1 size incorrect");
        }

        if (input2.Size() != k * n)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input matrix 2 size incorrect");
        }
    }

    template <typename ValueType>
    MatrixMatrixMultiplyNode<ValueType>::MatrixMatrixMultiplyNode(const model::PortElements<ValueType>& input1, size_t m, size_t n, size_t k, size_t matrix1Stride, bool transpose1, const model::PortElements<ValueType>& input2, size_t matrix2Stride, bool transpose2, size_t outputMatrixStride)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, defaultInput1PortName), _input2(this, input2, defaultInput2PortName), _output(this, defaultOutputPortName, m * n), _m(m), _n(n), _k(k), _lda(matrix1Stride), _ldb(matrix2Stride), _ldc(outputMatrixStride), _transpose1(transpose1), _transpose2(transpose2)
    {
        if (input1.Size() != m * k)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input matrix 1 size incorrect");
        }

        if (input2.Size() != k * n)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input matrix 2 size incorrect");
        }
    }

    template<typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::Compute() const
    {
        assert(!_transpose1 && !_transpose2 && "MatrixMatrixMultiplyNode::Compute() with transposed matrix not yet implemented!");

        assert(input1.Size() == _m * _k);
        assert(input2.Size() == _k * _n);
        auto inputMatrix1Values = input1.GetValue();
        auto inputMatrix2Values = input2.GetValue();
        std::vector<ValueType> outputMatrixValues(_m * _n);

        math::RowMatrixReference<ValueType> inputMatrix1Ref(inputMatrix1Values.data(), _m, _k);
        math::RowMatrixReference<ValueType> inputMatrix2Ref(inputMatrix2Values.data(), _k, _n);
        math::RowMatrixReference<ValueType> outputMatrixRef(outputMatrixValues.data(), _m, _n);

        // TODO: transpose if necessary
        math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), inputMatrix1Ref, inputMatrix2Ref, static_cast<ValueType>(0.0), outputMatrixRef);

        _output.SetOutput(outputMatrixValues);
    };

    template<typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(PortElements1, _m, _n, _k, _lda, _transpose1, PortElements2, _ldb, _transpose2, _ldc);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template<typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {  
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        function.CallGEMM<ValueType>(_transpose1, _transpose2, (int)_m, (int)_n, (int)_k, pInput1, (int)_lda, pInput2, (int)_ldb, pOutput, (int)_ldc);
    }

    template<typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver[defaultOutputPortName] << _output;
        archiver["m"] << _m;
        archiver["n"] << _n;
        archiver["k"] << _k;
        archiver["lda"] << _lda;
        archiver["ldb"] << _ldb;
        archiver["ldc"] << _ldc;
        archiver["transpose1"] << _transpose1;
        archiver["transpose2"] << _transpose2;
    }

    template<typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        archiver[defaultOutputPortName] >> _output;
        archiver["m"] >> _m;
        archiver["n"] >> _n;
        archiver["k"] >> _k;
        archiver["lda"] >> _lda;
        archiver["ldb"] >> _ldb;
        archiver["ldc"] >> _ldc;
        archiver["transpose1"] >> _transpose1;
        archiver["transpose2"] >> _transpose2;
    }

    // Explicitly instantiate versions
    template class MatrixMatrixMultiplyNode<float>;
    template class MatrixMatrixMultiplyNode<double>;
}
}
