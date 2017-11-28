////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Variant_test.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
void TestScalarVariant();
void TestVectorVariant();
void TestObjectVariant();

void TestVariantGetValueAs();

void TestVariantParseSimple();
void TestParseVectorVaraint();
void TestParsePortElementsProxyVariant();
void TestParseObjVariant();

void TestVariantToString();
void TestVariantArchive();
}
