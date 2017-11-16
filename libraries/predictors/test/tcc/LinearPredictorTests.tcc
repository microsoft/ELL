////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictorTests.tcc (predictors_test)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"
// testing
#include "testing.h"

using namespace ell;

template <typename ElementType>
void LinearPredictorTest()
{
    // test that default constructor works.
    predictors::LinearPredictor<ElementType> predictor0;
    testing::ProcessTest("DefaultLinearPredictor", testing::IsEqual(predictor0.GetBias(), static_cast<ElementType>(0)));

    // now test a known predictor.
    using DataVectorType = typename predictors::LinearPredictor<ElementType>::DataVectorType;
    math::ColumnVector<ElementType> weights({ 1, 2, 3, 4, 5 });
    ElementType bias = 1.5;

    predictors::LinearPredictor<ElementType> predictor(weights, bias);
    auto result = predictor.Predict(DataVectorType{ 1.0, 2.0, 1.0, -1.0, 0.5 });

    testing::ProcessTest("TestLinearPredictor", testing::IsEqual(result, static_cast<ElementType>(8.0)));
}
