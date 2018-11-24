////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LSTMNode.h (nodes)
//  Authors:  James Devine, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "LLVMUtilities.h"

// model
#include "Activation.h"
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"
#include "RNNNode.h"

// math
#include "Matrix.h"
#include "Vector.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    ///<summary> LSTMnode implements a Long Short Term Memory network.  See http://colah.github.io/posts/2015-08-Understanding-LSTMs/. </summary>
    template <typename ValueType>
    class LSTMNode : public RNNNode<ValueType>
    {
    public:
        using ActivationType = predictors::neural::Activation<ValueType>;

        /// <summary> Default contructor. </summary>
        LSTMNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="resetTrigger"> Port elements for the reset trigger. </param>
        /// <param name="hiddenUnits"> The number of hidden units. </param>
        /// <param name="inputWeights"> The weights to be applied to the input, must be a stack of 4 (input, forget, candidate, output). </param>
        /// <param name="hiddenWeights"> The weights to be applied to the hidden state, must be a stack of 4 (input, forget, candidate, output) </param>
        /// <param name="inputBias"> The bias to be applied to the input, must be a stack of 4 (input, forget, candidate, output) </param>
        /// <param name="hiddenBias"> The bias to be applied to the hidden state, must be a stack of 4 (input, forget, candidate, output) </param>
        /// <param name="activation"> The activation function. </param>
        /// <param name="recurrentActivation"> The recurrent activation function. </param>
        /// <param name="validateWeights"> Whether to check the size of the weights. </param>
        LSTMNode(const model::OutputPort<ValueType>& input,
                 const model::OutputPort<int>& resetTrigger,
                 size_t hiddenUnits,
                 const model::OutputPort<ValueType>& inputWeights,
                 const model::OutputPort<ValueType>& hiddenWeights,
                 const model::OutputPort<ValueType>& inputBias,
                 const model::OutputPort<ValueType>& hiddenBias,
                 const ActivationType& activation,
                 const ActivationType& recurrentActivation,
                 bool validateWeights = true);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("LSTMNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Resets any state on the node, if any </summary>
        void Reset() override;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return true; }

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        void Copy(model::ModelTransformer& transformer) const override;

        ActivationType _recurrentActivation;

    private:
        using VectorType = math::ColumnVector<ValueType>;

        // Additional Hidden state for compute
        mutable VectorType _cellState;
    };
} // namespace nodes
} // namespace ell
