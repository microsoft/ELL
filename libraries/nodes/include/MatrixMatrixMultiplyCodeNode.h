////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixMatrixMultiplyCodeNode.h (nodes)
//  Authors:  Mason Remy, Denny Sun
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Current gaps:
// Doesn't support transposed matrices
// Doesn't support alpha and beta values

#pragma once

#include <model/include/CompilableCodeNode.h>
#include <model/include/InputPort.h>
#include <model/include/OutputPort.h>

#include <nodes/include/MatrixMatrixMultiplyImplementation.h>

#include <utilities/include/ArchiveVersion.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <value/include/CachingStrategies.h>
#include <value/include/EmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LoopNests.h>
#include <value/include/Matrix.h>
#include <value/include/Scalar.h>
#include <value/include/ScalarOperations.h>
#include <value/include/loopnests/CodeGenerator.h>
#include <value/include/loopnests/Kernel.h>
#include <value/include/loopnests/LoopNest.h>

#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that multiplies two matrices. </summary>
    template <typename ValueType>
    class MatrixMatrixMultiplyCodeNode : public model::CompilableCodeNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MatrixMatrixMultiplyCodeNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="panelM"> The panel size to use in the M dimension (rows of A, C) </param>
        /// <param name="panelN"> The panel size to use in the N dimension (columns of B, C) </param>
        /// <param name="panelK"> The panel size to use in the K dimension (columns of A, rows of B) </param>
        /// <param name="kernelM"> The kernel size to use in the M dimension (rows of A, C). </param>
        /// <param name="kernelN"> The kernel size to use in the N dimension (columns of B, C). </param>
        /// <param name="kernelK"> The kernel size to use in the K dimension (columns of A, rows of B). </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="outputMemoryLayout"> The output memory layout to use. </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, const model::PortMemoryLayout& outputMemoryLayout, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="outputMemoryLayout"> The output memory layout to use. </param>
        /// <param name="panelM"> The panel size to use in the M dimension (rows of A, C) </param>
        /// <param name="panelN"> The panel size to use in the N dimension (columns of B, C) </param>
        /// <param name="panelK"> The panel size to use in the K dimension (columns of A, rows of B) </param>
        /// <param name="kernelM"> The kernel size to use in the M dimension (rows of A, C). </param>
        /// <param name="kernelN"> The kernel size to use in the N dimension (columns of B, C). </param>
        /// <param name="kernelK"> The kernel size to use in the K dimension (columns of A, rows of B). </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2, const model::PortMemoryLayout& outputMemoryLayout, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, const model::OutputPort<ValueType>& input2, int matrix2Stride, int outputMatrixStride, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="panelM"> The panel size to use in the M dimension (rows of A, C) </param>
        /// <param name="panelN"> The panel size to use in the N dimension (columns of B, C) </param>
        /// <param name="panelK"> The panel size to use in the K dimension (columns of A, rows of B) </param>
        /// <param name="kernelM"> The kernel size to use in the M dimension (rows of A, C). </param>
        /// <param name="kernelN"> The kernel size to use in the N dimension (columns of B, C). </param>
        /// <param name="kernelK"> The kernel size to use in the K dimension (columns of A, rows of B). </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, const model::OutputPort<ValueType>& input2, int matrix2Stride, int outputMatrixStride, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="transpose1"> If true, transpose the left-hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="transpose2"> If true, transpose the right-hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, bool transpose1, const model::OutputPort<ValueType>& input2, int matrix2Stride, bool transpose2, int outputMatrixStride, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="transpose1"> If true, transpose the left-hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="transpose2"> If true, transpose the right-hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="panelM"> The panel size to use in the M dimension (rows of A, C) </param>
        /// <param name="panelN"> The panel size to use in the N dimension (columns of B, C) </param>
        /// <param name="panelK"> The panel size to use in the K dimension (columns of A, rows of B) </param>
        /// <param name="kernelM"> The kernel size to use in the M dimension (rows of A, C). </param>
        /// <param name="kernelN"> The kernel size to use in the N dimension (columns of B, C). </param>
        /// <param name="kernelK"> The kernel size to use in the K dimension (columns of A, rows of B). </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, bool transpose1, const model::OutputPort<ValueType>& input2, int matrix2Stride, bool transpose2, int outputMatrixStride, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="transpose1"> If true, transpose the left-hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="transpose2"> If true, transpose the right-hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="transposeOutput"> If true, transpose the output matrix. </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, bool transpose1, const model::OutputPort<ValueType>& input2, int matrix2Stride, bool transpose2, int outputMatrixStride, bool transposeOutput, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="m"> The number of rows in the left hand input matrix and in the output matrix. </param>
        /// <param name="n"> The number of columns in the right hand input matrix and in the output matrix. </param>
        /// <param name="k"> The number of columns in the left hand input matrix and the number of columns in the right hand input matrix. </param>
        /// <param name="matrix1Stride"> The number of elements between successive elements in a single column in the left hand input matrix. </param>
        /// <param name="transpose1"> If true, transpose the left-hand input matrix. </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="matrix2Stride"> The number of elements between successive elements in a single column in the right hand input matrix. </param>
        /// <param name="transpose2"> If true, transpose the right-hand input matrix. </param>
        /// <param name="outputMatrixStride"> The number of elements between successive elements in a single column in the output matrix. </param>
        /// <param name="transposeOutput"> If true, transpose the output matrix. </param>
        /// <param name="panelM"> The panel size to use in the M dimension (rows of A, C) </param>
        /// <param name="panelN"> The panel size to use in the N dimension (columns of B, C) </param>
        /// <param name="panelK"> The panel size to use in the K dimension (columns of A, rows of B) </param>
        /// <param name="kernelM"> The kernel size to use in the M dimension (rows of A, C). </param>
        /// <param name="kernelN"> The kernel size to use in the N dimension (columns of B, C). </param>
        /// <param name="kernelK"> The kernel size to use in the K dimension (columns of A, rows of B). </param>
        /// <param name="gemmImpl"> Which implementation of matrix-matrix multiplication to use </param>
        MatrixMatrixMultiplyCodeNode(const model::OutputPort<ValueType>& input1, int m, int n, int k, int matrix1Stride, bool transpose1, const model::OutputPort<ValueType>& input2, int matrix2Stride, bool transpose2, int outputMatrixStride, bool transposeOutput, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, const MatrixMatrixMultiplyImplementation& gemmImpl = MatrixMatrixMultiplyImplementation::DEFAULT);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MatrixMatrixMultiplyCodeNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Define(value::FunctionDeclaration& fn) override;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state:  m, n, k, lda, ldb, ldc, transpose

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void ZeroMatrix(value::Matrix matrix) const;

        void ForLoopGEMM(const value::Matrix matA, const value::Matrix matB, value::Matrix matC);
        void Gemm(const value::Matrix mat, const value::Matrix matB, value::Matrix matC);
        void GemmFn(const value::Matrix mat, const value::Matrix matB, value::Matrix matC, int thread_num = 0);
        void ParallelizeGemmCol(const value::Matrix matA, const value::Matrix matB, value::Matrix matC, int numThreads = 2);
        void ParallelizeGemmRow(const value::Matrix matA, const value::Matrix matB, value::Matrix matC, int numThreads = 2);
        void ELLCodeGEMM(const value::Matrix matA, const value::Matrix matB, value::Matrix matC);

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;

        // Matrix dimensions
        // Matrix 1 is MxK, Matrix 2 is KxN, Output is MxN
        int _m = 0, _n = 0, _k = 0;
        int _lda = 0, _ldb = 0, _ldc = 0;
        bool _transpose1 = false, _transpose2 = false, _transposeOutput = false;

        // Implementation-controlling members
        int _panelM;
        int _panelN;
        int _panelK;
        int _kernelM;
        int _kernelN;
        int _kernelK;
        MatrixMatrixMultiplyImplementation _impl;

        static const int _defaultPanelM = 64;
        static const int _defaultPanelN = 64;
        static const int _defaultPanelK = 64;
        static const int _defaultKernelM = 4;
        static const int _defaultKernelN = 4;
        static const int _defaultKernelK = 4;
    };

    //
    // Explicit instantiation declarations
    //
    extern template class MatrixMatrixMultiplyCodeNode<float>;
    extern template class MatrixMatrixMultiplyCodeNode<double>;
} // namespace nodes
} // namespace ell
