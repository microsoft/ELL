////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor_test.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include "TensorOperations.h"

// utilities
#include "testing.h"

// stl
#include <cstdlib> // rand

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensor()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);
    testing::ProcessTest("Tensor::NumRows()", T.NumRows() == 10);
    testing::ProcessTest("Tensor::NumColumns()", T.NumColumns() == 20);
    testing::ProcessTest("Tensor::NumChannels()", T.NumChannels() == 30);

    T(3, 2, 1) = 2.0;
    T(4, 3, 2) = 3.0;
    T(3, 3, 3) = 4.0;

    auto S = T.GetSubTensor(3, 2, 1, 5, 5, 5);
    testing::ProcessTest("TensorReference::GetSubTensor", S(0, 0, 0) == 2.0 && S(1, 1, 1) == 3.0 && S(0, 1, 2) == 4.0);

    auto Tc = T.GetConstTensorReference();
    auto Sc = Tc.GetSubTensor(3, 2, 1, 5, 5, 5);
    testing::ProcessTest("ConstTensorReference::GetSubTensor", Sc(0, 0, 0) == 2.0 && Sc(1, 1, 1) == 3.0 && Sc(0, 1, 2) == 4.0);

    math::Tensor<ElementType, dimension0, dimension1, dimension2> T1(T);
    math::ChannelColumnRowTensor<ElementType> T2(Tc);
    math::ColumnRowChannelTensor<ElementType> T3(Tc);
}

template<typename ElementType>
void TestTensorReference()
{
    std::vector<ElementType> values(24);
    ElementType value = 0;
    std::generate(values.begin(), values.end(), [&]() { return value++; });

    math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row> T1(2, 3, 4, values.data());
    testing::ProcessTest("TensorReference::TensorReference(rows, columns, channels, data)", T1(0, 0, 0) == 0 && T1(0, 0, 1) == 1 && T1(1, 2, 2) == 22 && T1(1, 2, 3) == 23);

    math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row> T2(T1);
    testing::ProcessTest("TensorReference::TensorReference(otherTensor)", T2(0, 0, 0) == 0 && T2(0, 0, 1) == 1 && T2(1, 2, 2) == 22 && T2(1, 2, 3) == 23);
}

template<typename ElementType>
void TestCopyFromAndGetSubTensor()
{
    // CopyFrom
    auto A1 = math::ChannelColumnRowTensor<ElementType>{ 
        { { 10,11,12,13 },{ 14,15,16,17 },{ 18,19,10,11 } },
        { { 12,13,14,15 },{ 16,17,18,19 },{ 10,11,12,13 } } };

    auto A2 = math::ChannelColumnRowTensor<ElementType>{ 
        { { 20,21,22,23 },{ 24,25,26,27 },{ 28,29,20,21 } },
        { { 22,23,24,25 },{ 26,27,28,29 },{ 20,21,22,23 } } };

    auto A3 = math::ColumnRowChannelTensor<ElementType>{
        { { 30,31,32,33 },{ 34,35,36,37 },{ 38,39,30,31 } },
        { { 32,33,34,35 },{ 36,37,38,39 },{ 30,31,32,33 } } };

    auto A4 = math::ColumnRowChannelTensor<ElementType>{
        { { 40,41,42,43 },{ 44,45,46,47 },{ 48,49,40,41 } },
        { { 42,43,44,45 },{ 46,47,48,49 },{ 40,41,42,43 } } };

    A1.CopyFrom(A2);
    testing::ProcessTest("Tensor<channel,column,row>::CopyFrom(Tensor<channel,column,row>)", A1 == A2);

    A1.CopyFrom(A3);
    testing::ProcessTest("Tensor<channel,column,row>::CopyFrom(Tensor<column,row,channel>)", A1 == A3);

    A3.CopyFrom(A2);
    testing::ProcessTest("Tensor<column,row,channel>::CopyFrom(Tensor<channel,column,row>)", A3 == A2);

    A1.GetSubTensor({ 0,1,1 }, { 2,2,2 }).CopyFrom(A2.GetSubTensor({ 0,0,2 }, { 2,2,2 }));
    auto R1 = math::ChannelColumnRowTensor<ElementType>{
        { {30, 31, 32, 33}, {34, 22, 23, 37}, {38, 26, 27, 31} },
        { {32, 33, 34, 35}, {36, 24, 25, 39}, {30, 28, 29, 33} } };
    testing::ProcessTest("TensorReference<channel,column,row>::CopyFrom(TensorReference<channel,column,row>)", A1 == R1);

    A1.GetSubTensor({ 0,1,1 }, { 2,2,2 }).CopyFrom(A4.GetSubTensor({ 0,0,2 }, { 2,2,2 }));
    auto R2 = math::ChannelColumnRowTensor<ElementType>{
        { {30, 31, 32, 33}, {34, 42, 43, 37}, {38, 46, 47, 31} },
        { {32, 33, 34, 35}, {36, 44, 45, 39}, {30, 48, 49, 33} } };
    testing::ProcessTest("TensorReference<channel,column,row>::CopyFrom(TensorReference<column,row,channel>)", A1 == R2);

    A4.GetSubTensor({ 0,1,1 }, { 2,2,2 }).CopyFrom(A2.GetSubTensor({ 0,0,2 }, { 2,2,2 }));
    auto R3 = math::ChannelColumnRowTensor<ElementType>{
        { {40, 41, 42, 43}, {44, 22, 23, 47}, {48, 26, 27, 41} },
        { {42, 43, 44, 45}, {46, 24, 25, 49}, {40, 28, 29, 43} } };
    testing::ProcessTest("TensorReference<column,row,channel>::CopyFrom(TensorReference<channel,column,row>)", A4 == R3);
}

template<typename ElementType>
void TestTensorGetSlice()
{
    math::ColumnRowChannelTensor<ElementType> T1(3, 4, 5);
    T1(0, 0, 0) = 1;
    T1(1, 2, 3) = 2;
    T1(0, 3, 3) = 3;
    T1(2, 2, 4) = 3;

    auto M11 = T1.template GetSlice<math::Dimension::column, math::Dimension::row>(3);
    testing::ProcessTest("TensorReference::GetSlice()", M11(2, 1) == 2 && M11(3, 0) == 3);

    auto M12 = T1.template GetSlice<math::Dimension::row, math::Dimension::column>(3);
    testing::ProcessTest("TensorReference::GetSlice()", M12(1, 2) == 2 && M12(0, 3) == 3);

    auto M13 = T1.template GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M13(0, 0) == 1 && M13(3, 3) == 3);

    auto M14 = T1.template GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M14(0, 0) == 1 && M14(3, 3) == 3);

    auto T1c = T1.GetConstTensorReference();

    auto M11c = T1c.template GetSlice<math::Dimension::column, math::Dimension::row>(3);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M11c(2, 1) == 2 && M11c(3, 0) == 3);

    auto M12c = T1c.template GetSlice<math::Dimension::row, math::Dimension::column>(3);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M12c(1, 2) == 2 && M12c(0, 3) == 3);

    auto M13c = T1c.template GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M13c(0, 0) == 1 && M13c(3, 3) == 3);

    auto M14c = T1c.template GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M14c(0, 0) == 1 && M14c(3, 3) == 3);

    math::ChannelColumnRowTensor<ElementType> T2(3, 4, 5);
    T2(0, 0, 0) = 1;
    T2(1, 2, 3) = 2;
    T2(0, 3, 3) = 3;
    T2(2, 2, 4) = 4;

    auto M23 = T2.template GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M23(0, 0) == 1 && M23(3, 3) == 3);

    auto M24 = T2.template GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M24(0, 0) == 1 && M24(3, 3) == 3);

    auto M25 = T2.template GetSlice<math::Dimension::row, math::Dimension::channel>(2);
    testing::ProcessTest("TensorReference::GetSlice()", M25(1, 3) == 2 && M25(2, 4) == 4);

    auto M26 = T2.template GetSlice<math::Dimension::channel, math::Dimension::row>(2);
    testing::ProcessTest("TensorReference::GetSlice()", M26(3, 1) == 2 && M26(4, 2) == 4);

    auto T2c = T2.GetConstTensorReference();

    auto M23c = T2c.template GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M23c(0, 0) == 1 && M23c(3, 3) == 3);

    auto M24c = T2c.template GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M24c(0, 0) == 1 && M24c(3, 3) == 3);

    auto M25c = T2c.template GetSlice<math::Dimension::row, math::Dimension::channel>(2);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M25c(1, 3) == 2 && M25c(2, 4) == 4);

    auto M26c = T2c.template GetSlice<math::Dimension::channel, math::Dimension::row>(2);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M26c(3, 1) == 2 && M26c(4, 2) == 4);
}

template<typename ElementType>
void TestTensorReferenceAsMatrix()
{
    math::ChannelColumnRowTensor<ElementType> T(3, 4, 2);
    T(0, 0, 0) = 1;
    T(0, 0, 1) = 2;
    T(0, 1, 0) = 3;
    T(0, 1, 1) = 4;

    auto M = T.ReferenceAsMatrix();
    testing::ProcessTest("TensorReference::ReferenceAsMatrix()", M(0, 0) == 1 && M(0, 1) == 2 && M(0, 2) == 3 && M(0, 3) == 4);

    auto Tc = T.GetConstTensorReference();
    auto Mc = Tc.ReferenceAsMatrix();
    testing::ProcessTest("ConstTensorReference::ReferenceAsMatrix()", Mc(0, 0) == 1 && Mc(0, 1) == 2 && Mc(0, 2) == 3 && Mc(0, 3) == 4);
}

template<typename ElementType>
void TestTensorLayout()
{
    auto T1 = math::Tensor<ElementType, math::Dimension::column, math::Dimension::row, math::Dimension::channel>(2, 2, 2);
    T1(0, 0, 0) = 1;
    T1(0, 1, 0) = 2;
    T1(1, 0, 0) = 3;
    T1(1, 1, 0) = 4;
    T1(0, 0, 1) = 5;
    T1(0, 1, 1) = 6;
    T1(1, 0, 1) = 7;
    T1(1, 1, 1) = 8;

    auto v1 = T1.ReferenceAsVector();
    auto r = math::RowVector<ElementType>{ 1,2,3,4,5,6,7,8 };
    testing::ProcessTest("TestTensorLayout<column, row, channel>()", v1 == r);

    auto T2 = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>(2, 2, 2);
    T2(0, 0, 0) = 1;
    T2(0, 0, 1) = 2;
    T2(0, 1, 0) = 3;
    T2(0, 1, 1) = 4;
    T2(1, 0, 0) = 5;
    T2(1, 0, 1) = 6;
    T2(1, 1, 0) = 7;
    T2(1, 1, 1) = 8;

    auto v2 = T2.ReferenceAsVector();
    testing::ProcessTest("TestTensorLayout<channel, column, row>()", v2 == r);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestGetSubTensor()
{
    auto T1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(4, 6, 8);
    auto subT1 = T1.GetSubTensor({ 1,2,3 }, {2,3,4});
    subT1.Fill(1);

    auto T2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(4, 6, 8);
    for (size_t i = 1; i < 3; ++i)
    {
        for (size_t j = 2; j < 5; ++j)
        {
            for (size_t k = 3; k < 7; ++k)
            {
                T2(i, j, k) = 1;
            }
        }
    }

    testing::ProcessTest("TestGetSubTensor()", T1.ReferenceAsVector() == T2.ReferenceAsVector());
}

template<typename ElementType>
void TestGetSubTensorAndReferenceAsMatrix()
{
    auto T1 = math::Tensor<ElementType, math::Dimension::column, math::Dimension::row, math::Dimension::channel>(3, 4, 5);
    auto subT1 = T1.GetSubTensor({ 1,0,1 }, { 2,4,2 });
    ElementType c = 1;
    for (size_t k = 0; k < 2; ++k)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                subT1(i, j, k) = c++;
            }
        }
    }

    auto M1 = subT1.ReferenceAsMatrix();
    auto R1 = math::RowMatrix<ElementType>{ {1,2,3,4,5,6,7,8}, {9,10,11,12,13,14,15,16} };
    testing::ProcessTest("void TestGetSubTensorAndReferenceAsMatrix()", M1 == R1);

    // test 2
    auto rand = []() { return static_cast<ElementType>(std::rand() % 100); };
    T1.Generate(rand);

    auto B1 = T1.GetSubTensor(0, 0, 1, 3, 4, 2);
    auto C1 = B1.ReferenceAsMatrix();
    auto D1 = T1.ReferenceAsMatrix();
    auto E1 = D1.GetSubMatrix(1, 0, 2, 12);

    testing::ProcessTest("void TestGetSubTensorAndReferenceAsMatrix()", C1 == E1);

    // test 3
    auto T2 = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>(3, 4, 5);
    auto subT2 = T2.GetSubTensor({ 1,1,0 }, { 2,2,5 });
    c = 1;
    for (size_t i = 0; i < 2; ++i)
    {
        for (size_t j = 0; j < 2; ++j)
        {
            for (size_t k = 0; k < 5; ++k)
            {
                subT2(i, j, k) = c++;
            }
        }
    }

    auto M2 = subT2.ReferenceAsMatrix();
    auto R2 = math::RowMatrix<ElementType>{ { 1,2,3,4,5,6,7,8,9,10 },{ 11,12,13,14,15,16,17,18,19,20 } };
    testing::ProcessTest("void TestGetSubTensorAndReferenceAsMatrix()", M2 == R2);
    
    // test 4
    T2.Generate(rand);

    auto B2 = T2.GetSubTensor(1, 0, 0, 2, 4, 5);
    auto C2 = B2.ReferenceAsMatrix();
    auto D2 = T2.ReferenceAsMatrix();
    auto E2 = D2.GetSubMatrix(1, 0, 2, 20);

    testing::ProcessTest("void TestGetSubTensorAndReferenceAsMatrix()", C2 == E2);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorAdd()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);

    auto v1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    math::TensorOperations::Add<math::Dimension::row>(v1, T);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 }, { 1,1,1,1 }, { 1,1,1,1 } },
                                                                             { { 2,2,2,2 }, { 2,2,2,2 }, { 2,2,2,2 } } };
    testing::ProcessTest("void TestTensorVectorAdd()", T == R1);

    T.Fill(0);
    auto v2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    math::TensorOperations::Add<math::Dimension::column>(v2, T);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } },
                                                                             { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } } };
    testing::ProcessTest("void TestTensorVectorAdd()", T == R2);

    T.Fill(0);
    auto v3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    math::TensorOperations::Add<math::Dimension::channel>(v3, T);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
                                                                             { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } } };
    testing::ProcessTest("void TestTensorVectorAdd()", T == R3);

    // subtensors
    auto TT = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto TR = TT.GetSubTensor({ 5,3,1 }, {2,3,4});

    TR.Fill(0);
    math::TensorOperations::Add<math::Dimension::row>(v1, TR);
    testing::ProcessTest("void TestTensorVectorAdd() with subtensor", TR == R1);

    TR.Fill(0);
    math::TensorOperations::Add<math::Dimension::column>(v2, TR);
    testing::ProcessTest("void TestTensorVectorAdd() with subtensor", TR == R2);

    TR.Fill(0);
    math::TensorOperations::Add<math::Dimension::channel>(v3, TR);
    testing::ProcessTest("void TestTensorVectorAdd() with subtensor", TR == R3);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorMultiply()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T.Fill(1);
    auto v1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    math::TensorOperations::Multiply<math::Dimension::row>(v1, T);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } },
                                                                             { { 2,2,2,2 },{ 2,2,2,2 },{ 2,2,2,2 } } };
    testing::ProcessTest("void TestTensorVectorMultiply()", T == R1);

    T.Fill(1);
    auto v2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    math::TensorOperations::Multiply<math::Dimension::column>(v2, T);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } },
                                                                             { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } } };
    testing::ProcessTest("void TestTensorVectorMultiply()", T == R2);

    T.Fill(1);
    auto v3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    math::TensorOperations::Multiply<math::Dimension::channel>(v3, T);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
                                                                             { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } } };
    testing::ProcessTest("void TestTensorVectorMultiply()", T == R3);

    // subtensors
    auto TT = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto TR = TT.GetSubTensor({ 5,3,1 }, { 2,3,4 });

    TR.Fill(1);
    math::TensorOperations::Multiply<math::Dimension::row>(v1, TR);
    testing::ProcessTest("void TestTensorVectorMultiply() with subtensor", TR == R1);

    TR.Fill(1);
    math::TensorOperations::Multiply<math::Dimension::column>(v2, TR);
    testing::ProcessTest("void TestTensorVectorMultiply() with subtensor", TR == R2);

    TR.Fill(1);
    math::TensorOperations::Multiply<math::Dimension::channel>(v3, TR);
    testing::ProcessTest("void TestTensorVectoMultiply() with subtensor", TR == R3);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorVectorMultiplyAdd()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T.Fill(1);
    auto s1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    auto b1 = math::Vector<ElementType, math::VectorOrientation::row>{ 3,4 };
    math::TensorOperations::MultiplyAdd<math::Dimension::row>(s1, b1, T);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 4,4,4,4 },{ 4,4,4,4 },{ 4,4,4,4 } },
                                                                             { { 6,6,6,6 },{ 6,6,6,6 },{ 6,6,6,6 } } };
    testing::ProcessTest("void TestTensorVectorMultiplyAdd()", T == R1);

    T.Fill(1);
    auto s2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    auto b2 = math::Vector<ElementType, math::VectorOrientation::row>{ 4,5,6 };
    math::TensorOperations::MultiplyAdd<math::Dimension::column>(s2, b2, T);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 5,5,5,5 },{ 7,7,7,7 },{ 9,9,9,9 } },
                                                                             { { 5,5,5,5 },{ 7,7,7,7 },{ 9,9,9,9 } } };
    testing::ProcessTest("void TestTensorVectorMultiplyAdd()", T == R2);

    T.Fill(1);
    auto s3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    auto b3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,1,2,2 };
    math::TensorOperations::MultiplyAdd<math::Dimension::channel>(s3, b3, T);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 2,3,5,6 },{ 2,3,5,6 },{ 2,3,5,6 } },
                                                                             { { 2,3,5,6 },{ 2,3,5,6 },{ 2,3,5,6 } } };
    testing::ProcessTest("void TestTensorVectorMultiplyAdd()", T == R3);

    // subtensors
    auto TT = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto TR = TT.GetSubTensor({ 5,3,1 }, { 2,3,4 });

    TR.Fill(1);
    math::TensorOperations::MultiplyAdd<math::Dimension::row>(s1, b1, TR);
    testing::ProcessTest("void TestTensorVectorMultiplyAdd() with subtensor", TR == R1);

    TR.Fill(1);
    math::TensorOperations::MultiplyAdd<math::Dimension::column>(s2, b2, TR);
    testing::ProcessTest("void TestTensorVectorMultiplyAdd() with subtensor", TR == R2);

    TR.Fill(1);
    math::TensorOperations::MultiplyAdd<math::Dimension::channel>(s3, b3, TR);
    testing::ProcessTest("void TestTensorVectoMultiplyAdd() with subtensor", TR == R3);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorArchiver()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);

    T(3, 2, 1) = 2.0;
    T(4, 3, 2) = 3.0;
    T(3, 3, 3) = 4.0;

    utilities::SerializationContext context;
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    math::TensorArchiver::Write(T, "test", archiver);
    utilities::JsonUnarchiver unarchiver(strstream, context);

    math::Tensor<ElementType, dimension0, dimension1, dimension2> Ta(0, 0, 0);
    math::TensorArchiver::Read(Ta, "test", unarchiver);
    testing::ProcessTest("void TestTensorArchiver(), write and read tensor", Ta == T);
}
