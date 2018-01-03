////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixMatrixMultiplyNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "IRFunctionEmitter.h"

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that multiplies two matrices. </summary>
    template <typename ValueType>
    class MatrixMatrixMultiplyNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MatrixMatrixMultiplyNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        MatrixMatrixMultiplyNode(const model::PortElements<ValueType>& input1, size_t m, size_t n, size_t k, size_t matrix1Stride, const model::PortElements<ValueType>& input2, size_t matrix2Stride, size_t outputMatrixStride);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the matrix multiplication, a row-major matrix of size m x k.  </param>
        /// <param name="input2"> The right-hand input of the matrix multiplication, a row-major matrix of size k x n. </param>
        /// <param name="transpose1"> If true, transpose the left-hand input matrix. </param>
        /// <param name="transpose2"> If true, transpose the right-hand input matrix. </param>
        MatrixMatrixMultiplyNode(const model::PortElements<ValueType>& input1, size_t m, size_t n, size_t k, size_t matrix1Stride, bool transpose1, const model::PortElements<ValueType>& input2, size_t matrix2Stride, bool transpose2, size_t outputMatrixStride);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MatrixMatrixMultiplyNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state:  m, n, k, lda, ldb, ldc, transpose

    private:
        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;

        // Matrix dimensions
        // Matrix 1 is MxK, Matrix 2 is KxN, Output is MxN
        size_t _m, _n, _k;
        size_t _lda, _ldb, _ldc;
        bool _transpose1, _transpose2;
    };
}
}
