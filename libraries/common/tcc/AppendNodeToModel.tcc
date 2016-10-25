////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AppendNodeToModel.tcc (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace common
{
    template <typename PredictorNodeType, typename PredictorType>
    model::Model AppendNodeToModel(model::DynamicMap& map, const PredictorType& predictor)
    {
        model::Model model = map.GetModel();
        auto mapOutput = model::PortElements<double>(map.GetOutputElementsBase(0));
        model.AddNode<PredictorNodeType>(mapOutput, predictor);
        return model;
    }
}
}
