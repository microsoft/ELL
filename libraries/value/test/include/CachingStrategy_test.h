////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CachingStrategy_test.h (value)
//  Authors:  Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace value
{
    class Scalar;
}

// Simple Blas TCOPY tests
value::Scalar BLASTCOPY_ValidateOutput_Test1();
value::Scalar BLASTCOPY_ValidateOutput_Test2();
value::Scalar BLASTCOPY_ValidateMemory_Test1();
value::Scalar BLASTCOPY_ValidateMemory_Test2();
value::Scalar BLASTCOPY_ValidateMemory_Test3();

value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test1();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test2();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test3();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test4();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test5();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test6();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test7();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test8();
value::Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test9();

value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test1();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test2();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test3();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test4();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test5();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test6();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test7();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test8();
value::Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test9();

// Direct convolution caching
value::Scalar ConvolutionWeight_ValidateOutput_Test1();
value::Scalar ConvolutionWeight_Reshape_ValidateMemory_Test1();
value::Scalar ConvolutionInput_ValidateOutput_Test1();
value::Scalar ConvolutionInput_ValidateOutput_Test2();
value::Scalar ConvolutionOutput_ValidateOutput_Test1();
value::Scalar ConvolutionOutput_ValidateOutput_Test1();
value::Scalar DirectConvolution_Test1();

// General caching strategy
value::Scalar GeneralCachingStrategy_ValidateOutput_Test1();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test2();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test3();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test4();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test5();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test6();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test7();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test8();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test9();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test10();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test11();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test12();
value::Scalar GeneralCachingStrategy_ValidateOutput_Test13();

value::Scalar GeneralCachingStrategy_ValidateMemory_Test1();

// General caching strategy boundary condition output tests
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test1();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test2();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test3();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test4();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test5();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test6();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test7();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test8();
value::Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test9();

// General caching strategy BLASTCopy-style tests
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_Test1();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_Test2();

value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test1();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test2();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test3();

value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test1();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test2();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test3();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test4();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test5();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test6();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test7();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test8();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test9();

value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test1();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test2();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test3();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test4();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test5();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test6();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test7();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test8();
value::Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test9();

// General caching strategy ProgressiveBLASNCopy-style tests

value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_Test1();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_Test2();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test1();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test2();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test3();

value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test1();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test2();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test3();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test4();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test5();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test6();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test7();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test8();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test9();

value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test1();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test2();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test3();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test4();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test5();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test6();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test7();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test8();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test9();

value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test1();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test2();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test3();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test4();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test5();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test6();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test7();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test8();
value::Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test9();

} // namespace ell
