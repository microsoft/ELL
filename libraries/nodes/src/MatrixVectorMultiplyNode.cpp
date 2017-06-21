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

        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;

        //
        // Matrix math
        //
        template <typename ValueType>
        void EmitMatrixVectorMultiplyBlas(emitters::IRFunctionEmitter& function, int m, int n, llvm::Value* M, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy)
        {
            llvm::Function* gemv = function.GetModule().GetRuntime().GetGEMVFunction<ValueType>();
            assert(gemv != nullptr && "Couldn't get GEMV function!");

            emitters::IRValueList args{ function.Literal(CBLAS_ORDER::CblasRowMajor),
                                        function.Literal(CBLAS_TRANSPOSE::CblasNoTrans),
                                        function.Literal(m),
                                        function.Literal(n),
                                        function.Literal(static_cast<ValueType>(1.0)), // alpha
                                        M,
                                        function.Literal(lda), // lda
                                        x, // x
                                        function.Literal(incx),
                                        function.Literal(static_cast<ValueType>(0.0)), // beta
                                        y, // y (output)
                                        function.Literal(incy) }; // incy
            function.Call(gemv, args);
        }

        // TODO: emit this as a function in the module
        template <typename ValueType>
        void EmitMatrixVectorMultiplySlow(emitters::IRFunctionEmitter& function, int m, int n, llvm::Value* M, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy)
        {
            llvm::Value* rowOffset = function.Variable(emitters::VariableType::Int32, "rowOffset");
            llvm::Value* rowPos = function.Variable(emitters::VariableType::Int32, "rowPos");
            llvm::Value* xPos = function.Variable(emitters::VariableType::Int32, "xPos");
            llvm::Value* yPos = function.Variable(emitters::VariableType::Int32, "yPos");
            llvm::Value* accum = function.Variable(emitters::GetVariableType<ValueType>(), "accum");

            function.Store(rowOffset, function.Literal(0));
            function.Store(yPos, function.Literal(0));

            auto mLoop = function.ForLoop();
            mLoop.Begin(m);
            {
                function.Store(rowPos, function.Load(rowOffset));
                function.Store(xPos, function.Literal(0));
                function.Store(accum, function.Literal(static_cast<ValueType>(0.0)));
                auto nLoop = function.ForLoop();
                nLoop.Begin(n);
                {
                    auto mVal = function.ValueAt(M, function.Load(rowPos));
                    auto xVal = function.ValueAt(x, function.Load(xPos));
                    auto mTimesX = function.Operator(timesFloat, mVal, xVal);
                    function.OperationAndUpdate(accum, plusFloat, mTimesX);

                    function.OperationAndUpdate(rowPos, plus, function.Literal(1));
                    function.OperationAndUpdate(xPos, plus, function.Literal(incx));
                }
                nLoop.End();

                function.SetValueAt(y, function.Load(yPos), function.Load(accum));
                function.OperationAndUpdate(yPos, plus, function.Literal(incy));
                function.OperationAndUpdate(rowOffset, plus, function.Literal(lda));
            }
            mLoop.End();
        }
    } // end anonymous namespace

    template <typename ValueType>
    MatrixVectorMultiplyNode<ValueType>::MatrixVectorMultiplyNode()
        : CompilableNode({ &_inputMatrix, &_inputVector }, { &_output }), _inputMatrix(this, {}, inputMatrixPortName), _inputVector(this, {}, inputVectorPortName), _output(this, outputPortName, 0), _m(0), _n(0), _lda(0), _incx(0)
    {
    }

    template <typename ValueType>
    MatrixVectorMultiplyNode<ValueType>::MatrixVectorMultiplyNode(const model::PortElements<ValueType>& inputMatrix, size_t m, size_t n, size_t matrixStride, const model::PortElements<ValueType>& inputVector)
        : CompilableNode({ &_inputMatrix, &_inputVector }, { &_output }), _inputMatrix(this, inputMatrix, inputMatrixPortName), _inputVector(this, inputVector, inputVectorPortName), _output(this, outputPortName, m), _m(m), _n(n), _lda(matrixStride), _incx(1)
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

        math::RowMatrixReference<ValueType> inputMatrixRef(_m, _n, inputMatrixValues.data());
        math::ColumnVectorReference<ValueType> inputVectorRef(inputVectorValues.data(), _n);
        math::ColumnVectorReference<ValueType> outputVectorRef(outputVectorValues.data(), _m);

        math::Operations::Multiply(static_cast<ValueType>(1.0), inputMatrixRef, inputVectorRef, static_cast<ValueType>(0.0), outputVectorRef);

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

        if (compiler.GetMapCompilerParameters().compilerSettings.useBlas)
        {
            EmitMatrixVectorMultiplyBlas<ValueType>(function, (int)_m, (int)_n, pInputMatrix, (int)_lda, pInputVector, _incx, pOutput, 1);
        }
        else
        {
            EmitMatrixVectorMultiplySlow<ValueType>(function, (int)_m, (int)_n, pInputMatrix, (int)_lda, pInputVector, _incx, pOutput, 1);
        }
    }

    template <typename ValueType>
    void MatrixVectorMultiplyNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputMatrixPortName] << _inputMatrix;
        archiver[inputVectorPortName] << _inputVector;
        archiver[outputPortName] << _output;
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
        archiver[outputPortName] >> _output;
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
