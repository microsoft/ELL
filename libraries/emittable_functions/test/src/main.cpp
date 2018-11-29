////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (emittable_functions)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Convolutions_test.h"
#include "Emittable_functions_util.h"
#include "IIRFilter_test.h"
#include "VoiceActivityDetectorTest.h"

#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>

#include <testing/include/testing.h>

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>

#include <iostream>

using namespace ell;
using namespace ell::emitters;
using namespace ell::value;

int main(int argc, char* argv[])
{
    std::string path = ell::utilities::GetDirectoryPath(argv[0]);
    using namespace ell;
    using namespace utilities;

    testing::EnableLoggingHelper enableLogging;

    try
    {
        for (const auto& context : GetContexts())
        {
            value::ContextGuard guard(*context);

            // disabled until we have function name mangling
            // TestVoiceActivityDetector<float>(path);
            TestVoiceActivityDetector<double>(path);

            TestIIRFilter();

            test_simpleDepthwiseSeparableConvolve2D();
        }
    }
    catch (const Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}
