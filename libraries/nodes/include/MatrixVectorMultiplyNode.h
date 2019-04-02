////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorMultiplyNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <emitters/include/IRFunctionEmitter.h>

#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that multiplies a matrix with a vector. </summary>
    template <typename ValueType>
    class MatrixVectorMultiplyNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputMatrixPortName = "inputMatrix";
        static constexpr const char* inputVectorPortName = "inputVector";
        const model::InputPort<ValueType>& inputMatrix = _inputMatrix;
        const model::InputPort<ValueType>& inputVector = _inputVector;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MatrixVectorMultiplyNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="inputMatrix"> The left-hand input of the matrix multiplication. </param>
        /// <param name="m"> The number of rows in the matrix. </param>
        /// <param name="n"> The number of columns in the matrix. </param>
        /// <param name="matrixStride"> The stride of the matrix (the number of elements between adjacent rows). </param>
        /// <param name="inputVector"> The right-hand input of the matrix multiplication. </param>
        MatrixVectorMultiplyNode(const model::OutputPort<ValueType>& inputMatrix, size_t m, size_t n, size_t matrixStride, const model::OutputPort<ValueType>& inputVector);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MatrixVectorMultiplyNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: m, n, lda, incx

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _inputMatrix;
        model::InputPort<ValueType> _inputVector;

        // Output
        model::OutputPort<ValueType> _output;

        // Matrix dimensions
        // Matrix is MxN, vector is of length N
        size_t _m, _n;
        size_t _lda, _incx;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="inputMatrix"> The left-hand input of the matrix multiplication. </param>
    /// <param name="m"> The number of rows in the matrix. </param>
    /// <param name="n"> The number of columns in the matrix. </param>
    /// <param name="matrixStride"> The stride of the matrix (the number of elements between adjacent rows). </param>
    /// <param name="inputVector"> The right-hand input of the matrix multiplication. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const model::OutputPort<ValueType>& MatrixVectorMultiply(const model::OutputPort<ValueType>& inputMatrix, size_t m, size_t n, size_t matrixStride, const model::OutputPort<ValueType>& inputVector);
} // namespace nodes
} // namespace ell
