////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AppendNodeToModel.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Map.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>

namespace ell
{
namespace common
{
    /// <summary> Appends a predictor node of the given type to the model in a map. </summary>
    ///
    /// <typeparam name="PredictorNodeType"> The type of the new predictor node to add </typeparam>
    /// <typeparam name="PredictorType"> The type of the predictor to add </typeparam>
    /// <param name="map"> The map </param>
    /// <param name="predictor"> The predictor to wrap in a node and add to the model </param>
    /// <returns> The new model, with the predictor node appended </returns>
    template <typename PredictorNodeType, typename PredictorType>
    model::Model AppendNodeToModel(const model::Map& map, const PredictorType& predictor);
} // namespace common
} // namespace ell

#pragma region implementation

namespace ell
{
namespace common
{
    template <typename PredictorNodeType, typename PredictorType>
    model::Model AppendNodeToModel(const model::Map& map, const PredictorType& predictor)
    {
        auto model = map.GetModel().ShallowCopy();
        const auto& mapOutput = map.GetOutputElements<double>(0);
        model.AddNode<PredictorNodeType>(mapOutput, predictor);
        return model;
    }
} // namespace common
} // namespace ell

#pragma endregion implementation
