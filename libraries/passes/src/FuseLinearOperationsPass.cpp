////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FuseLinearOperationsPass.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FuseLinearOperationsPass.h"

// model
#include "ModelTransformer.h"
#include "OptimizationPassRegistry.h"

// nodes
#include "BroadcastFunctionNode.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace passes
{
    //
    // Implementation
    //
    namespace
    {
        //
        // Data structures
        //
        template <typename ValueType>
        struct LinearCoefficients
        {
            std::vector<ValueType> scale;
            std::vector<ValueType> bias;
        };

        template <typename ValueType>
        struct LinearCoeffNodes
        {
            const nodes::ConstantNode<ValueType>* scaleNode;
            const nodes::ConstantNode<ValueType>* biasNode;
        };

        //
        // Functions
        //
        template <typename ValueType>
        bool HasSimpleConstantSecondaryInputs(const nodes::BroadcastLinearFunctionNode<ValueType>& node)
        {
            // First verify our inputs are compatible
            int scaleInputSize = node.secondaryInput1.Size();
            int biasInputSize = node.secondaryInput2.Size();

            if (scaleInputSize > 0 && biasInputSize > 0 && scaleInputSize != biasInputSize)
            {
                return false; // sizes incompatible
            }

            const auto& scaleElements = node.secondaryInput1.GetPortElements();
            const auto& biasElements = node.secondaryInput2.GetPortElements();

            if (!scaleElements.IsFullPortOutput() || !biasElements.IsFullPortOutput())
            {
                return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
            }

            const nodes::ConstantNode<ValueType>* scaleInputNode = scaleInputSize == 0 ? nullptr : dynamic_cast<const nodes::ConstantNode<ValueType>*>(scaleElements.GetElement(0).ReferencedPort()->GetNode());
            const nodes::ConstantNode<ValueType>* biasInputNode = biasInputSize == 0 ? nullptr : dynamic_cast<const nodes::ConstantNode<ValueType>*>(biasElements.GetElement(0).ReferencedPort()->GetNode());

            if (scaleInputNode == nullptr && biasInputNode == nullptr)
            {
                return false; // need at least one secondary input
            }

            return true;
        }

        template <typename ValueType>
        bool CanCombineWithPrimaryInput(const nodes::BroadcastLinearFunctionNode<ValueType>& node)
        {
            // First verify our inputs are constant nodes
            if (!HasSimpleConstantSecondaryInputs(node))
            {
                return false;
            }

            const auto& primaryElements = node.primaryInput.GetPortElements();
            if (!primaryElements.IsFullPortOutput())
            {
                return false; // we require all inputs to a port to come from the same place (though we could relax this requirement in the future, perhaps)
            }

            const nodes::BroadcastLinearFunctionNode<ValueType>* primaryInputNode = dynamic_cast<const nodes::BroadcastLinearFunctionNode<ValueType>*>(primaryElements.GetElement(0).ReferencedPort()->GetNode());
            if (primaryInputNode == nullptr)
            {
                return false; // primary input must be another linear function
            }

            // Our secondary inputs are OK and the primary input comes from a single lineary function node, now check that its
            // secondary inputs are simple
            if (!HasSimpleConstantSecondaryInputs(*primaryInputNode))
            {
                return false;
            }

            // Check that the Shapes are compatible
            if (!PortMemoryLayoutsEqual(node.GetInputLayout(), primaryInputNode->GetInputLayout()))
            {
                return false;
            }

            if (!PortMemoryLayoutsEqual(node.GetOutputLayout(), primaryInputNode->GetOutputLayout()))
            {
                return false;
            }

            return true;
        }

        template <typename ValueType>
        LinearCoeffNodes<ValueType> GetConstantSecondaryInputNodes(const nodes::BroadcastLinearFunctionNode<ValueType>& node)
        {
            const auto& scaleElements = node.secondaryInput1.GetPortElements();
            const auto& biasElements = node.secondaryInput2.GetPortElements();

            if ((scaleElements.Size() != 0 && !scaleElements.IsFullPortOutput()) || (biasElements.Size() != 0 && !biasElements.IsFullPortOutput()))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Combined linear function coefficients must be full port output");
            }

            int scaleInputSize = scaleElements.Size();
            int biasInputSize = biasElements.Size();
            if (scaleInputSize > 0 && biasInputSize > 0 && scaleInputSize != biasInputSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Combined linear function coefficients must have same size");
            }

            const nodes::ConstantNode<ValueType>* scaleInputNode = scaleInputSize == 0 ? nullptr : dynamic_cast<const nodes::ConstantNode<ValueType>*>(scaleElements.GetElement(0).ReferencedPort()->GetNode());
            const nodes::ConstantNode<ValueType>* biasInputNode = biasInputSize == 0 ? nullptr : dynamic_cast<const nodes::ConstantNode<ValueType>*>(biasElements.GetElement(0).ReferencedPort()->GetNode());

            return { scaleInputNode, biasInputNode };
        }

        template <typename ValueType>
        LinearCoefficients<ValueType> GetCombinedLinearCoeffs(const nodes::BroadcastLinearFunctionNode<ValueType>& node1, const nodes::BroadcastLinearFunctionNode<ValueType>& node2)
        {
            LinearCoefficients<ValueType> coefficients;
            auto& scale = coefficients.scale;
            auto& bias = coefficients.bias;

            // Here, we have two linear functions, f1(x) = s1*x + b1; f2(x) = s2*x + b2
            // and we want to find their composition f' = s'*x + b' = f2(f1(x)) = (f2 * f1)(x) = s2*(s1*x + b1) + b2 = s1*s2*x + (b1*s2 + b2)
            // (Where `node1` is the node computing f1, and `node2` is the node representing f2)

            auto prevSecondaryInputs = GetConstantSecondaryInputNodes(node1);
            auto thisSecondaryInputs = GetConstantSecondaryInputNodes(node2);

            // Compute the combined scale, s' = s1*s2
            if (prevSecondaryInputs.scaleNode == nullptr && thisSecondaryInputs.scaleNode == nullptr) // s1 == 1, s2 == 1, so s' = 1
            {
                scale.resize(0); // signal there's no scale (scale = 1)
            }
            else if (prevSecondaryInputs.scaleNode == nullptr) // s1 == 1, so s' = s2
            {
                scale = thisSecondaryInputs.scaleNode->GetValues(); // s2
            }
            else if (thisSecondaryInputs.scaleNode == nullptr) // s2 == 1, so s' = s1,
            {
                scale = prevSecondaryInputs.scaleNode->GetValues(); // s1
            }
            else // s' = s1*s2*x
            {
                scale = prevSecondaryInputs.scaleNode->GetValues(); // scale = s1
                const auto& s2 = thisSecondaryInputs.scaleNode->GetValues();
                assert(s2.size() == scale.size());
                for (size_t index = 0; index < scale.size(); ++index)
                {
                    scale[index] *= s2[index];
                }
            }

            // Now compute the combined bias, b' = (b1*s2) + b2
            if (prevSecondaryInputs.biasNode == nullptr && thisSecondaryInputs.biasNode == nullptr) // b1 == 0, b2 == 0, so b' == 0
            {
                bias.resize(0); // signal there's no bias (bias = 0)
            }
            else if (prevSecondaryInputs.biasNode == nullptr) // b1 == 0, so b' = b2
            {
                bias = thisSecondaryInputs.biasNode->GetValues(); // b2
            }
            else // b' = (b1*s2) + b1 (but s2 may be 1, and b1 may be zero)
            {
                bias = prevSecondaryInputs.biasNode->GetValues(); // bias == b1
                if (thisSecondaryInputs.scaleNode != nullptr) // if s2 is present, set bias = bias*s2
                {
                    const auto& s2 = thisSecondaryInputs.scaleNode->GetValues();
                    assert(s2.size() == bias.size());
                    for (size_t index = 0; index < bias.size(); ++index)
                    {
                        bias[index] *= s2[index];
                    }
                }

                if (thisSecondaryInputs.biasNode != nullptr) // b2 == 0, so b' = b1*s2, but perhaps s2 == 1
                {
                    const auto& b2 = thisSecondaryInputs.biasNode->GetValues(); // now add b2
                    for (size_t index = 0; index < bias.size(); ++index)
                    {
                        bias[index] += b2[index];
                    }
                }
            }

            return coefficients;
        }

        // returns 'true' if we handled the situation, else 'false'. If we return 'false', keep trying other ValueTypes
        template <typename ValueType>
        bool TryCombineLinearFunctionNodes(const model::Node& node, model::ModelTransformer& transformer)
        {
            auto thisNode = dynamic_cast<const nodes::BroadcastLinearFunctionNode<ValueType>*>(&node);
            if (thisNode == nullptr)
            {
                return false;
            }

            if(!CanCombineWithPrimaryInput(*thisNode))
            {
                return false;
            }

            // These are the elements in the new model that correspond to our inputs -- that is, the outputs
            // of the refined version of the linear function node attached to our primaryInput
            auto primaryInputElements = transformer.TransformPortElements(thisNode->primaryInput.GetPortElements());
            auto prevNode = dynamic_cast<const nodes::BroadcastLinearFunctionNode<ValueType>*>(primaryInputElements.GetElement(0).ReferencedPort()->GetNode());
            if (prevNode == nullptr)
            {
                node.Copy(transformer);
                return true;
            }

            auto newCoeffs = GetCombinedLinearCoeffs(*thisNode, *prevNode);
            auto prevPrimaryInputElements = prevNode->primaryInput.GetPortElements();
            auto scaleValuesNode = transformer.AddNode<nodes::ConstantNode<ValueType>>(newCoeffs.scale);
            auto biasValuesNode = transformer.AddNode<nodes::ConstantNode<ValueType>>(newCoeffs.bias);
            auto newNode = transformer.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(prevPrimaryInputElements,
                                                                                              thisNode->GetInputLayout(),
                                                                                              scaleValuesNode->output,
                                                                                              biasValuesNode->output,
                                                                                              thisNode->GetBroadcastDimension(),
                                                                                              thisNode->GetOutputLayout());
            transformer.MapNodeOutput(thisNode->output, newNode->output);
            return true;
        }

        void CombineLinearFunctionNodes(const model::Node& node, model::ModelTransformer& transformer)
        {
            if (TryCombineLinearFunctionNodes<float>(node, transformer))
            {
                return;
            }
            if (TryCombineLinearFunctionNodes<double>(node, transformer))
            {
                return;
            }
            node.Copy(transformer);
        }
    }

    //
    // FuseLinearOperationsPass methods
    //
    void FuseLinearOperationsPass::OptimizeNode(const model::Node& node, model::ModelTransformer& transformer)
    {
        CombineLinearFunctionNodes(node, transformer);
    }

    void FuseLinearOperationsPass::AddToRegistry()
    {
        model::OptimizationPassInfo info = {
            "FuseLinearOperationsPass",
            [](const model::ModelOptimizerOptions& settings) { return settings.fuseLinearFunctionNodes; },
            []() { return std::make_unique<FuseLinearOperationsPass>(); }
        };
        model::OptimizationPassRegistry::AddPass(info);
    }
}
}
