////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorMultiplyNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// emitters
#include "IRFunctionEmitter.h"

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
    /// <summary> A node that multiplies a matrix with a vector. </summary>
    template <typename ValueType>
    class MatrixVectorMultiplyNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputMatrixPortName = "inputMatrix";
        static constexpr const char* inputVectorPortName = "inputVector";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& inputMatrix = _inputMatrix;
        const model::InputPort<ValueType>& inputVector = _inputVector;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MatrixVectorMultiplyNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="inputMatrix"> The left-hand input of the matrix multiplication. </param>
        /// <param name="inputVector"> The right-hand input of the matrix multiplication. </param>
        MatrixVectorMultiplyNode(const model::PortElements<ValueType>& inputMatrix, size_t m, size_t n, size_t matrixStride, const model::PortElements<ValueType>& inputVector);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("MatrixVectorMultiplyNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
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
}
}
