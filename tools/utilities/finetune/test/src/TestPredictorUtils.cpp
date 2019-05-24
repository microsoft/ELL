////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestPredictorUtils.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestPredictorUtils.h"

#include "PredictorUtils.h"

#include <math/include/Vector.h>

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>
#include <utilities/include/RandomEngines.h>

// stl
#include <algorithm>
#include <vector>

using namespace ell;
using namespace ell::testing;

namespace
{
const std::string randomSeed = "123";
static std::default_random_engine randomEngine = utilities::GetRandomEngine(randomSeed);

template <typename ValueType>
class Uniform
{
public:
    Uniform(ValueType minVal, ValueType maxVal) :
        _range(static_cast<double>(randomEngine.max() - randomEngine.min())),
        _minOutput(minVal),
        _outputRange(maxVal - minVal) {}

    ValueType operator()()
    {
        double uniform = static_cast<double>(randomEngine()) / _range;
        return static_cast<ValueType>((uniform * _outputRange) + _minOutput);
    }

private:
    double _range;
    ValueType _minOutput;
    ValueType _outputRange;
};

template <typename ElementType>
void FillRandomVector(std::vector<ElementType>& vector, ElementType min, ElementType max)
{
    Uniform<ElementType> rand(min, max);
    std::generate(vector.begin(), vector.end(), rand);
}

template <typename ElementType>
std::vector<ElementType> GetRandomVector(int size, ElementType min = 0, ElementType max = 1)
{
    std::vector<double> values(size);
    FillRandomVector<double>(values, min, max);
    return values;
}

template <typename ElementType>
void FillRandomMatrix(math::RowMatrix<ElementType>& m, ElementType min, ElementType max)
{
    Uniform<ElementType> rand(min, max);
    m.Generate(rand);
}

math::RowMatrix<double> GetRandomPredictorWeights(int numPredictors, int size)
{
    math::RowMatrix<double> result(numPredictors, size);
    FillRandomMatrix(result, -1.0, 1.0);
    return result;
}

math::ColumnVector<double> GetRandomPredictorBiases(int numPredictors)
{
    return { GetRandomVector<double>(numPredictors, 0.0, 1.0) };
}
} // namespace

// Tests
void TestPredictorUtils()
{
    FailOnException(TestGetWeightsAndBias);
}

void TestGetWeightsAndBias()
{
    throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Not implemented");
}
