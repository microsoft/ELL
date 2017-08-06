////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryFunctionNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "MapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// emitters
#include "EmitterTypes.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"

// stl
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a coordinatewise binary operation on its inputs ('binary' in the sense that there are 2 inputs). </summary>
    template <typename ValueType, typename FunctionType>
    class BinaryFunctionNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* input1PortName = "input1";
        static constexpr const char* input2PortName = "input2";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        BinaryFunctionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input1"> The left-hand input of the function. </param>
        /// <param name="input2"> The right-hand input of the function. </param>
        /// <param name="inputLayout"> The layout for both inputs. </param>
        /// <param name="outputLayout"> The output layout. </param>
        /// <param name="function"> The function to apply coordinate-wise. </param>
        /// <param name="padding"> The padding value. </param>
        BinaryFunctionNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2,
            const PortMemoryLayout& inputLayout, const PortMemoryLayout& outputLayout, FunctionType function, ValueType padding = 0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, FunctionType>("BinaryFunctionNode"); }

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
        virtual bool HasState() const override { return true; } // stored state: paddingValue

    private:
        void ComputeDimensionLoop(size_t dimension,
            std::vector<ValueType>& output,
            size_t prevInputDimensionOffset,
            size_t prevOutputDimensionOffset) const;

        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
            emitters::IRFunctionEmitter& function,
            size_t dimension,
            llvm::Value* input1,
            llvm::Value* input2,
            llvm::Value* output,
            llvm::Value* prevInputDimensionOffset,
            llvm::Value* prevOutputDimensionOffset) const;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;
        PortMemoryLayout _inputLayout;

        // Output
        model::OutputPort<ValueType> _output;
        PortMemoryLayout _outputLayout;

        // Function to apply coordinate-wise
        FunctionType _function;

        ValueType _paddingValue;
    };
}
}

#include "../tcc/BinaryFunctionNode.tcc"