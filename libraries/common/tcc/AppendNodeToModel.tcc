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
    model::Model AppendNodeToModel(model::Map& map, const PredictorType& predictor)
    {
        model::Model model = map.GetModel();
        auto mapOutput = map.GetOutputElements<double>(0);
        model.AddNode<PredictorNodeType>(mapOutput, predictor);
        return model;
    }
}
}
