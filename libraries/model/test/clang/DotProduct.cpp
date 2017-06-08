////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DotProduct.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// compile with clang -S -emit-llvm -o DotProduct.ll DotProduct.cpp

extern "C"
{
void _Node__DotProduct(double* input1, double* input2, int size, double* output)
{
    double result = 0;
    for(int index = 0; index < size; ++index)
    {
        result += input1[index]*input2[index];
    }
    output[0] = result;
}
}
