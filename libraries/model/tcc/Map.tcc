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
    template <typename... OutputPortTypes>
    Map<OutputPortTypes...>::Map(const Model& model, const std::tuple<PortElements<OutputPortTypes>...>& outputs, const std::array<std::string, sizeof...(OutputPortTypes)>& outputNames)
        : _model(model), _outputNames(outputNames), _outputs(outputs)
    {
    }

    //
    // Refine
    //
    template <typename... OutputPortTypes>
    template <typename OutputElementsType>
    void Map<OutputPortTypes...>::RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer)
    {
        auto refinedOutputElements = modelTransformer.GetCorrespondingOutputs(output);
       output = refinedOutputElements;
    }

    template <typename... OutputPortTypes>
    template <size_t... Sequence>
    void Map<OutputPortTypes...>::RemapOutputElementsHelper(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer)
    {
        RemapOutputElement(std::get<Sequence>(_outputs)..., modelTransformer);
    }

    template <typename... OutputPortTypes>
    void Map<OutputPortTypes...>::Refine(const TransformContext& context)
    {
        ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(_model, context);
        RemapOutputElementsHelper(std::index_sequence_for<OutputPortTypes...>(), transformer);
        _model = refinedModel;
    }

    //
    // Compute
    //
    template <typename... OutputPortTypes>
    template <typename PortElementsType, typename OutputType>
    void Map<OutputPortTypes...>::ComputeElements(PortElementsType& elements, OutputType& output) const
    {
        output = _model.ComputeOutput(elements);
    }

    template <typename... OutputPortTypes>
    template <size_t... Sequence>
    void Map<OutputPortTypes...>::ComputeElementsHelper(std::index_sequence<Sequence...>, std::tuple<std::vector<OutputPortTypes...>>& outputValues) const
    {
        ComputeElements(std::get<Sequence>(_outputs)..., std::get<Sequence>(outputValues)...);
    }

    template <typename... OutputPortTypes>
    std::tuple<std::vector<OutputPortTypes...>> Map<OutputPortTypes...>::Compute() const
    {
        // template <typename ValueType>
        // std::vector<ValueType> ComputeOutput(const PortElements<ValueType>& elements) const;

        // for each of the output port elements, call Compute on the model
        auto result = std::tuple<std::vector<OutputPortTypes...>>();
        ComputeElementsHelper(std::index_sequence_for<OutputPortTypes...>(), result);
        return result;
    }
}
}
