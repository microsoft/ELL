////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogisticFunctionsTest.cpp (emittable_functions)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LogisticFunctionsTest.h"
#include "Emittable_functions_util.h"

#include <emitters/include/IRModuleEmitter.h>

#include <testing/include/testing.h>

#include <utilities/include/FunctionUtils.h>

#include <value/include/ComputeContext.h>
#include <value/include/LLVMContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>
#include <value/include/Scalar.h>
#include <value/include/ScalarOperations.h>

#include <emittable_functions/include/LogisticFunctions.h>

#include <type_traits>
#include <vector>

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;
using namespace ell::emittable_functions;

namespace ell
{

void TestSoftmax()
{
    auto input = std::vector<double>{ 1, 2, 3, 4, 5 };
    auto expected = std::vector<double>{ 0.01165623, 0.03168492, 0.08612854, 0.23412166, 0.63640865 };

    InvokeForContext<ComputeContext>([&](auto&) {
        Vector inputVector(input);
        Vector output = MakeVector<double>(inputVector.Size());
        Softmax(input, output);
        double* buffer = output.GetValue().Get<double*>();
        std::vector<double> actual(buffer, buffer + output.Size());
        bool ok = testing::IsEqual(expected, actual);
        testing::ProcessTest("Testing Softmax", ok);
    });

    InvokeForContext<TestLLVMContext>(PrintIR);
}


void TestSigmoid()
{
    auto input = std::vector<double>{ -10, -5, -3, -2, -1, 0, 1, 2, 3, 5, 10 };
    auto expected = std::vector<double>{ 4.5397868702434395e-05, 0.0066928509242848554, 0.04742587317756678, 0.11920292202211755, 0.2689414213699951, 0.5, 0.7310585786300049, 0.8807970779778823, 0.9525741268224334, 0.9933071490757153, 0.9999546021312976 };

    InvokeForContext<ComputeContext>([&](auto&) {
        Vector inputVector(input);
        Vector output = MakeVector<double>(inputVector.Size());
        For(input, [&](Scalar index) {
            Scalar v = inputVector(index);
            output(index) = Sigmoid(v);
        });
        double* buffer = output.GetValue().Get<double*>();
        std::vector<double> actual(buffer, buffer + output.Size());
        bool ok = testing::IsEqual(expected, actual);
        testing::ProcessTest("Testing Sigmoid", ok);
    });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

void TestHardSigmoid()
{
    auto input = std::vector<double>{ -10, -5, -3, -2, -1, 0, 1, 2, 3, 5, 10 };
    auto expected = std::vector<double>{ 0, 0, 0, 0.1, 0.3, 0.5, 0.7, 0.9, 1, 1, 1 };

    InvokeForContext<ComputeContext>([&](auto&) {
        Vector inputVector(input);
        Vector output = MakeVector<double>(inputVector.Size());
        For(input, [&](Scalar index) {
            Scalar v = inputVector(index);
            output(index) = HardSigmoid(v);
        });
        double* buffer = output.GetValue().Get<double*>();
        std::vector<double> actual(buffer, buffer + output.Size());
        bool ok = testing::IsEqual(expected, actual);
        testing::ProcessTest("Testing HardSigmoid", ok);
    });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

} // namespace ell
