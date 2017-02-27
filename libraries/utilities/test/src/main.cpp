////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (utilities_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Format_test.h"
#include "FunctionUtils_test.h"
#include "IArchivable_test.h"
#include "Iterator_test.h"
#include "ObjectArchive_test.h"
#include "TypeFactory_test.h"
#include "TypeName_test.h"
#include "Variant_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace ell;

/// Runs all tests
///
int main()
{
    try
    {
        // Format tests
        TestMatchFormat();

        // Iterator tests
        TestIteratorAdapter();
        TestTransformIterator();
        TestParallelTransformIterator();

        // TypeFactory tests
        TypeFactoryTest();

        // Variant tests
        TestScalarVariant();
        TestVectorVariant();
        TestObjectVariant();
        TestVariantGetValueAs();
        TestVariantToString();
        TestVariantParseSimple();
        TestParseVectorVaraint();
        TestParsePortElementsProxyVariant();
        TestParseObjVariant();

        // Serialization tests
        TestJsonArchiver();
        TestJsonUnarchiver();

        // TestXmlArchiver();
        // TestXmlUnarchiver();

        // ObjectArchive tests
        TestGetTypeDescription();
        TestGetObjectArchive();
        TestSerializeIArchivable();
        TestObjectArchiver();

        // TypeName tests
        TestFundamentalTypeNames();
        TestClassTypeNames();
        TestEnumTypeNames();

        // FunctionUtil tests
        TestInOrderFunctionEvaluator();
        TestApplyToEach();
        TestFunctionTraits();
        TestApplyFunction();
    }
    catch (const utilities::Exception& exception)
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
