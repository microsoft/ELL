////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SinkNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "SteppableMap.h"

// emitters
#include "IRMetadata.h"

// stl
#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the SinkNode calls to deliver data to user code. </summary>
    template <typename ValueType>
    using SinkFunction = std::function<void(const std::vector<ValueType>&)>;

    template <typename ValueType>
    class SinkNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor </summary>
        SinkNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> Port elements for input values </param>
        /// <param name="sink"> The sink function that will receive output values </param>
        /// <param name="sinkFunctionName"> The optional sink function name to be emitted </param>
        SinkNode(const model::PortElements<ValueType>& input, SinkFunction<ValueType> sink, const std::string& sinkFunctionName = "SinkNode_OutputFunction");

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SinkNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` receiving the copy  </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        // Evaluates whether the input meets the filter criteria,
        // and should be forwarded to the sink function.
        virtual bool EvaluateInput() const;

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void SetOutputValuesLoop(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void SetOutputValuesExpanded(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

    private:
        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;

        std::string _sinkFunctionName;
        SinkFunction<ValueType> _sink;
    };
}
}

#include "../tcc/SinkNode.tcc"