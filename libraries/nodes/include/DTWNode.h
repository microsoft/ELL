////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DTWNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

// model
#include "BinaryOperationNode.h"
#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "Exception.h"
#include "TypeName.h"

// stl
#include <string>

namespace emll
{
namespace nodes
{
    /// <summary> A node that performs DTW between its inputs </summary>
    template <typename ValueType>
    class DTWNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DTWNode();

        /// <summary> Constructor </summary>
        /// <param name="input1"> One of the signals to take the dot product of </param>
        /// <param name="input2"> The other signal to take the dot product of </param>
        DTWNode(const model::PortElements<ValueType>& input, const std::vector<std::vector<ValueType>>& prototype);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DTWNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently copying the model </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` currently refining the model </param>
        // virtual bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        void Reset() const;

        // private:
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

#include "../tcc/DTWNode.tcc"