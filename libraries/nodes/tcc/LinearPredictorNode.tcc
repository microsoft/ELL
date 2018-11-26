////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ElementType>
    LinearPredictorNode<ElementType>::LinearPredictorNode() :
        Node({ &_input }, { &_output, &_weightedElements }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _weightedElements(this, weightedElementsPortName, 0)
    {
    }

    template <typename ElementType>
    LinearPredictorNode<ElementType>::LinearPredictorNode(const model::OutputPort<ElementType>& input, const predictors::LinearPredictor<ElementType>& predictor) :
        Node({ &_input }, { &_output, &_weightedElements }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _weightedElements(this, weightedElementsPortName, input.Size()),
        _predictor(predictor)
    {
        if (input.Size() != predictor.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "LinearPredictorNode: input size must match the predictor size");
        }
    }

    template <typename ElementType>
    void LinearPredictorNode<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["weightedElements"] << _weightedElements;
        archiver["predictor"] << _predictor;
    }

    template <typename ElementType>
    void LinearPredictorNode<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["weightedElements"] >> _weightedElements;
        archiver["predictor"] >> _predictor;
    }

    template <typename ElementType>
    void LinearPredictorNode<ElementType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<LinearPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
        transformer.MapNodeOutput(weightedElements, newNode->weightedElements);
    }

    template <typename ElementType>
    bool LinearPredictorNode<ElementType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);

        auto weightsNode = transformer.AddNode<ConstantNode<ElementType>>(_predictor.GetWeights().ToArray());
        auto dotProductNode = transformer.AddNode<DotProductNode<ElementType>>(weightsNode->output, newPortElements);
        auto coordinatewiseMultiplyNode = transformer.AddNode<BinaryOperationNode<ElementType>>(weightsNode->output, newPortElements, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto biasNode = transformer.AddNode<ConstantNode<ElementType>>(_predictor.GetBias());
        auto addNode = transformer.AddNode<BinaryOperationNode<ElementType>>(dotProductNode->output, biasNode->output, emitters::BinaryOperationType::add);

        transformer.MapNodeOutput(output, addNode->output);
        transformer.MapNodeOutput(weightedElements, coordinatewiseMultiplyNode->output);
        return true;
    }

    template <typename ElementType>
    void LinearPredictorNode<ElementType>::Compute() const
    {
        using DataVectorType = typename LinearPredictorType::DataVectorType;
        auto inputDataVector = DataVectorType(_input.GetValue());
        _output.SetOutput({ _predictor.Predict(inputDataVector) });
        _weightedElements.SetOutput(_predictor.GetWeightedElements(inputDataVector).ToArray());
    }

    template <typename ElementType>
    LinearPredictorNode<ElementType>* AddNodeToModelTransformer(const model::PortElements<ElementType>& input, const predictors::LinearPredictor<ElementType>& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<LinearPredictorNode<ElementType>>(input, predictor);
    }
} // namespace nodes
} // namespace ell
