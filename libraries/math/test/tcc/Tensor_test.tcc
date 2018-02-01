////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor_test.tcc (math_test)
//  Authors:  Ofer Dekel, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include "TensorOperations.h"

// utilities
#include "testing.h"

// stl
#include <cstdlib> // rand

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorIndexer()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto S = T.GetSubTensor({ 0,1,2 }, { 2,2,2 });

    T(1, 2, 3) = 7;
    T(0, 1, 2) = 8;

    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,8,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,7 } }
    };

    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 8, 4 },{ 3, 4 } },
        { { 3, 4 },{ 3, 7 } }
    };

    testing::ProcessTest("Tensor::operator()", T == R1 && S == R2);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorSize()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);
    auto S = T.GetSubTensor({ 0,1,2 }, { 2,2,2 });

    testing::ProcessTest("Tensor::Size", T.Size() == 10*20*30 && S.Size() == 2*2*2);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumRows()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);

    testing::ProcessTest("Tensor::NumRows", T.NumRows() == 10);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumColumns()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);

    testing::ProcessTest("Tensor::NumColumns", T.NumColumns() == 20);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorNumChannels()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);

    testing::ProcessTest("Tensor::NumChannels", T.NumChannels() == 30);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetShape()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);
    auto shape = T.GetShape();

    testing::ProcessTest("Tensor::GetShape", shape == math::TensorShape{10,20,30});
}

template<typename ElementType>
void TestTensorNumSlices()
{
    math::ColumnRowChannelTensor<ElementType> T(10, 20, 30);
    math::ChannelColumnRowTensor<ElementType> S(10, 20, 30);

    testing::ProcessTest("Tensor::NumSlices",
        math::NumSlices<math::Dimension::column, math::Dimension::row>(T) == 30
        && math::NumSlices<math::Dimension::row, math::Dimension::column>(T) == 30
        && math::NumSlices<math::Dimension::column, math::Dimension::channel>(T) == 10
        && math::NumSlices<math::Dimension::channel, math::Dimension::column>(T) == 10
        && math::NumSlices<math::Dimension::channel, math::Dimension::row>(S) == 20
        && math::NumSlices<math::Dimension::row, math::Dimension::channel>(S) == 20
        && math::NumSlices<math::Dimension::column, math::Dimension::channel>(S) == 10
        && math::NumSlices<math::Dimension::channel, math::Dimension::column>(S) == 10);

    auto test1DNumSlices = [](auto T)
    {
        testing::ProcessTest("Tensor::NumSlices",
            math::NumSlices<math::Dimension::channel>(T) == (10 * 20)
            && math::NumSlices<math::Dimension::column>(T) == (10 * 30)
            && math::NumSlices<math::Dimension::row>(T) == (20 * 30));
    };
    test1DNumSlices(T);
    test1DNumSlices(S);
}

template<typename ElementType>
void TestTensorNumPrimarySlices()
{
    math::ColumnRowChannelTensor<ElementType> T(10, 20, 30);
    math::ChannelColumnRowTensor<ElementType> S(10, 20, 30);

    testing::ProcessTest("Tensor::NumPrimarySlices", T.NumPrimarySlices() == 30 && S.NumPrimarySlices() == 10);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorIsEqual()
{
    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    testing::ProcessTest("Tensor::IsEqual", S.IsEqual(T) && T.GetSubTensor({ 0,1,2 }, { 2,2,2 }).IsEqual(S.GetSubTensor({ 0,1,2 }, { 2,2,2 })));
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorEqualityOperator()
{
    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    testing::ProcessTest("Tensor::operator==", T == S && T.GetSubTensor({ 0,1,2 }, {2,2,2}) == S.GetSubTensor({ 0,1,2 }, { 2,2,2 }));
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorInequalityOoperator()
{
    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,8,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto U = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 } }
    };

    testing::ProcessTest("Tensor::operator!=", T != S && T.GetSubTensor({ 0,1,2 }, { 2,2,2 }) != S.GetSubTensor({ 0,1,2 }, { 2,2,2 }) && T != U);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetConstReference()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
        { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } }
    };

    auto S = T.GetSubTensor({ 0,1,2 }, { 2,2,2 });

    testing::ProcessTest("Tensor::operator==", T == T.GetConstReference() && S == S.GetConstReference());
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetSubTensor()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(4, 6, 8);
    auto subT = T.GetSubTensor({ 1,2,3 }, { 2,3,4 });
    subT.Fill(1);

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>(4, 6, 8);
    for (size_t i = 1; i < 3; ++i)
    {
        for (size_t j = 2; j < 5; ++j)
        {
            for (size_t k = 3; k < 7; ++k)
            {
                S(i, j, k) = 1;
            }
        }
    }

    testing::ProcessTest("TestGetSubTensor()", T == S);
}

template <typename ElementType>
void TestTensorGetSlice()
{
    math::ColumnRowChannelTensor<ElementType> T1(3, 4, 5);
    T1(0, 0, 0) = 1;
    T1(1, 2, 3) = 2;
    T1(0, 3, 3) = 3;
    T1(2, 2, 4) = 3;

    auto T1Test2DSlice = [](auto T)
    {
        auto M1 = math::GetSlice<math::Dimension::column, math::Dimension::row>(T, 3);
        testing::ProcessTest("TensorReference::GetSlice()", M1(2, 1) == 2 && M1(3, 0) == 3);

        auto M2 = math::GetSlice<math::Dimension::row, math::Dimension::column>(T, 3);
        testing::ProcessTest("TensorReference::GetSlice()", M2(1, 2) == 2 && M2(0, 3) == 3);

        auto M3 = math::GetSlice<math::Dimension::column, math::Dimension::channel>(T, 0);
        testing::ProcessTest("TensorReference::GetSlice()", M3(0, 0) == 1 && M3(3, 3) == 3);

        auto M4 = math::GetSlice<math::Dimension::channel, math::Dimension::column>(T, 0);
        testing::ProcessTest("TensorReference::GetSlice()", M4(0, 0) == 1 && M4(3, 3) == 3);
    };

    T1Test2DSlice(T1);
    T1Test2DSlice(T1.GetConstReference());

    math::ChannelColumnRowTensor<ElementType> T2(3, 4, 5);
    T2(0, 0, 0) = 1;
    T2(1, 2, 3) = 2;
    T2(0, 3, 3) = 3;
    T2(2, 2, 4) = 4;

    auto T2Test2DSlice = [](auto T)
    {
        auto M1 = math::GetSlice<math::Dimension::column, math::Dimension::channel>(T, 0);
        testing::ProcessTest("TensorReference::GetSlice()", M1(0, 0) == 1 && M1(3, 3) == 3);

        auto M2 = math::GetSlice<math::Dimension::channel, math::Dimension::column>(T, 0);
        testing::ProcessTest("TensorReference::GetSlice()", M2(0, 0) == 1 && M2(3, 3) == 3);

        auto M3 = math::GetSlice<math::Dimension::row, math::Dimension::channel>(T, 2);
        testing::ProcessTest("TensorReference::GetSlice()", M3(1, 3) == 2 && M3(2, 4) == 4);

        auto M4 = math::GetSlice<math::Dimension::channel, math::Dimension::row>(T, 2);
        testing::ProcessTest("TensorReference::GetSlice()", M4(3, 1) == 2 && M4(4, 2) == 4);
    };

    T2Test2DSlice(T2);
    T2Test2DSlice(T2.GetConstReference());

    auto vectorSliceTest = [](auto _)
    {
        using TensorType = decltype(_);

        // T = numpy.arange(5 * 7 * 11).reshape(5, 7, 11)
        TensorType T(5, 7, 11);
        for (unsigned i = 0; i < 5; ++i)
        {
            for (unsigned j = 0; j < 7; ++j)
            {
                for (unsigned k = 0; k < 11; ++k)
                {
                    T(i, j, k) = static_cast<typename TensorType::TensorElementType>(k + j * 11 + i * 77);
                }
            }
        }

        auto test1DGetSlice = [](auto T)
        {
            // equivalent of NumPy's T[4, 6, ...]
            auto V1 = math::GetSlice<math::Dimension::channel>(T, 4, 6);
            testing::ProcessTest("TensorReference::GetSlice()", V1 == math::ColumnVector<ElementType>({ 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384 }));

            // equivalent of NumPy's T[4, ..., 8]
            auto V2 = math::GetSlice<math::Dimension::column>(T, 4, 8);
            testing::ProcessTest("TensorReference::GetSlice()", V2 == math::ColumnVector<ElementType>({ 316, 327, 338, 349, 360, 371, 382 }));

            // equivalent of NumPy's T[..., 6, 8]
            auto V3 = math::GetSlice<math::Dimension::row>(T, 6, 8);
            testing::ProcessTest("TensorReference::GetSlice()", V3 == math::ColumnVector<ElementType>({ 74, 151, 228, 305, 382 }));
        };

        test1DGetSlice(T);
        test1DGetSlice(T.GetConstReference());

        typename TensorType::TensorElementType originalElementVal = 0;

        // T[..., 6, 8][0] = 0
        auto V1 = math::GetSlice<math::Dimension::channel>(T, 4, 6);
        std::swap(originalElementVal, V1[0]);
        testing::ProcessTest("TensorReference::GetSlice() after modification", V1 == math::ColumnVector<ElementType>({ 0, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384 }));
        testing::ProcessTest("T(4, 6, 0) == 0", T(4, 6, 0) == 0);
        std::swap(originalElementVal, V1[0]);

        // T[4..., 8][0] = 0
        auto V2 = math::GetSlice<math::Dimension::column>(T, 4, 8);
        std::swap(originalElementVal, V2[0]);
        testing::ProcessTest("TensorReference::GetSlice() after modification", V2 == math::ColumnVector<ElementType>({ 0, 327, 338, 349, 360, 371, 382 }));
        testing::ProcessTest("T(4, 0, 8) == 0", T(4, 0, 8) == 0);
        std::swap(originalElementVal, V2[0]);

        // T[4, 6, ...][0] = 0
        auto V3 = math::GetSlice<math::Dimension::row>(T, 6, 8);
        std::swap(originalElementVal, V3[0]);
        testing::ProcessTest("TensorReference::GetSlice() after modification", V3 == math::ColumnVector<ElementType>({ 0, 151, 228, 305, 382 }));
        testing::ProcessTest("T(0, 6, 8) == 0", T(0, 6, 8) == 0);
        std::swap(originalElementVal, V3[0]);
    };

    vectorSliceTest(math::ChannelColumnRowTensor<ElementType>{});
    vectorSliceTest(math::ColumnRowChannelTensor<ElementType>{});
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGetPrimarySlice(){}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorReferenceAsVector()
{
    math::ChannelColumnRowTensor<ElementType> T(3, 4, 2);
    T(0, 0, 0) = 1;
    T(0, 0, 1) = 2;
    T(0, 1, 0) = 3;
    T(0, 1, 1) = 4;
    math::ColumnRowChannelTensor<ElementType> S(T);

    auto u = T.ReferenceAsVector();
    auto v = S.ReferenceAsVector();

    math::RowVector<ElementType> r1{ 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    math::RowVector<ElementType> r2{ 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    testing::ProcessTest("TensorReference::ReferenceAsVector()", u == r1 && v == r2);
}

template<typename ElementType>
void TestTensorReferenceAsMatrix()
{
    math::ChannelColumnRowTensor<ElementType> T(3, 4, 2);
    T(0, 0, 0) = 1;
    T(0, 0, 1) = 2;
    T(0, 1, 0) = 3;
    T(0, 1, 1) = 4;
    math::ColumnRowChannelTensor<ElementType> S(T);

    auto M = T.ReferenceAsMatrix();
    auto N = S.ReferenceAsMatrix();

    math::RowMatrix<ElementType> R1
    {
        { 1, 2, 3, 4, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    math::RowMatrix<ElementType> R2
    {
        { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    testing::ProcessTest("TensorReference::ReferenceAsMatrix", M == R1 && N == R2);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorCopyFrom()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    math::Tensor<ElementType, dimension0, dimension1, dimension2> S(2, 3, 4);
    S.CopyFrom(T);

    math::Tensor<ElementType, math::Dimension::column, math::Dimension::row, math::Dimension::channel> S2(2, 3, 4);
    S2.CopyFrom(T);

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    auto N = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 5,6 },{ 9,0 } },
        { { 4,5 },{ 7,8 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }).CopyFrom(N);

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,5,6 },{ 9,0,9,0 } },
        { { 3,4,5,6 },{ 7,8,4,5 },{ 1,2,7,8 } }
    };

    testing::ProcessTest("TensorReference::CopyFrom", S == T &&  S2 == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorReset()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T.Reset();

    math::Tensor<ElementType, dimension0, dimension1, dimension2> S(2, 3, 4);

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }).Reset();

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,0,0 },{ 9,0,0,0 } },
        { { 3,4,5,6 },{ 7,8,0,0 },{ 1,2,0,0 } }
    };

    testing::ProcessTest("TensorReference::Reset", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorFill()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T.Fill (3);

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 3,3,3,3 },{ 3,3,3,3 },{ 3,3,3,3 } },
        { { 3,3,3,3 },{ 3,3,3,3 },{ 3,3,3,3 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }).Fill(3);

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,3,3 },{ 9,0,3,3 } },
        { { 3,4,5,6 },{ 7,8,3,3 },{ 1,2,3,3 } }
    };

    testing::ProcessTest("TensorReference::Fill", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorGenerate()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T.Generate([]()->ElementType {return 3; });

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 3,3,3,3 },{ 3,3,3,3 },{ 3,3,3,3 } },
        { { 3,3,3,3 },{ 3,3,3,3 },{ 3,3,3,3 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }).Generate([]()->ElementType {return 3; });

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,3,3 },{ 9,0,3,3 } },
        { { 3,4,5,6 },{ 7,8,3,3 },{ 1,2,3,3 } }
    };

    testing::ProcessTest("TensorReference::Generate", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorTransform()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T.Transform([](ElementType x) {return 2*x; });

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 2,4,6,8 },{ 10,12,14,16 },{ 18,0,2,4 } },
        { { 6,8,10,12 },{ 14,16,18,0 },{ 2,4,6,8 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }).Transform([](ElementType x) {return 2 * x; });

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,14,16 },{ 9,0,2,4 } },
        { { 3,4,5,6 },{ 7,8,18,0 },{ 1,2,6,8 } }
    };

    testing::ProcessTest("TensorReference::Transform", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorPlusEqualsOperator()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T += 2;

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 3,4,5,6 },{ 7,8,9,10 },{ 11,2,3,4 } },
        { { 5,6,7,8 },{ 9,10,11,2 },{ 3,4,5,6 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }) += 2;

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,9,10 },{ 9,0,3,4 } },
        { { 3,4,5,6 },{ 7,8,11,2 },{ 1,2,5,6 } }
    };

    testing::ProcessTest("TensorReference::operator+=", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorMinusEqualsOperator()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T -= -2;

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 3,4,5,6 },{ 7,8,9,10 },{ 11,2,3,4 } },
        { { 5,6,7,8 },{ 9,10,11,2 },{ 3,4,5,6 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }) -= -2;

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,9,10 },{ 9,0,3,4 } },
        { { 3,4,5,6 },{ 7,8,11,2 },{ 1,2,5,6 } }
    };

    testing::ProcessTest("TensorReference::operator-=", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorTimesEqualsOperator()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T *= 2;

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 2,4,6,8 },{ 10,12,14,16 },{ 18,0,2,4 } },
        { { 6,8,10,12 },{ 14,16,18,0 },{ 2,4,6,8 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }) *= 2;

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,14,16 },{ 9,0,2,4 } },
        { { 3,4,5,6 },{ 7,8,18,0 },{ 1,2,6,8 } }
    };

    testing::ProcessTest("TensorReference::operator*=", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensorDivideEqualsOperator()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    T /=0.5;

    auto S = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 2,4,6,8 },{ 10,12,14,16 },{ 18,0,2,4 } },
        { { 6,8,10,12 },{ 14,16,18,0 },{ 2,4,6,8 } }
    };

    auto M = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,7,8 },{ 9,0,1,2 } },
        { { 3,4,5,6 },{ 7,8,9,0 },{ 1,2,3,4 } }
    };

    M.GetSubTensor({ 0,1,2 }, { 2,2,2 }) /= 0.5;

    auto R = math::Tensor<ElementType, dimension0, dimension1, dimension2>
    {
        { { 1,2,3,4 },{ 5,6,14,16 },{ 9,0,2,4 } },
        { { 3,4,5,6 },{ 7,8,18,0 },{ 1,2,6,8 } }
    };

    testing::ProcessTest("TensorReference::operator/=", S == T && M == R);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorAddUpdate()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);

    auto v1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    math::AddUpdate<math::Dimension::row, implementation>(v1, T);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 }, { 1,1,1,1 }, { 1,1,1,1 } },
                                                                             { { 2,2,2,2 }, { 2,2,2,2 }, { 2,2,2,2 } } };
    testing::ProcessTest("void TestTensorVectorAddUpdate()", T == R1);

    T.Fill(0);
    auto v2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    math::AddUpdate<math::Dimension::column, implementation>(v2, T);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } },
                                                                             { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } } };
    testing::ProcessTest("void TestTensorVectorAddUpdate()", T == R2);

    T.Fill(0);
    auto v3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    math::AddUpdate<math::Dimension::channel, implementation>(v3, T);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
                                                                             { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } } };
    testing::ProcessTest("void TestTensorVectorAddUpdate()", T == R3);

    // subtensors
    auto TT = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto TR = TT.GetSubTensor({ 5,3,1 }, {2,3,4});

    TR.Fill(0);
    math::AddUpdate<math::Dimension::row, implementation>(v1, TR);
    testing::ProcessTest("void TestTensorVectorAddUpdate() with subtensor", TR == R1);

    TR.Fill(0);
    math::AddUpdate<math::Dimension::column, implementation>(v2, TR);
    testing::ProcessTest("void TestTensorVectorAddUpdate() with subtensor", TR == R2);

    TR.Fill(0);
    math::AddUpdate<math::Dimension::channel, implementation>(v3, TR);
    testing::ProcessTest("void TestTensorVectorAddUpdate() with subtensor", TR == R3);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorMultiply()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    auto T1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T1.Fill(1);
    auto v1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    math::ScaleUpdate<math::Dimension::row, implementation>(v1, T1);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } },
                                                                             { { 2,2,2,2 },{ 2,2,2,2 },{ 2,2,2,2 } } };

    auto T2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T2.Fill(1);
    auto v2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    math::ScaleUpdate<math::Dimension::column, implementation>(v2, T2);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } },
                                                                             { { 1,1,1,1 },{ 2,2,2,2 },{ 3,3,3,3 } } };

    auto T3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T3.Fill(1);
    auto v3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    math::ScaleUpdate<math::Dimension::channel, implementation>(v3, T3);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } },
                                                                             { { 1,2,3,4 },{ 1,2,3,4 },{ 1,2,3,4 } } };

    // subtensors
    auto S1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto M1 = S1.GetSubTensor({ 5,3,1 }, { 2,3,4 });
    M1.Fill(1);
    math::ScaleUpdate<math::Dimension::row, implementation>(v1, M1);

    auto S2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto M2 = S2.GetSubTensor({ 5,3,1 }, { 2,3,4 });
    M2.Fill(1);
    math::ScaleUpdate<math::Dimension::column, implementation>(v2, M2);

    auto S3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto M3 = S3.GetSubTensor({ 5,3,1 }, { 2,3,4 });
    M3.Fill(1);
    math::ScaleUpdate<math::Dimension::channel, implementation>(v3, M3);

    testing::ProcessTest(implementationName + "::Multiply(Vector, Tensor)", T1 == R1 && T2 == R2 && T3 == R3 && M1 == R1 && M2 == R2 && M3 == R3);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2, math::ImplementationType implementation>
void TestTensorVectorScaleAddUpdate()
{
    auto T = math::Tensor<ElementType, dimension0, dimension1, dimension2>(2, 3, 4);
    T.Fill(1);
    auto s1 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2 };
    auto b1 = math::Vector<ElementType, math::VectorOrientation::row>{ 3,4 };
    math::ScaleAddUpdate<math::Dimension::row, implementation>(s1, b1, T);
    auto R1 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 4,4,4,4 },{ 4,4,4,4 },{ 4,4,4,4 } },
                                                                             { { 6,6,6,6 },{ 6,6,6,6 },{ 6,6,6,6 } } };
    testing::ProcessTest("void TestTensorVectorScaleAddUpdate()", T == R1);

    T.Fill(1);
    auto s2 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3 };
    auto b2 = math::Vector<ElementType, math::VectorOrientation::row>{ 4,5,6 };
    math::ScaleAddUpdate<math::Dimension::column, implementation>(s2, b2, T);
    auto R2 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 5,5,5,5 },{ 7,7,7,7 },{ 9,9,9,9 } },
                                                                             { { 5,5,5,5 },{ 7,7,7,7 },{ 9,9,9,9 } } };
    testing::ProcessTest("void TestTensorVectorScaleAddUpdate()", T == R2);

    T.Fill(1);
    auto s3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,2,3,4 };
    auto b3 = math::Vector<ElementType, math::VectorOrientation::row>{ 1,1,2,2 };
    math::ScaleAddUpdate<math::Dimension::channel, implementation>(s3, b3, T);
    auto R3 = math::Tensor<ElementType, dimension0, dimension1, dimension2>{ { { 2,3,5,6 },{ 2,3,5,6 },{ 2,3,5,6 } },
                                                                             { { 2,3,5,6 },{ 2,3,5,6 },{ 2,3,5,6 } } };
    testing::ProcessTest("void TestTensorVectorScaleAddUpdate()", T == R3);

    // subtensors
    auto TT = math::Tensor<ElementType, dimension0, dimension1, dimension2>(10, 10, 10);
    auto TR = TT.GetSubTensor({ 5,3,1 }, { 2,3,4 });

    TR.Fill(1);
    math::ScaleAddUpdate<math::Dimension::row, implementation>(s1, b1, TR);
    testing::ProcessTest("void TestTensorVectorScaleAddUpdate() with subtensor", TR == R1);

    TR.Fill(1);
    math::ScaleAddUpdate<math::Dimension::column, implementation>(s2, b2, TR);
    testing::ProcessTest("void TestTensorVectorScaleAddUpdate() with subtensor", TR == R2);

    TR.Fill(1);
    math::ScaleAddUpdate<math::Dimension::channel, implementation>(s3, b3, TR);
    testing::ProcessTest("void TestTensorVectoScaleAddUpdate() with subtensor", TR == R3);
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

