////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (utilities_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Format_test.h"
#include "FunctionUtils_test.h"
#include "Archiver_test.h"
#include "Iterator_test.h"
#include "ObjectArchive_test.h"
#include "PropertyBag_test.h"
#include "TypeFactory_test.h"
#include "TypeName_test.h"
#include "Variant_test.h"
#include "Files_test.h"
#include "Files.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"
#include "Unused.h"

// stl
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace ell;

/// Runs all tests
///
int main(int argc, char* argv[])
{
    UNUSED(argc);
    try
    {
        std::string basePath = ell::utilities::GetDirectoryPath(argv[0]);

        // Format tests
        TestMatchFormat();

        // Iterator tests
        TestIteratorAdapter();
        TestTransformIterator();
        TestParallelTransformIterator();
        TestStlStridedIterator();

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
        TestVariantArchive();

        // Verison number tests
        TestArchivedObjectInfo();
        TestArchiveVersion();

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

        // File system tests
        TestStringf();
        TestJoinPaths(basePath);
        TestUnicodePaths(basePath);

        // PropertyBag tests
        TestPropertyBag();
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
