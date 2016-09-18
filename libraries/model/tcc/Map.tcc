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
                                                const WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
                                                const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames,
                                                const WrappedTuple<OutputTypesTuple, PortElements>& outputs,
                                                const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
        : DynamicMap(model), _inputs(inputs), _inputNames(inputNames), _outputs(outputs), _outputNames(outputNames)
    {
        AddInputsToNameMap(std::make_index_sequence<std::tuple_size<InputTypesTuple>::value>(), _inputs, _inputNames);
        AddOutputsToNameMap(std::make_index_sequence<std::tuple_size<OutputTypesTuple>::value>(), _outputs, _outputNames);
    }

    // Helper function
    template <typename WrappedInputTypesTuple, typename WrappedOutputTypesTuple>
    auto MakeMap(const Model& model,
                 const WrappedInputTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<WrappedInputTypesTuple>::value>& inputNames,
                 const WrappedOutputTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<WrappedOutputTypesTuple>::value>& outputNames)
    {
        using InputTypesTuple = typename UnwrappedTuple<WrappedInputTypesTuple>::type;
        using OutputTypesTuple = typename UnwrappedTuple<WrappedOutputTypesTuple>::type;

        // need to unwrap tuple types
        return Map<InputTypesTuple, OutputTypesTuple>(model, inputs, inputNames, outputs, outputNames);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddInputsToNameMap(std::index_sequence<Sequence...>,
                                                                    WrappedTuple<InputTypesTuple, PointerToInputNode>& inputs,
                                                                    const std::array<std::string, std::tuple_size<InputTypesTuple>::value>& inputNames)
    {
        AddInput(std::get<Sequence>(inputNames)..., static_cast<InputNodeBase*>(std::get<Sequence>(inputs))...);
    }

    template <typename InputTypesTuple, typename OutputTypesTuple>
    template <size_t... Sequence>
    void Map<InputTypesTuple, OutputTypesTuple>::AddOutputsToNameMap(std::index_sequence<Sequence...>,
                                                                     WrappedTuple<OutputTypesTuple, PortElements>& outputs,
                                                                     const std::array<std::string, std::tuple_size<OutputTypesTuple>::value>& outputNames)
    {
        AddOutput(std::get<Sequence>(outputNames)..., static_cast<PortElementsBase>(std::get<Sequence>(outputs))...);
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
    ModelTransformer Map<InputTypesTuple, OutputTypesTuple>::Refine(const TransformContext& context)
    {
        ModelTransformer transformer = DynamicMap::Refine(context);
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
        output = GetModel().ComputeOutput(elements); // elementOutput is a vector<T>, output param is PortElements<T>&...
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
        DynamicMap::ReadFromArchive(archiver);
        // TODO: reconstuct _inputs and _inputNames
        // TODO: reconstuct _outputs and _outputNames
        }
}
}
