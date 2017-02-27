////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    namespace ModelBuilderDetail
    {
        // GetArgsSuffixFromVariantVector
        template <typename ArgsTupleType, size_t... Sequence>
        ArgsTupleType GetArgsSuffixFromVariantVectorHelper(const std::vector<utilities::Variant>& args, std::index_sequence<Sequence...>)
        {
            return ArgsTupleType({ args[Sequence].GetValue<typename std::tuple_element<Sequence + 1, ArgsTupleType>::type>() }...);
        }

        template <typename FunctionType>
        auto GetArgsSuffixFromVariantVector(FunctionType& function, const std::vector<utilities::Variant>& args) -> utilities::TupleTailType<utilities::FunctionArgTypes<FunctionType>>
        {
            using ArgTypes = utilities::FunctionArgTypes<FunctionType>;
            using ArgSuffixTypes = utilities::TupleTailType<ArgTypes>;
            return utilities::GetTupleFromVariants<ArgSuffixTypes>(args);
        }

        template <typename FunctionType>
        std::vector<utilities::Variant> GetAddFunctionArgTypes(FunctionType& f)
        {
            return utilities::GetVariantsFromTupleType<utilities::TupleTailType<utilities::FunctionArgTypes<FunctionType>>>();
        }

        // CallAddNodeFunction
        template <typename FunctionType, size_t... Sequence>
        Node* CallAddNodeFunctionHelper(FunctionType& function, Model& model, const std::vector<utilities::Variant>& args, std::index_sequence<Sequence...>)
        {
            auto argsTuple = GetArgsSuffixFromVariantVector(function, args);
            return function(model, std::get<Sequence>(argsTuple)...);
        }

        template <typename FunctionType>
        Node* CallAddNodeFunction(FunctionType& function, Model& model, const std::vector<utilities::Variant>& args)
        {
            using ArgTypes = utilities::FunctionArgTypes<FunctionType>;
            return CallAddNodeFunctionHelper(function, model, args, std::make_index_sequence<std::tuple_size<ArgTypes>::value - 1>());
        }
    }

    // AddNode<NodeType>(Args...)
    template <typename NodeType, typename... Args>
    NodeType* ModelBuilder::AddNode(Model& model, Args&&... args)
    {
        return model.AddNode<NodeType>(args...);
    }

    //
    // RegisterNodeCreator
    //

    // Using Model::AddNode
    template <typename NodeType, typename... ArgTypes>
    void ModelBuilder::RegisterNodeCreator()
    {
        auto addFunction = std::function<Node*(Model&, ArgTypes...)>{ [](Model& model, ArgTypes... args) {
            return model.AddNode<NodeType>(args...);
        } };

        RegisterNodeCreator<NodeType>(NodeType::GetTypeName(), addFunction);
    }

    // Using custom add function
    template <typename NodeType, typename FunctionType>
    void ModelBuilder::RegisterNodeCreator(const std::string& creatorName, FunctionType addFunction)
    {
        auto addNodeFunction = [addFunction](Model& model, const std::vector<utilities::Variant>& args) {
            return ModelBuilderDetail::CallAddNodeFunction(addFunction, model, args);
        };

        std::string key = NodeType::GetTypeName();
        _addNodeFunctions[key] = addNodeFunction;
        _getNodeArgsFunctions[key] = [addFunction]() { return ModelBuilderDetail::GetAddFunctionArgTypes(addFunction); };
    }
}
}
