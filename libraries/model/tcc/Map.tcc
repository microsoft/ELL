////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace model
{
    template <typename InputTypesTuple, typename OutputTypesTuple>
    Map<InputTypesTuple, OutputTypesTuple>::Map(const Model& model,
                                                const InputTypesTuple& inputs,
                                                const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                                                const OutputTypesTuple& outputs,
                                                const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
        : _model(model), _inputs(inputs), _inputNames(inputNames), _outputs(outputs), _outputNames(outputNames)
    {
        AddInputsToNameMap(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), _inputs, _inputNames);
        AddOutputsToNameMap(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), _outputs, _outputNames);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto MakeMap(const Model& model,
                 const InputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                 const OutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
    {
        return Map<InputTypesTuple, OutputTypesTuple>(model, inputs, inputNames, outputs, outputNames);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddInputsToNameMap(std::index_sequence<Sequence...>,
                                                                    InputTypesTuple& inputs,
                                                                    const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames)
    {
        _inputNodeMap.insert({ std::get<Sequence>(inputNames)..., static_cast<Node*>(std::get<Sequence>(inputs))... });
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddOutputsToNameMap(std::index_sequence<Sequence...>,
                                                                     OutputTypesTuple& outputs,
                                                                     const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
    {
        _outputElementsMap.insert({ std::get<Sequence>(outputNames)..., static_cast<PortElementsBase&>(std::get<Sequence>(outputs))... });
    }

    //
    // Refine
    //
    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename InputNodeType>
    void Map<InputTypesTuple, OutputTypesTuple>::RemapInputNode(InputNodeType& input, ModelTransformer& modelTransformer)
    {
        auto refinedInput = modelTransformer.GetCorrespondingInputNode(input);
        input = refinedInput;
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::RemapInputNodes(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer)
    {
        RemapInputNode(std::get<Sequence>(_inputs)..., modelTransformer);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename OutputElementsType>
    void Map<InputTypesTuple, OutputTypesTuple>::RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer)
    {
        auto refinedOutputElements = modelTransformer.GetCorrespondingOutputs(output);
        output = refinedOutputElements;
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::RemapOutputElements(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer)
    {
        RemapOutputElement(std::get<Sequence>(_outputs)..., modelTransformer);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::Refine(const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(_model, context);
        RemapInputNodes(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), transformer);
        RemapOutputElements(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), transformer);
        _model = refinedModel;
    }

    //
    // SetInputs
    //
    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename InputNodeType, typename InputType>
    void Map<InputTypesTuple, OutputTypesTuple>::SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues)
    {
        inputNode->SetInput(inputValues);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename InputNodesTupleType, size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::SetInputElementsHelper(std::index_sequence<Sequence...> seq, const InputNodesTupleType& inputValues)
    {
        SetNodeInput(std::get<Sequence>(_inputs)..., std::get<Sequence>(inputValues)...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename... InputNodeTypes>
    void Map<InputTypesTuple, OutputTypesTuple>::SetInputTuple(const std::tuple<std::vector<InputNodeTypes>...>& inputTuple)
    {
        SetInputElementsHelper(std::index_sequence_for<InputNodeTypes...>(), inputTuple);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename... InputTypes>
    void Map<InputTypesTuple, OutputTypesTuple>::SetInputs(std::vector<InputTypes>... inputs)
    {
        SetInputTuple(std::tuple<std::vector<InputTypes>...>(inputs...));
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename ValueType>
    void Map<InputTypesTuple, OutputTypesTuple>::SetInput(const std::string& inputName, const std::vector<ValueType>& inputValues)
    {
        auto iter = _inputNodeMap.find(inputName);
        if (iter == _inputNodeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        auto node = dynamic_cast<InputNode<ValueType>*>(iter->second);
        if (node != nullptr)
        {
            node->SetInput(inputValues);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    //
    // Compute
    //
    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename PortElementsType, typename OutputType>
    void Map<InputTypesTuple, OutputTypesTuple>::ComputeElements(PortElementsType& elements, OutputType& output) const
    {
        output = _model.ComputeOutput(elements); // elementOutput is a vector<T>, output param is PortElements<T>&...
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::ComputeElementsHelper(std::index_sequence<Sequence...>, ComputeOutputType& outputValues) const
    {
        ComputeElements(std::get<Sequence>(_outputs)..., std::get<Sequence>(outputValues)...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto Map<InputTypesTuple, OutputTypesTuple>::Compute() const -> ComputeOutputType
    {
        ComputeOutputType result;
        ComputeElementsHelper(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), result);
        return result;
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename ValueType>
    std::vector<ValueType> Map<InputTypesTuple, OutputTypesTuple>::ComputeOutput(const std::string& outputName)
    {
        auto iter = _outputElementsMap.find(outputName);
        if (iter == _outputElementsMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        auto elements = dynamic_cast<const PortElements<ValueType>&>(iter->second);
        return _model.ComputeOutput(elements);
    }
}
}
