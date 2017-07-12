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
#include "Operations.h"

// BLAS
#ifdef USE_BLAS
#include "cblas.h"
#else
enum CBLAS_ORDER
{
    CblasRowMajor = 101,
    CblasColMajor = 102
};

enum CBLAS_TRANSPOSE
{
    CblasNoTrans = 111,
    CblasTrans = 112
};
#endif

namespace ell
{
namespace nodes
{
    namespace
    {
        // Useful aliases for operators
        const auto plus = emitters::TypedOperator::add;
        const auto minus = emitters::TypedOperator::subtract;
        const auto times = emitters::TypedOperator::multiply;
        const auto divide = emitters::TypedOperator::divideSigned;
        const auto modulo = emitters::TypedOperator::moduloSigned;

        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto minusFloat = emitters::TypedOperator::subtractFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;
        const auto divideFloat = emitters::TypedOperator::divideFloat;

        template <typename ValueType>
        void EmitMatrixMatrixMultiplyBlas(emitters::IRFunctionEmitter& function, bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc)
        {
            llvm::Function* gemm = function.GetModule().GetRuntime().GetGEMMFunction<ValueType>();

            emitters::IRValueList args{
                function.Literal(CBLAS_ORDER::CblasRowMajor), // order
                function.Literal(transposeA ? CBLAS_TRANSPOSE::CblasTrans : CBLAS_TRANSPOSE::CblasNoTrans), // transposeA
                function.Literal(transposeB ? CBLAS_TRANSPOSE::CblasTrans : CBLAS_TRANSPOSE::CblasNoTrans), // transposeB
                function.Literal(m),
                function.Literal(n),
                function.Literal(k),
                function.Literal(static_cast<ValueType>(1.0)), // alpha
                A,
                function.Literal(lda), // lda
                B,
                function.Literal(ldb), // ldb
                function.Literal(static_cast<ValueType>(0.0)), // beta
                C, // C (output)
                function.Literal(ldc) // ldc
            };
            function.Call(gemm, args);
        }

        // TODO: emit this as a function in the module
        template <typename ValueType>
        void EmitMatrixMatrixMultiplySlow(emitters::IRFunctionEmitter& function, bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc)
        {
            llvm::Value* accum = function.Variable(emitters::GetVariableType<ValueType>(), "accum");

            auto mLoop = function.ForLoop();
            mLoop.Begin(m);
            {
                auto mIndex = mLoop.LoadIterationVariable();

                auto nLoop = function.ForLoop();
                nLoop.Begin(n);
                {
                    auto nIndex = nLoop.LoadIterationVariable();

                    function.Store(accum, function.Literal(static_cast<ValueType>(0.0)));
                    auto kLoop = function.ForLoop();
                    kLoop.Begin(k);
                    {
                        auto kIndex = kLoop.LoadIterationVariable();

                        llvm::Value* aIndex = nullptr;
                        llvm::Value* bIndex = nullptr;
                        if (transposeA)
                            aIndex = function.Operator(plus, function.Operator(times, kIndex, function.Literal(lda)), mIndex);
                        else
                            aIndex = function.Operator(plus, function.Operator(times, mIndex, function.Literal(lda)), kIndex);

                        if (transposeB)
                            bIndex = function.Operator(plus, function.Operator(times, nIndex, function.Literal(ldb)), kIndex);
                        else
                            bIndex = function.Operator(plus, function.Operator(times, kIndex, function.Literal(ldb)), nIndex);

                        auto aValue = function.ValueAt(A, aIndex);
                        auto bValue = function.ValueAt(B, bIndex);
                        auto value = function.Operator(timesFloat, aValue, bValue);
                        function.OperationAndUpdate(accum, plusFloat, value);
                    }
                    kLoop.End();

                    // store output in C[m,n]
                    auto cIndex = function.Operator(plus, function.Operator(times, mIndex, function.Literal(ldc)), nIndex);
                    function.SetValueAt(C, cIndex, function.Load(accum));
                }
                nLoop.End();
            }
            mLoop.End();
        }
    } // end anonymous namespace

    template <typename ValueType>
    MatrixMatrixMultiplyNode<ValueType>::MatrixMatrixMultiplyNode()
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0), _m(0), _n(0), _k(0), _lda(0), _ldb(0), _ldc(0), _transpose1(false), _transpose2(false)
    {
    }

    template <typename ValueType>
    MatrixMatrixMultiplyNode<ValueType>::MatrixMatrixMultiplyNode(const model::PortElements<ValueType>& input1, size_t m, size_t n, size_t k, size_t matrix1Stride, const model::PortElements<ValueType>& input2, size_t matrix2Stride, size_t outputMatrixStride)
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, m * n), _m(m), _n(n), _k(k), _lda(matrix1Stride), _ldb(matrix2Stride), _ldc(outputMatrixStride), _transpose1(false), _transpose2(false)
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
        : CompilableNode({ &_input1, &_input2 }, { &_output }), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, m * n), _m(m), _n(n), _k(k), _lda(matrix1Stride), _ldb(matrix2Stride), _ldc(outputMatrixStride), _transpose1(transpose1), _transpose2(transpose2)
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
    void MatrixMatrixMultiplyNode<ValueType>::Compute() const
    {
        assert(!_transpose1 && !_transpose2 && "MatrixMatrixMultiplyNode::Compute() with transposed matrix not yet implemented!");

        assert(input1.Size() == _m * _k);
        assert(input2.Size() == _k * _n);
        auto inputMatrix1Values = input1.GetValue();
        auto inputMatrix2Values = input2.GetValue();
        std::vector<ValueType> outputMatrixValues(_m * _n);

        math::RowMatrixReference<ValueType> inputMatrix1Ref(_m, _k, inputMatrix1Values.data());
        math::RowMatrixReference<ValueType> inputMatrix2Ref(_k, _n, inputMatrix2Values.data());
        math::RowMatrixReference<ValueType> outputMatrixRef(_m, _n, outputMatrixValues.data());

        // TODO: transpose if necessary
        math::Operations::Multiply(static_cast<ValueType>(1.0), inputMatrix1Ref, inputMatrix2Ref, static_cast<ValueType>(0.0), outputMatrixRef);

        _output.SetOutput(outputMatrixValues);
    };

    template <typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto PortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto PortElements2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(PortElements1, _m, _n, _k, _lda, _transpose1, PortElements2, _ldb, _transpose2, _ldc);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput1 = compiler.EnsurePortEmitted(input1);
        llvm::Value* pInput2 = compiler.EnsurePortEmitted(input2);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        if (compiler.GetMapCompilerParameters().compilerSettings.useBlas)
        {
            EmitMatrixMatrixMultiplyBlas<ValueType>(function, _transpose1, _transpose2, (int)_m, (int)_n, (int)_k, pInput1, (int)_lda, pInput2, (int)_ldb, pOutput, (int)_ldc);
        }
        else
        {
            EmitMatrixMatrixMultiplySlow<ValueType>(function, _transpose1, _transpose2, (int)_m, (int)_n, (int)_k, pInput1, (int)_lda, pInput2, (int)_ldb, pOutput, (int)_ldc);
        }
    }

    template <typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[input1PortName] << _input1;
        archiver[input2PortName] << _input2;
        archiver[outputPortName] << _output;
        archiver["m"] << _m;
        archiver["n"] << _n;
        archiver["k"] << _k;
        archiver["lda"] << _lda;
        archiver["ldb"] << _ldb;
        archiver["ldc"] << _ldc;
        archiver["transpose1"] << _transpose1;
        archiver["transpose2"] << _transpose2;
    }

    template <typename ValueType>
    void MatrixMatrixMultiplyNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[input1PortName] >> _input1;
        archiver[input2PortName] >> _input2;
        archiver[outputPortName] >> _output;
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
