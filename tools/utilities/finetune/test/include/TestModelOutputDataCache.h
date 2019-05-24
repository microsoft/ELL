////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestModelOutputDataCache.h (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Main driver function
void TestModelOutputDataCache();

// Individual tests
void TestModelOutputDataCache_CreateAndPopulate();
void TestModelOutputDataCache_FindNearestCachedOutput();
void TestModelOutputDataCache_TransformWithCache();
