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
    // Constructor
    template <typename InputTypesTuple, typename OutputTypesTuple>
    Map<InputTypesTuple, OutputTypesTuple>::Map(const Model& model,
                                                const utilities::WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
                                                const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                                                const utilities::WrappedTuple<OutputTypesTuple, PortElements>& outputs,
                                                const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
        : DynamicMap(model), _inputs(inputs), _outputs(outputs)
    {
        AddInputsToNameMap(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), _inputs, inputNames);
        AddOutputsToNameMap(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), _outputs, outputNames);
    }

    // Helper function
    template <typename WrappedInputTypesTuple, typename WrappedOutputTypesTuple>
    auto MakeMap(const Model& model,
                 const WrappedInputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<WrappedInputTypesTuple>::value>& inputNames,
                 const WrappedOutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<WrappedOutputTypesTuple>::value>& outputNames)
    {
        using InputTypesTuple = utilities::UnwrappedTupleType<WrappedInputTypesTuple>;
        using OutputTypesTuple = utilities::UnwrappedTupleType<WrappedOutputTypesTuple>;

        return Map<InputTypesTuple, OutputTypesTuple>(model, inputs, inputNames, outputs, outputNames);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddInputsToNameMap(std::index_sequence<Sequence...>,
                                                                    utilities::WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
                                                                    const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames)
    {
        utilities::EvalInOrder([&]() { AddInput(std::get<Sequence>(inputNames), static_cast<InputNodeBase*>(std::get<Sequence>(inputs))); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddOutputsToNameMap(std::index_sequence<Sequence...>,
                                                                     utilities::WrappedTuple<OutputTypesTuple, PortElements>& outputs,
                                                                     const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
    {
        utilities::EvalInOrder([&]() { AddOutput(std::get<Sequence>(outputNames), static_cast<PortElementsBase>(std::get<Sequence>(outputs))); }...);
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
        utilities::EvalInOrder([&]() { RemapInputNode(std::get<Sequence>(_inputs), modelTransformer); }...);
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
        utilities::EvalInOrder([&]() { RemapOutputElement(std::get<Sequence>(_outputs), modelTransformer); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    ModelTransformer Map<InputTypesTuple, OutputTypesTuple>::DoRefine(const TransformContext& context)
    {
        ModelTransformer transformer = DynamicMap::DoRefine(context);
        RemapInputNodes(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), transformer);
        RemapOutputElements(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), transformer);
        return transformer;
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

    //
    // Compute
    //
    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <typename PortElementsType, typename OutputType>
    void Map<InputTypesTuple, OutputTypesTuple>::ComputeElements(PortElementsType& elements, OutputType& output) const
    {
        output = GetModel().ComputeOutput(elements);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::ComputeElementsHelper(std::index_sequence<Sequence...>, ComputeOutputType& outputValues) const
    {
        ComputeElements(std::get<Sequence>(_outputs)..., std::get<Sequence>(outputValues)...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto Map<InputTypesTuple, OutputTypesTuple>::ComputeOutput() const -> ComputeOutputType
    {
        ComputeOutputType result;
        ComputeElementsHelper(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), result);
        return result;
    }

    //
    // Serialization
    //
    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::WriteToArchive(utilities::Archiver& archiver) const
    {
        DynamicMap::WriteToArchive(archiver);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        DynamicMap::ReadFromArchive(archiver); // Rats! we don't know the order they were serialized
        PopulateInputs(); // reconstuct _inputs
        PopulateOutputs(); // reconstuct _outputs
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::PopulateInputsHelper(std::index_sequence<Sequence...>)
    {
        utilities::EvalInOrder([&]() { std::get<Sequence>(_inputs) = dynamic_cast<InputNode<typename std::tuple_element<Sequence, InputTypesTuple>::type>*>(GetInput(Sequence)); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::PopulateInputs()
    {
        PopulateInputsHelper(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>());
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::PopulateOutputsHelper(std::index_sequence<Sequence...>)
    {
        utilities::EvalInOrder([&]() { std::get<Sequence>(_outputs) = static_cast<PortElements<typename std::tuple_element<Sequence, InputTypesTuple>::type>>(GetOutput(Sequence)); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::PopulateOutputs()
    {
        PopulateOutputsHelper(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>());
    }
}
}
