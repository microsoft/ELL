////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector_test.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include "VectorOperations.h"

// testing
#include "testing.h"

// utilities
#include "JsonArchiver.h"

// stl
#include <sstream>

template <typename ElementType>
void TestVectorIndexer()
{
    math::RowVector<ElementType> v{ 1,2,3,4,5,6,7 };
    auto u = v.GetSubVector(2, 2);
    
    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    math::ColumnMatrix<ElementType> N(M);
    auto w = M.GetRow(1);
    auto z = N.GetRow(1);

    testing::ProcessTest("Vector::Operator[]", v[0] == 1 && v[1] == 2 && v[6] == 7 && u[0] == 3 && u[1] == 4 && w[0] == 4 && w[1] == 5 && w[2] == 6 && z[0] == 4 && z[1] == 5 && z[2] == 6);
}

template <typename ElementType>
void TestVectorSize()
{
    math::RowVector<ElementType> u{};
    math::RowVector<ElementType> v{ 1,2,3,4,5,6,7 };
    auto w = v.GetSubVector(2, 3);

    testing::ProcessTest("Vector::Size", v.Size() == 7 && u.Size() == 0 && w.Size() == 3);
}

template <typename ElementType>
void TestVectorGetDataPointer()
{
    math::RowVector<ElementType> v{ 1,2,3,4,5,6,7 };
    auto u = v.GetSubVector(2, 2);

    testing::ProcessTest("Vector::GetDataPointer", &(v[0]) == v.GetDataPointer() && v.GetDataPointer() + 2 == u.GetDataPointer());
}

template <typename ElementType>
void TestVectorGetIncrement()
{
    math::RowVector<ElementType> v{ 1,2,3,4,5,6,7 };
    auto u = v.GetSubVector(2, 2);
    
    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    math::ColumnMatrix<ElementType> N(M);
    auto w = M.GetRow(1);
    auto z = N.GetRow(1);
    
    testing::ProcessTest("Vector::GetIncrement", v.GetIncrement() == 1 && u.GetIncrement() == 1 && w.GetIncrement() == 1 && z.GetIncrement() == 3);
}

template <typename ElementType>
void TestVectorNorm0()
{
    math::RowVector<ElementType> x{ 0, 1, 0, -2, 0 };
    auto v = x.GetSubVector(2, 2);

    testing::ProcessTest("Vector::Norm0", x.Norm0() == 2 && v.Norm0() == 1);
}

template <typename ElementType>
void TestVectorNorm1()
{
    math::RowVector<ElementType> x{ 0, 1, 0, -2, 0 };
    auto v = x.GetSubVector(2, 2);

    testing::ProcessTest("Vector::Norm1", x.Norm1() == 3 && v.Norm2() == 2);
}

template <typename ElementType>
void TestVectorNorm2()
{
    math::RowVector<ElementType> x{ 0, 1, 0, -2, 0 };
    auto v = x.GetSubVector(2, 2);

    testing::ProcessTest("Vector::Norm2", testing::IsEqual(x.Norm2(), static_cast<ElementType>(std::sqrt(5))) && v.Norm2() == 2);
}

template <typename ElementType>
void TestVectorNorm2Squared()
{
    math::RowVector<ElementType> x{ 0, 1, 0, -2, 0 };
    auto v = x.GetSubVector(2, 2);

    testing::ProcessTest("Vector::Norm2Squared", x.Norm2Squared() == 5 && v.Norm2Squared() == 4);
}

template <typename ElementType>
void TestVectorToArray()
{
    std::vector<ElementType> r0{ 41, 47, 53, 59 };
    std::vector<ElementType> r1{ 15, 25, 23, 33 };

    math::RowVector<ElementType> p(r0);
    math::ColumnVector<ElementType> q(r1);

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> A{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };
    std::vector<ElementType> r(A.GetRow(0).ToArray());
    std::vector<ElementType> s(A.GetRow(2).ToArray());

    math::Matrix<ElementType, math::MatrixLayout::columnMajor> B(A);
    std::vector<ElementType> t(B.GetRow(0).ToArray());
    std::vector<ElementType> u(B.GetRow(2).ToArray());

    testing::ProcessTest("Vector::ToArray", p.ToArray() == r0 && q.ToArray() == r1 && r == r0 && s == r1 && t == r0 && u == r1);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorEqualityOperator()
{
    math::Vector<ElementType, orientation> u{ 1,2,3,4,5 };
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5 };

    testing::ProcessTest("Vector::operator==", u == v);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorInequalityOperator()
{
    math::Vector<ElementType, orientation> u{ 1,2,3,4,5 };
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5 };
    math::Vector<ElementType, orientation> w{ -1,2,3,4,5 };
    math::Vector<ElementType, orientation> z{ 1,2,3,4 };

    testing::ProcessTest("Vector::operator!=", u != w && u != v.Transpose() && u != z);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGetConstReference()
{
    math::Vector<ElementType, orientation> u{ 1,2,3,4,5 };
    auto v = u.GetConstReference();

    testing::ProcessTest("Vector::GetConstReference", u == v);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGetSubVector()
{
    math::Vector<ElementType, orientation> u{ 1,2,3,4,5 };
    auto v = u.GetSubVector(2, 2);

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> A{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };
    auto w = A.GetColumn(2);
    auto z = w.GetSubVector(1, 2);

    testing::ProcessTest("Vector::GetSubVector", v[0] == 3 && v[1] == 4 && z[0] == 56 && z[1] == 23);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTranspose()
{
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5,6,7 };
    auto u = v.Transpose();
    math::Vector<ElementType, math::TransposeVectorOrientation<orientation>::value> w{ 1,2,3,4,5,6,7 };

    auto x = v.GetSubVector(2,3).Transpose();
    math::Vector<ElementType, math::TransposeVectorOrientation<orientation>::value> z{ 3,4,5 };

    testing::ProcessTest("Vector::Transpose", u == w && x == z);
}

template <typename ElementType>
void TestVectorSwap()
{
    math::RowVector<ElementType> v{ 1,2,3,4,5,6,7 };
    math::RowVector<ElementType> u{ -1,-2,-3,-4,-5,-6,-7 };
    math::RowVector<ElementType> s{ -1,-2,-3,-4,-5,-6,-7 };
    math::RowVector<ElementType> t{ 1,2,3,4,5,6,7 };
    v.Swap(u);

    testing::ProcessTest("Vector::Swap", v == s && u == t);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorCopyFrom()
{
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5,6,7 };
    math::Vector<ElementType, orientation> u(7);
    u.CopyFrom(v);

    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    math::ColumnVector<ElementType> x{ 11,12,13 };
    M.GetColumn(1).CopyFrom(x);
    math::RowMatrix<ElementType> R{ { 1, 11, 3 },{ 4,12,6 },{ 7,13,9 } };

    testing::ProcessTest("Vector::CopyFrom", u == v && M == R);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorReset()
{
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5,6,7 };
    v.GetSubVector(1, 2).Reset();
    math::Vector<ElementType, orientation> r{ 1,0,0,4,5,6,7 };

    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    M.GetColumn(1).Reset();
    M.GetRow(1).Reset();
    math::RowMatrix<ElementType> R{ { 1, 0, 3 },{ 0, 0, 0 },{ 7, 0, 9 } };

    testing::ProcessTest("Vector::Reset", v == r && M == R);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorFill()
{
    math::Vector<ElementType, orientation> v(10);
    v.Fill(2);
    math::Vector<ElementType, orientation> r{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    M.GetColumn(1).Fill(-1);
    M.GetRow(1).Fill(1);
    math::RowMatrix<ElementType> R{ { 1, -1, 3 },{ 1, 1, 1 },{ 7, -1, 9 } };

    testing::ProcessTest("Vector::Fill", v == r && M == R);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorGenerate()
{
    math::Vector<ElementType, orientation> v{ 1, 2, 3, 4, 5, 6, 7 };
    v.GetSubVector(1, 2).Generate([]()->ElementType {return -1; });
    math::Vector<ElementType, orientation> r{ 1, -1, -1, 4, 5, 6, 7 };

    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    M.GetColumn(1).Generate([]()->ElementType {return -1.0; });
    M.GetRow(1).Generate([]()->ElementType {return 1.0; });
    math::RowMatrix<ElementType> R{ { 1, -1, 3 },{ 1, 1, 1 },{ 7, -1, 9 } };

    testing::ProcessTest("Vector::Generate", v == r && M == R);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTransform()
{
    math::Vector<ElementType, orientation> v{ 1,2,3,4,5,6,7 };
    v.Transform([](ElementType value) {return value * 2; });
    math::Vector<ElementType, orientation> u{ 2,4,6,8,10,12,14 };
    
    math::RowMatrix<ElementType> M{ { 1, 2, 3 },{ 4,5,6 },{ 7,8,9 } };
    M.GetColumn(1).Transform([](ElementType value) {return value * 2; });
    math::ColumnVector<ElementType> w{ 4,10,16 };

    math::Vector<ElementType, orientation> z{ 1,-2,3,-4,-5,6,-7 };
    z.Transform(math::AbsoluteValueTransformation<ElementType>);
    math::Vector<ElementType, orientation> y{ 1,2,3,4,5,6,7 };

    testing::ProcessTest("Vector::Transform", v == u && M.GetColumn(1) == w && z == y);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorResize()
{
    math::Vector<ElementType, orientation> v{ 1, 2, 3, 4, 5, 6, 7 };
    v.Resize(3);
    math::Vector<ElementType, orientation> r{ 1, 2, 3 };

    math::Vector<ElementType, orientation> u{ 1, 2, 3, 4, 5, 6, 7 };
    u.Resize(10);
    math::Vector<ElementType, orientation> s{ 1, 2, 3, 4, 5, 6, 7, 0, 0, 0 };

    testing::ProcessTest("Vector::Resize", v.Size() == 3 && v == r && u == s);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorPrint()
{
    std::stringstream stream;
    math::Vector<ElementType, orientation> u{ 0, 2, 0, 4, 0, 0, 0 };
    math::Print(u, stream);
    auto x = stream.str();

    testing::ProcessTest("Print(Vector)", stream.str() == "{ 0, 2, 0, 4, 0, 0, 0 }");
}

template <typename ElementType, math::VectorOrientation orientation>
void TestScalarVectorMultiply()
{
    math::Vector<ElementType, orientation> u{ 1, 2, 3, 4, 5 };
    math::Vector<ElementType, orientation> v{ 2, 0, -1, 0, 1 };
    u += 2 * v;
    math::Vector<ElementType, orientation> r{ 5, 2, 1, 4, 7 };

    testing::ProcessTest("scalar * Vector", u == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorSquare()
{
    math::Vector<ElementType, orientation> u(5);
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    u += Square(v);
    math::Vector<ElementType, orientation> r{ 1, 1, 4, 4, 9 };

    testing::ProcessTest("Square(Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorSqrt()
{
    math::Vector<ElementType, orientation> u(5);
    math::Vector<ElementType, orientation> v{ 1, 1, 4, 4, 9 };
    u += Sqrt(v);
    math::Vector<ElementType, orientation> r{ 1, 1, 2, 2, 3 };

    math::Vector<ElementType, orientation> w{ 1, 1, 4, 4, 9 };
    math::TransformUpdate(math::SquareRootTransformation<ElementType>, w);

    testing::ProcessTest("Sqrt(Vector)", testing::IsEqual(u.ToArray(), r.ToArray()) && testing::IsEqual(w.ToArray(), r.ToArray()));
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorAbs()
{
    math::Vector<ElementType, orientation> u(5);
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    u += Abs(v);
    math::Vector<ElementType, orientation> r{ 1, 1, 2, 2, 3 };

    math::Vector<ElementType, orientation> w{ 1, -1, 2, -2, 3 };
    math::TransformUpdate(math::AbsoluteValueTransformation<ElementType>, w);

    testing::ProcessTest("Abs(Vector)", w == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorPlusEqualsOperator()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    v += -2;
    math::Vector<ElementType, orientation> r{ -1, -3, 0, -4, 1 };

    testing::ProcessTest("Add(scalar, Vector)", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorMinusEqualsOperator()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    v -= 2;
    math::Vector<ElementType, orientation> r{ -1, -3, 0, -4, 1 };

    testing::ProcessTest("Add(scalar, Vector)", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorTimesEqualsOperator()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    v *= -2;
    math::Vector<ElementType, orientation> r{ -2, 2, -4, 4, -6 };

    testing::ProcessTest("Vector::operator*=", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorDivideEqualsOperator()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    v /= -0.5;
    math::Vector<ElementType, orientation> r{ -2, 2, -4, 4, -6 };

    testing::ProcessTest("Vector::operator/=", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorElementwiseMultiplySet()
{
    math::Vector<ElementType, orientation> u{ 1, 2, 3, 4, 5 };
    math::Vector<ElementType, orientation> v{ 2, 0, -1, 0, 1 };
    math::Vector<ElementType, orientation> w(5);
    math::ElementwiseMultiplySet(u, v, w);
    math::Vector<ElementType, orientation> r{ 2, 0, -3, 0, 5 };

    testing::ProcessTest("ElementwiseMultiplySet(Vector, Vector)", w == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorVectorDot()
{
    math::Vector<ElementType, orientation> u{ 1, 2, 3, 4, 5 };
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2, 3 };
    auto result = math::Dot(u, v);

    testing::ProcessTest("Dot(Vector, Vector)", result == 12);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestVectorVectorOuter()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    math::ColumnVector<ElementType> u{ 1, 2, 3 };
    math::RowVector<ElementType> v{ 1, -1 };
    math::Matrix<ElementType, layout> A(3, 2);

    math::OuterProduct<implementation>(u, v, A);

    math::ColumnMatrix<ElementType> B{ {1, -1}, {2, -2}, {3, -3} };
    testing::ProcessTest(implementationName + "::OuterProduct(Vector, Vector)", A == B);
}

template <typename ElementType, math::ImplementationType implementation>
void TestVectorVectorInner()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    math::RowVector<ElementType> u{ 1, 2, 3, 4, 5 };
    math::ColumnVector<ElementType> v{ 1, -1, 2, -2, 3 };
    ElementType result;
    math::InnerProduct<implementation>(u, v, result);

    testing::ProcessTest(implementationName + "::InnerProduct(Vector, Vector)", result == 12);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddUpdateScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::AddUpdate(a, u);

    math::Vector<ElementType, orientation> w{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(a, math::OnesVector(), static_cast<ElementType>(1), w);

    math::Vector<ElementType, orientation> r{ 1, 3, 4, 4 };
    testing::ProcessTest(implementationName + "::AddUpdate(scalar, Vector)", u == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddUpdateVector()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::AddUpdate<implementation>(v, u);

    math::Vector<ElementType, orientation> w{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(1), v, static_cast<ElementType>(1), w);

    math::Vector<ElementType, orientation> r{ -1, -1, 3, -1 };
    testing::ProcessTest(implementationName + "::AddUpdate(Vector, Vector)", u == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);
    math::AddSet<implementation>(a, u, z);

    math::Vector<ElementType, orientation> w(4);
    math::ScaleAddSet<implementation>(a, math::OnesVector(), static_cast<ElementType>(1), u, w);

    math::Vector<ElementType, orientation> r{ 1, 3, 4, 4 };
    testing::ProcessTest(implementationName + "::AddSet(scalar, Vector, output)", z == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalarZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);
    math::AddSet<implementation>(a, u, z);

    math::Vector<ElementType, orientation> w(4);
    math::ScaleAddSet<implementation>(a, math::OnesVector(), static_cast<ElementType>(1), u, w);

    testing::ProcessTest(implementationName + "::AddSet(0.0, Vector, output)", z == u && w == u);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetScalarOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);
    math::AddSet<implementation>(a, u, z);

    math::Vector<ElementType, orientation> w(4);
    math::ScaleAddSet<implementation>(a, math::OnesVector(), static_cast<ElementType>(1), u, w);

    math::Vector<ElementType, orientation> r{ -1, 1, 2, 2 };
    testing::ProcessTest(implementationName + "::AddSet(1.0, Vector, output)", z == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorAddSetVector()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::AddSet<implementation>(v, u, z);

    math::Vector<ElementType, orientation> w(4);
    math::ScaleAddSet<implementation>(static_cast<ElementType>(1), v, static_cast<ElementType>(1), u, w);

    math::Vector<ElementType, orientation> r{ -1, -1, 3, -1 };
    testing::ProcessTest(implementationName + "::ScaleAddUpdate(1.0, Vector, 1.0, Vector)", z == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdate()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleUpdate<implementation>(b, u);

    math::Vector<ElementType, orientation> r{ -4, 0, 2, 2 };
    testing::ProcessTest(implementationName + "::ScaleUpdate(scalar, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdateZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleUpdate<implementation>(b, u);

    math::Vector<ElementType, orientation> r{ 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "::ScaleUpdate(0.0, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleUpdateOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleUpdate<implementation>(b, u);

    math::Vector<ElementType, orientation> r{ -2, 0, 1, 1 };
    testing::ProcessTest(implementationName + "::ScaleUpdate(1.0, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSet()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleSet<implementation>(a, v, u);

    math::Vector<ElementType, orientation> r{ 3, -3, 6, -6 };
    testing::ProcessTest(implementationName + "::ScaleSet(scalar, Vector, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSetZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ 2, 0, 1, 1 };
    math::ScaleSet<implementation>(a, v, u);

    math::Vector<ElementType, orientation> r{ 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "::ScaleSet(0.0, Vector, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleSetOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleSet<implementation>(a, v, u);

    math::Vector<ElementType, orientation> r{ 1, -1, 2, -2 };
    testing::ProcessTest(implementationName + "::ScaleSet(1.0, Vector, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarVectorOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(a, v, math::One(), u);

    math::Vector<ElementType, orientation> w{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(a, v, static_cast<ElementType>(1), w);

    math::Vector<ElementType, orientation> r{ 1, -3, 7, -5 };
    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, Vector, 1.0, Vector)", u == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarOnesScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(a, math::OnesVector(), b, u);

    math::Vector<ElementType, orientation> r{ -1, 3, 5, 5 };
    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, Ones, scalar, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateOneVectorScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(math::One(), v, b, u);

    math::Vector<ElementType, orientation> w{ -2, 0, 1, 1 };
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(1), v, b, w);

    math::Vector<ElementType, orientation> r{ -3, -1, 4, 0 };
    testing::ProcessTest(implementationName + "::ScaleAddUpdate(1.0, Vector, scalar, Vector)", u == r && w == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddUpdateScalarVectorScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 2, -2 };
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };

    math::ScaleAddUpdate<implementation>(a, v, b, u);

    math::Vector<ElementType, orientation> r{ -1, -3, 8, -4 };
    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, Vector, scalar, Vector)", u == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnes()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ -1, 3, 5, 5 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, ones, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesScalarZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ 3, 3, 3, 3 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, ones, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesScalarOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ 1, 3, 4, 4 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, ones, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ -4, 0, 2, 2 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, ones, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ -3, 1, 3, 3 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, ones, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ -2, 0, 1, 1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, ones, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ 1, 1, 1, 1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, ones, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesOneOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ -1, 1, 2, 2 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, ones, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetOnesZeroZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, math::OnesVector(), b, u, z);

    math::Vector<ElementType, orientation> r{ 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, ones, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVector()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ -1, -3, 5, -1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, Vector, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorScalarZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ 3, -3, 3, -3 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, Vector, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorScalarOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 3.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ 1, -3, 4, -2 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, Vector, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ -4, 0, 2, 2 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, Vector, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneScalar()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 2.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ -3, -1, 3, 1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, Vector, scalar, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ -2, 0, 1, 1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, Vector, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ 1, -1, 1, -1 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, Vector, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorOneOne()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 1.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 1.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ -1, -1, 2, 0 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(1.0, Vector, 1.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation, math::ImplementationType implementation>
void TestVectorScaleAddSetVectorZeroZero()
{
    auto implementationName = math::Internal::VectorOperations<implementation>::GetImplementationName();

    ElementType a = 0.0;
    math::Vector<ElementType, orientation> v{ 1, -1, 1, -1 };
    ElementType b = 0.0;
    math::Vector<ElementType, orientation> u{ -2, 0, 1, 1 };
    math::Vector<ElementType, orientation> z(4);

    math::ScaleAddSet<implementation>(a, v, b, u, z);

    math::Vector<ElementType, orientation> r{ 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "::ScaleAddSet(0.0, Vector, 0.0, Vector, output)", z == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorCumulativeSumUpdate()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 3, 2 };
    math::CumulativeSumUpdate(v);
    math::Vector<ElementType, orientation> r{ 1, 0, 3, 5 };
    testing::ProcessTest("CumulativeSumUpdate(Vector)", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorConsecutiveDifferenceUpdate()
{
    math::Vector<ElementType, orientation> v{ 1, -1, 3, 2 };
    math::ConsecutiveDifferenceUpdate(v);
    math::Vector<ElementType, orientation> r{ 1, -2, 4, -1 };
    testing::ProcessTest("ConsecutiveDifferenceUpdate(Vector)", v == r);
}

template <typename ElementType, math::VectorOrientation orientation>
void TestVectorArchiver()
{
    math::Vector<ElementType, orientation> V{ 1, 2, 3, 4, 5, 5, 4, 3, 2, 1 };

    utilities::SerializationContext context;
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    math::VectorArchiver::Write(V, "test", archiver);
    utilities::JsonUnarchiver unarchiver(strstream, context);

    math::Vector<ElementType, orientation> Va(0);
    math::VectorArchiver::Read(Va, "test", unarchiver);

    testing::ProcessTest("VectorArchiver", Va == V);
}
