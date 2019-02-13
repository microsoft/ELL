////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizerOptionsTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizerOptionsTest.h"
#include "GlobalOptimizerOptions.h"
#include "OptimizerTestUtil.h"

#include <testing/include/testing.h>

using namespace ell;
using namespace ell::model;
using namespace ell::model::optimizer;
using namespace ell::testing;

//
// Optimizer class tests
//

void TestOptimizerOptions()
{
    OptimizerOptions options;
    ProcessTest("OptimizerOptionsMissingEmptyOnInit", !options.HasOption(""));
    ProcessTest("OptimizerOptionsMissingABCOnInit", !options.HasOption("abc"));

    options.SetOption("abc", "def");
    ProcessTest("OptimizerOptionsMissingEmptyAfterAddABC", !options.HasOption(""));
    ProcessTest("OptimizerOptionsHasABC", options.HasOption("abc"));
    ProcessTest("OptimizerOptionsHasABCValue", options.GetOption("abc") == "def");

    options.SetOption("abc", "ghi");
    ProcessTest("OptimizerOptionsMissingEmptyAfterAddABC", !options.HasOption(""));
    ProcessTest("OptimizerOptionsHasABC", options.HasOption("abc"));
    ProcessTest("OptimizerOptionsHasABCValue", options.GetOption("abc") == "ghi");
}
