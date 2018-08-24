////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AppendNodeToModel.tcc (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace common
{
    template <typename PredictorNodeType, typename PredictorType>
    model::Model AppendNodeToModel(const model::Map& map, const PredictorType& predictor)
    {
        model::TransformContext context;
        model::ModelTransformer transformer;
        auto model = transformer.CopyModel(map.GetModel(), context);
        auto mapOutput = map.GetOutputElements<double>(0);
        model.AddNode<PredictorNodeType>(mapOutput, predictor);
        return model;
    }
}
}
