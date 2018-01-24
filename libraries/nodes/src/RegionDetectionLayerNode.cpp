///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegionDetectionLayer.cpp (neural)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#include "RegionDetectionLayerNode.h"
#include "ActivationLayerNode.h"
#include "SigmoidActivation.h"
#include "CompiledActivationFunctions.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    RegionDetectionLayerNode<ValueType>::RegionDetectionLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::RegionDetectionLayer<ValueType>& layer)
        : BaseType(input, layer)
    {
    }

    template <typename ValueType>
    bool RegionDetectionLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->_input.GetPortElements());

        const auto& detectionParams = this->_layer.GetDetectionParameters();

        auto detectionNode = transformer.AddNode<RegionDetectionNode<ValueType>>(newInput,
                                                                                 detectionParams,
                                                                                 this->GetInputMemoryLayout(),
                                                                                 this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, detectionNode->output);

        return false;
    }

    //
    // RegionDetectionNode
    //
    template <typename ValueType>
    RegionDetectionNode<ValueType>::RegionDetectionNode()
        : RegionDetectionNode({}, {}, {}, {})
    {}

    template <typename ValueType>
    RegionDetectionNode<ValueType>::RegionDetectionNode(const model::PortElements<ValueType>& input,
                                                        predictors::neural::RegionDetectionParameters params,
                                                        const model::PortMemoryLayout& inputMemoryLayout,
                                                        const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input }, { &_output }),
          _input(this, input, defaultInputPortName),
          _params(std::move(params)),
          _output(this, defaultOutputPortName, input.Size()),
          _inputMemoryLayout(inputMemoryLayout),
          _outputMemoryLayout(outputMemoryLayout)
    {}

    template <typename ValueType>
    void RegionDetectionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<RegionDetectionNode>(newInput, _params, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void RegionDetectionNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto add = emitters::TypedOperator::add;
        const auto mul = emitters::TypedOperator::multiply;

        auto expFunc = function.GetModule().GetRuntime().GetExpFunction<ValueType>();
        SigmoidActivationFunction<ValueType> sigmoidActivation;

        llvm::Value* input = compiler.EnsurePortEmitted(this->input);
        llvm::Value* width = function.Literal(_params.width);
        llvm::Value* height = function.Literal(_params.height);
        llvm::Value* numBoxes = function.Literal(_params.numBoxesPerCell);
        llvm::Value* numClasses = function.Literal(_params.numClasses);
        llvm::Value* numCoords = function.Literal(_params.numCoordinates);

        llvm::Value* output = compiler.EnsurePortEmitted(this->output);

        const auto zero = function.Literal(0);
        const auto one = function.Literal(1);

        auto J = height;
        // the stride between each box in a cell (numCoordinates + 1 + numClasses)
        auto boxStride = function.Operator(add, numCoords, function.Operator(add, one, numClasses));
        // the stride of each cell (numBoxesPerCell * boxStride)
        auto K = function.Operator(mul, numBoxes, boxStride);
        function.For(width, [input, output, zero, one, expFunc, sigmoidActivation, boxStride, numClasses, J, K, height, numBoxes, add, mul](auto& fn, auto i)
        {
            fn.For(height, [input, output, zero, one, expFunc, sigmoidActivation, boxStride, numClasses, i, J, K, numBoxes, add, mul](auto& fn, auto j)
            {
                // (i*J + j)*K
                auto sliceIndex = fn.Operator(mul,
                                    fn.Operator(add,
                                        fn.Operator(mul, i, J),
                                    j),
                                  K);
                fn.For(numBoxes, [sliceIndex, input, output, zero, one, expFunc, sigmoidActivation, boxStride, numClasses, add, mul](auto& fn, auto k)
                {
                    const auto addFloat = emitters::TypedOperator::addFloat;
                    const auto subFloat = emitters::TypedOperator::subtractFloat;
                    const auto divFloat = emitters::TypedOperator::divideFloat;

                    // sliceIndex + k*boxStride
                    auto boxOffset = fn.Operator(add, sliceIndex, fn.Operator(mul, k, boxStride));

                    // output[boxOffset + 0] = sigmoid(input[boxOffset + 0])
                    auto elementOffset = fn.Operator(add, boxOffset, zero);
                    fn.SetValueAt(output, elementOffset, sigmoidActivation.Compile(fn, fn.ValueAt(input, elementOffset)));

                    // output[boxOffset + 1] = sigmoid(input[boxOffset + 1])
                    elementOffset = fn.Operator(add, elementOffset, one);
                    fn.SetValueAt(output, elementOffset, sigmoidActivation.Compile(fn, fn.ValueAt(input, elementOffset)));

                    // output[boxOffset + 2] = exp(input[boxOffset + 2])
                    elementOffset = fn.Operator(add, elementOffset, one);
                    fn.SetValueAt(output, elementOffset, fn.Call(expFunc, { fn.ValueAt(input, elementOffset) }));

                    // output[boxOffset + 3] = exp(input[boxOffset + 3])
                    elementOffset = fn.Operator(add, elementOffset, one);
                    fn.SetValueAt(output, elementOffset, fn.Call(expFunc, { fn.ValueAt(input, elementOffset) }));

                    // output[boxOffset + 4] = sigmoid(input[boxOffset + 4])
                    elementOffset = fn.Operator(add, elementOffset, one);
                    fn.SetValueAt(output, elementOffset, sigmoidActivation.Compile(fn, fn.ValueAt(input, elementOffset)));

                    // output[boxOffset + 5 : boxOffset + 5 + numClasses] = softmax(input[boxOffset + 5 : boxOffset + 5 + numClasses])
                    elementOffset = fn.Operator(add, elementOffset, one);

                    // set the initial max value to the first element
                    auto classProbMax = fn.Variable(emitters::GetVariableType<ValueType>());
                    fn.Store(classProbMax, fn.ValueAt(input, elementOffset));

                    // find the max value in the rest of the probabilities
                    fn.For(fn.Operator(add, elementOffset, one), fn.Operator(add, elementOffset, numClasses),
                           [classProbMax, input](auto& fn, auto c)
                    {
                        auto val = fn.ValueAt(input, c);
                        auto test = fn.Comparison(emitters::TypedComparison::greaterThanFloat, fn.Load(classProbMax), val);
                        fn.If(test, [classProbMax, val](auto& fn)
                        {
                            // if val > classProbMax: classProbMax = val
                            fn.Store(classProbMax, val);
                        });
                    });

                    // Calculate the sum of the all the euler values, which are calculated using the max above
                    auto sum = fn.Variable(emitters::GetVariableType<ValueType>());
                    fn.Store(sum, fn.Literal(ValueType{ 0 }));
                    fn.For(elementOffset, fn.Operator(add, elementOffset, numClasses),
                        [sum, classProbMax, input, output, expFunc, subFloat, addFloat](auto& fn, auto c)
                    {
                        // inputVal = input[c]
                        auto inputVal = fn.ValueAt(input, c);
                        // eulerVal = exp(inputVal - classProbMax)
                        auto eulerVal = fn.Call(expFunc, { fn.Operator(subFloat, inputVal, fn.Load(classProbMax)) });
                        // sum += eulerVal
                        fn.Store(sum, fn.Operator(addFloat, fn.Load(sum), eulerVal));
                        // output[c] = eulerVal
                        fn.SetValueAt(output, c, eulerVal);
                    });

                    // Divide each element by the sum
                    fn.For(elementOffset, fn.Operator(add, elementOffset, numClasses),
                        [sum, output, divFloat](auto& fn, auto c)
                    {
                        // ouput[c] /= sum
                        fn.SetValueAt(output, c, fn.Operator(divFloat, fn.ValueAt(output, c), fn.Load(sum)));
                    });
                });
            });
        });
    }

    // Explicit instantiations
    template class RegionDetectionLayerNode<float>;
    template class RegionDetectionLayerNode<double>;
    template class RegionDetectionNode<float>;
    template class RegionDetectionNode<double>;
}
}
