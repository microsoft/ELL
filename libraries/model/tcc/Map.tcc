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
                                                const utilities::WrappedTuple<InputTypesTuple, NamedInput>& inputs,
                                                const utilities::WrappedTuple<OutputTypesTuple, NamedOutput>& outputs)
        : DynamicMap(model)
    {
        AddInputs(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), inputs);
        AddOutputs(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), outputs);
    }

    template <typename NamedInputTypesTuple, typename NamedOutputTypesTuple>
    auto MakeMap(const Model& model,
                 const NamedInputTypesTuple& inputs,
                 const NamedOutputTypesTuple& outputs)
    {
        using utilities::UnwrappedTupleType;
        using InputTypesTuple = UnwrappedTupleType<NamedInputTypesTuple>;
        using OutputTypesTuple = UnwrappedTupleType<NamedOutputTypesTuple>;

        return Map<InputTypesTuple, OutputTypesTuple>(model, inputs, outputs);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddInputs(std::index_sequence<Sequence...>,
                                                           const utilities::WrappedTuple<InputTypesTuple, NamedInput>& inputs)
    {
        utilities::InOrderFunctionEvaluator([&]() { 
            std::get<Sequence>(_inputs) = std::get<1>(std::get<Sequence>(inputs));
            AddInput(std::get<0>(std::get<Sequence>(inputs)), static_cast<InputNodeBase*>(std::get<1>(std::get<Sequence>(inputs)))); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddOutputs(std::index_sequence<Sequence...>,
                                                            const utilities::WrappedTuple<OutputTypesTuple, NamedOutput>& outputs)
    {
        utilities::InOrderFunctionEvaluator([&]() { 
            std::get<Sequence>(_outputs) = std::get<1>(std::get<Sequence>(outputs));
            AddOutput(std::get<0>(std::get<Sequence>(outputs)), static_cast<PortElementsBase>(std::get<1>(std::get<Sequence>(outputs)))); }...);
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
        utilities::InOrderFunctionEvaluator([&]() { RemapInputNode(std::get<Sequence>(_inputs), modelTransformer); }...);
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
        utilities::InOrderFunctionEvaluator([&]() { RemapOutputElement(std::get<Sequence>(_outputs), modelTransformer); }...);
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
    // SetInputValues
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
    void Map<InputTypesTuple, OutputTypesTuple>::ComputeElementsHelper(std::index_sequence<Sequence...>, ComputedOutputType& outputValues) const
    {
        ComputeElements(std::get<Sequence>(_outputs)..., std::get<Sequence>(outputValues)...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    auto Map<InputTypesTuple, OutputTypesTuple>::ComputeOutput() const -> ComputedOutputType
    {
        ComputedOutputType result;
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
        utilities::InOrderFunctionEvaluator([&]() { std::get<Sequence>(_inputs) = dynamic_cast<InputNode<typename std::tuple_element<Sequence, InputTypesTuple>::type>*>(GetInput(Sequence)); }...);
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
        utilities::InOrderFunctionEvaluator([&]() { std::get<Sequence>(_outputs) = static_cast<PortElements<typename std::tuple_element<Sequence, InputTypesTuple>::type>>(GetOutput(Sequence)); }...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    void Map<InputTypesTuple, OutputTypesTuple>::PopulateOutputs()
    {
        PopulateOutputsHelper(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>());
    }
}
}
