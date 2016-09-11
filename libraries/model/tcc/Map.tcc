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
    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    Map<InputNodeTypesTuple, OutputPortTypesTuple>::Map(const Model& model, const InputNodeTypesTuple& inputs,
                                                        const std::array<std::string, std::tuple_size<InputNodeTypesTuple>::value>& inputNames,
                                                        const OutputPortTypesTuple& outputs,
                                                        const std::array<std::string, std::tuple_size<OutputPortTypesTuple>::value>& outputNames)
        : _model(model), _inputs(inputs), _inputNames(inputNames), _outputs(outputs), _outputNames(outputNames)
    {
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    auto MakeMap(const Model& model,
                 const InputNodeTypesTuple& inputs,
                 const std::array<std::string, std::tuple_size<InputNodeTypesTuple>::value>& inputNames,
                 const OutputPortTypesTuple& outputs,
                 const std::array<std::string, std::tuple_size<OutputPortTypesTuple>::value>& outputNames)
    {
        return Map<InputNodeTypesTuple, OutputPortTypesTuple>(model, inputs, inputNames, outputs, outputNames);
    }

    //
    // Refine
    //
    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <typename OutputElementsType>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer)
    {
        auto refinedOutputElements = modelTransformer.GetCorrespondingOutputs(output);
        output = refinedOutputElements;
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <size_t... Sequence>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::RemapOutputElementsHelper(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer)
    {
        RemapOutputElement(std::get<Sequence>(_outputs)..., modelTransformer);
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::Refine(const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(_model, context);
        RemapOutputElementsHelper(std::make_index_sequence<std::tuple_size<OutputPortTypesTuple>::value>(), transformer);
        _model = refinedModel;
    }

    //
    // SetInputs
    //
    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <typename InputNodeType, typename InputType>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::SetNodeInput(InputNode<InputNodeType>* inputNode, const InputType& inputValues)
    {
        inputNode->SetInput(inputValues);
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <typename InputNodesTupleType, size_t... Sequence>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::SetInputElementsHelper(std::index_sequence<Sequence...> seq, const InputNodesTupleType& inputValues)
    {
        std::cout << "Index sequence size: " << seq.size() << ", stored size: " << std::tuple_size<InputNodeTypesTuple>::value << ", passed-in size: " << std::tuple_size<InputNodesTupleType>::value << std::endl;
       SetNodeInput(std::get<Sequence>(_inputs)..., std::get<Sequence>(inputValues)...);
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <typename... InputNodeTypes>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::SetInputs(const std::tuple<std::vector<InputNodeTypes>...>& inputValues)
    {
        SetInputElementsHelper(std::index_sequence_for<InputNodeTypes...>(), inputValues);    
    }

    //
    // Compute
    //
    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <typename PortElementsType, typename OutputType>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::ComputeElements(PortElementsType& elements, OutputType& output) const
    {
        auto elementOutput = _model.ComputeOutput(elements); // elementOutput is a vector<T>, output param is PortElements<T>&... 
        output = elementOutput;
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    template <size_t... Sequence>
    void Map<InputNodeTypesTuple, OutputPortTypesTuple>::ComputeElementsHelper(std::index_sequence<Sequence...>, typename TupleVectorMakerFromPortElements<OutputPortTypesTuple>::type& outputValues) const
    {
        ComputeElements(std::get<Sequence>(_outputs)..., std::get<Sequence>(outputValues)...);
    }

    template <typename InputNodeTypesTuple, typename OutputPortTypesTuple>
    typename TupleVectorMakerFromPortElements<OutputPortTypesTuple>::type Map<InputNodeTypesTuple, OutputPortTypesTuple>::Compute() const
    {
        typename TupleVectorMakerFromPortElements<OutputPortTypesTuple>::type result;
        ComputeElementsHelper(std::make_index_sequence<std::tuple_size<OutputPortTypesTuple>::value>(), result);
        return result;
    }
}
}
