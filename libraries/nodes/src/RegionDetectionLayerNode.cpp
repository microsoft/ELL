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
        auto input = function.LocalArray(compiler.EnsurePortEmitted(this->input));
        auto width = function.LocalScalar(_params.width);
        auto height = function.LocalScalar(_params.height);
        auto numBoxes = function.LocalScalar(_params.numBoxesPerCell);
        auto numClasses = function.LocalScalar(_params.numClasses);
        auto numCoords = function.LocalScalar(_params.numCoordinates);

        auto output = function.LocalArray(compiler.EnsurePortEmitted(this->output));

        auto J = height;
        // the stride between each box in a cell (numCoordinates + 1 + numClasses)
        auto boxStride = numCoords + 1 + numClasses;
        // the stride of each cell (numBoxesPerCell * boxStride)
        auto K = numBoxes * boxStride;
        function.For(width, [input, output, boxStride, numClasses, J, K, height, numBoxes](auto& fn, auto i)
        {
            fn.For(height, [input, output, boxStride, numClasses, i, J, K, numBoxes](auto& fn, auto j)
            {
                auto sliceIndex = ((i * J) + j) * K;
                fn.For(numBoxes, [sliceIndex, input, output, boxStride, numClasses](auto& fn, auto k)
                {
                    auto boxOffset = sliceIndex + (k * boxStride);

                    output[boxOffset + 0] = emitters::Sigmoid<ValueType>(input[boxOffset + 0]);

                    output[boxOffset + 1] = emitters::Sigmoid<ValueType>(input[boxOffset + 1]);

                    output[boxOffset + 2] = Exp(input[boxOffset + 2]);

                    output[boxOffset + 3] = Exp(input[boxOffset + 3]);

                    output[boxOffset + 4] = emitters::Sigmoid<ValueType>(input[boxOffset + 4]);

                    // output[boxOffset + 5 : boxOffset + 5 + numClasses] = softmax(input[boxOffset + 5 : boxOffset + 5 + numClasses])
                    // set the initial max value to the first element
                    // NB: 5 is the result of numCoords + 1, where the 1 is due to the confidence scale value of the prediction.
                    //     If numCoords ever needs support to be anything other than 4, this value will need to be calculated accordingly
                    auto classProbMax = fn.LocalScalar(fn.Variable(emitters::GetVariableType<ValueType>()));
                    fn.Store(classProbMax, static_cast<emitters::IRLocalScalar>(input[boxOffset + 5]));

                    // find the max value in the rest of the probabilities
                    fn.For(boxOffset + 5 + 1, boxOffset + 5 + numClasses,
                           [classProbMax, input](auto& fn, auto c)
                    {
                        emitters::IRLocalScalar val = input[c];
                        fn.If(val > fn.Load(classProbMax), [classProbMax, val](auto& fn)
                        {
                            fn.Store(classProbMax, val);
                        });
                    });

                    // Calculate the sum of the all the euler values, which are calculated using the max above
                    auto sum = fn.Variable(emitters::GetVariableType<ValueType>());
                    fn.Store(sum, fn.Literal(ValueType{ 0 }));
                    fn.For(boxOffset + 5, boxOffset + 5 + numClasses, [sum, classProbMax, input, output](auto& fn, auto c)
                    {
                        emitters::IRLocalScalar inputVal = input[c];
                        auto eulerVal = Exp(inputVal - fn.Load(classProbMax));
                        fn.Store(sum, eulerVal + fn.Load(sum));
                        output[c] = eulerVal;
                    });

                    // Divide each element by the sum
                    fn.For(boxOffset + 5, boxOffset + 5 + numClasses,
                        [sum, output](auto& fn, auto c)
                    {
                        output[c] = (output[c] / fn.Load(sum));
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
