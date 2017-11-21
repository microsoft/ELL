////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNPredictorTests.h (predictors_test)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProtoNNPredictor.h"

// testing
#include "testing.h"

using namespace ell;

void ProtoNNPredictorTest()
{
    size_t dim = 5, projectedDim = 4, numPrototypes = 3, numLabels = 2;
    double gamma = 0.3;
    predictors::ProtoNNPredictor protonnPredictor(dim, projectedDim, numPrototypes, numLabels, gamma);

    // projectedDim * dim
    auto W = protonnPredictor.GetProjectionMatrix().GetReference();
    // clang-format off
    W(0, 0) = 0.4; W(0, 1) = 0.5; W(0, 2) = 0.1; W(0, 3) = 0.1; W(0, 4) = 0.1;
    W(1, 0) = 0.1; W(1, 1) = 0.4; W(1, 2) = 0.8; W(1, 3) = 0.2; W(1, 4) = 0.5;
    W(2, 0) = 0.2; W(2, 1) = 0.1; W(2, 2) = 0.7; W(2, 3) = 0.3; W(2, 4) = 0.4;
    W(3, 0) = 0.3; W(3, 1) = 0.3; W(3, 2) = 0.2; W(3, 3) = 0.5; W(3, 4) = 0.2;
    // clang-format on

    // projectedDim * numPrototypes
    auto B = protonnPredictor.GetPrototypes().GetReference();
    // clang-format off
    B(0, 0) = 0.1; B(0, 1) = 0.2; B(0, 2) = 0.3;
    B(1, 0) = 0.8; B(1, 1) = 0.7; B(1, 2) = 0.6;
    B(2, 0) = 0.4; B(2, 1) = 0.6; B(2, 2) = 0.2;
    B(3, 0) = 0.2; B(3, 1) = 0.1; B(3, 2) = 0.3;
    // clang-format on

    // numLabels * numPrototypes
    auto Z = protonnPredictor.GetLabelEmbeddings().GetReference();
    // clang-format off
    Z(0, 0) = 0.1; Z(0, 1) = 0.3; Z(0, 2) = 0.2;
    Z(1, 0) = 0.2; Z(1, 1) = 0.4; Z(1, 2) = 0.8;
    // clang-format on

    auto prediction = protonnPredictor.Predict(std::vector<double>{ 0.2, 0.5, 0.6, 0.8, 0.1 });

    auto maxElement = std::max_element(prediction.GetDataPointer(), prediction.GetDataPointer() + prediction.Size());
    ptrdiff_t maxLabelIndex = maxElement - prediction.GetDataPointer();

    ptrdiff_t R = 1;
    double score = 1.321484;

    testing::ProcessTest("ProtoNNPredictorTest", testing::IsEqual(maxLabelIndex, R));
    testing::ProcessTest("ProtoNNPredictorTest", testing::IsEqual(*maxElement, score, 1e-6));
}

