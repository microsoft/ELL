////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DTWDistanceNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

// model
#include "BinaryOperationNode.h"
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
#include "TypeName.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that computes the dynamic time-warping distance between its inputs </summary>
    template <typename ValueType>
    class DTWDistanceNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DTWDistanceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signals to compare to the prototype </param>
        /// <param name="prototype"> The prototype </param>
        DTWDistanceNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DTWDistanceNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently copying the model </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently refining the model </param>
        // virtual bool Refine(model::ModelTransformer& transformer) const override;

        std::vector<std::vector<ValueType>> GetPrototype() const { return _prototype; }

    protected:
        void Reset() const;
        virtual void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual bool HasState() const override { return true; }
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        std::vector<ValueType> GetPrototypeData() const;

        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;

        size_t _sampleDimension;
        size_t _prototypeLength;
        std::vector<std::vector<ValueType>> _prototype;
        // double _threshold;
        double _prototypeVariance;

        mutable std::vector<ValueType> _d;
        mutable std::vector<int> _s;
        mutable int _currentTime;
    };
}
}

#include "../tcc/DTWDistanceNode.tcc"