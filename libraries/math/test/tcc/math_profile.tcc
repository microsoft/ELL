////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     math_profile.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include "VectorOperations.h"
#include "MatrixOperations.h"

// stl
#include <chrono>
#include <iostream>
#include <random>

// utilities
#include "RandomEngines.h"


using namespace ell;

template<typename Function>
double GetTime(Function function, size_t repetitions)
{
    // warm up
    function();
    function();
    function();

    // timed reps
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t t = 0; t < repetitions; ++t)
    {
        function();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
    return static_cast<double>(duration);
}

void PrintLine(std::string functionName, double native, double singleBlas, double multiBlas)
{
    std::cout << functionName 
        <<"\tnative:1.0\tsingleBlas:" << singleBlas / native
        <<"\tmultiBlas:" << multiBlas / native
        << std::endl;
}

template <typename ElementType, typename VectorAType, math::VectorOrientation orientation>
void ProfileVectorScaleAddWorker(ElementType scalarA, VectorAType vectorA, ElementType scalarB, math::VectorReference<ElementType, orientation> vectorB, std::string description, size_t repetitions)
{
    double native = GetTime([&]() { math::ScaleAddUpdate<math::ImplementationType::native>(scalarA, vectorA, scalarB, vectorB); }, repetitions);
    math::Blas::SetNumThreads(0);
    double multiBlas = GetTime([&]() { math::ScaleAddUpdate<math::ImplementationType::openBlas>(scalarA, vectorA, scalarB, vectorB); }, repetitions);
    math::Blas::SetNumThreads(1);
    double singleBlas = GetTime([&]() { math::ScaleAddUpdate<math::ImplementationType::openBlas>(scalarA, vectorA, scalarB, vectorB); }, repetitions);

    std::string type = std::string("<") + typeid(ElementType).name() + ">";
    PrintLine("ScaleAddUpdate" + type + "(" + description + ", vector)", native, singleBlas, multiBlas);
}

template<typename ElementType>
void ProfileVectorScaleAdd(size_t size, size_t repetitions, std::string seed)
{
    auto engine = utilities::GetRandomEngine(seed);
    std::uniform_real_distribution<ElementType> uniform(-1, 1);
    auto generator = [&]() { return uniform(engine); };

    math::RowVector<ElementType> v(size);
    v.Generate(generator);

    math::RowVector<ElementType> u(size);
    u.Generate(generator);

    ElementType scalar = static_cast<ElementType>(-7.3);
    ElementType one = 1.0;

    ProfileVectorScaleAddWorker(scalar, math::OnesVector(), one, u, "scalar, ones, one", repetitions);
    ProfileVectorScaleAddWorker(one, v, one, u, "one, vector, one", repetitions);
    ProfileVectorScaleAddWorker(scalar, v, one, u, "scalar, vector, one", repetitions);
    ProfileVectorScaleAddWorker(scalar, math::OnesVector(), scalar, u, "scalar, ones, scalar", repetitions);
    ProfileVectorScaleAddWorker(one, v, scalar, u, "one, vector, scalar", repetitions);
    ProfileVectorScaleAddWorker(scalar, v, scalar, u, "scalar, vector, scalar", repetitions);
}

template <typename ElementType>
void ProfileVectorInner(size_t size, size_t repetitions, std::string seed)
{
    auto engine = utilities::GetRandomEngine(seed);
    std::uniform_real_distribution<ElementType> uniform(-1, 1);
    auto generator = [&]() { return uniform(engine); };

    math::RowVector<ElementType> u(size);
    u.Generate(generator);

    math::ColumnVector<ElementType> v(size);
    v.Generate(generator);

    ElementType result;
    double native = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::native>::InnerProduct(u, v, result); }, repetitions);
    math::Blas::SetNumThreads(1);
    double singleBlas = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::openBlas>::InnerProduct(u, v, result); }, repetitions);
    math::Blas::SetNumThreads(0);
    double multiBlas = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::openBlas>::InnerProduct(u, v, result); }, repetitions);

    std::string type = std::string("<") + typeid(ElementType).name() + ">";
    std::string vector = "Vector" + type + "[" + std::to_string(size) + "]";
    PrintLine("Dot(" + vector + ", " + vector + ")", native, singleBlas, multiBlas);
}

template <typename ElementType, math::MatrixLayout layout>
void ProfileVectorOuter(size_t size, size_t repetitions, std::string seed)
{
    auto engine = utilities::GetRandomEngine(seed);
    std::uniform_real_distribution<ElementType> uniform(-1, 1);
    auto generator = [&]() { return uniform(engine); };

    math::ColumnVector<ElementType> u(size);
    u.Generate(generator);

    math::RowVector<ElementType> v(size);
    v.Generate(generator);

    math::Matrix<ElementType, layout> S(size, size);

    double native = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::native>::OuterProduct(u, v, S); }, repetitions);
    math::Blas::SetNumThreads(1);
    double singleBlas = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::openBlas>::OuterProduct(u, v, S); }, repetitions);
    math::Blas::SetNumThreads(0);
    double multiBlas = GetTime([&]() { math::Internal::VectorOperations<math::ImplementationType::openBlas>::OuterProduct(u, v, S); }, repetitions);

    std::string type = std::string("<") + typeid(ElementType).name() + ">";
    std::string vector = "Vector" + type + "[" + std::to_string(size) + "]";
    std::string functionName = "OuterProduct(" + vector + ", " + vector + ")";
    PrintLine(functionName, native, singleBlas, multiBlas);
}

template <typename ElementType, math::MatrixLayout layout>
void ProfileMatrixVectorMultiplyScaleAddUpdate(size_t numRows, size_t numColumns, size_t repetitions, std::string seed)
{
    auto engine = utilities::GetRandomEngine(seed);
    std::uniform_real_distribution<ElementType> uniform(-1, 1);
    auto generator = [&]() { return uniform(engine); };

    math::Matrix<ElementType, layout> M(numRows, numColumns);
    M.Generate(generator);

    math::ColumnVector<ElementType> v(numColumns);
    v.Generate(generator);

    math::ColumnVector<ElementType> u(numRows);
    u.Generate(generator);

    auto s = generator();
    auto t = generator();

    double native = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::native>::MultiplyScaleAddUpdate(s, M, v, t, u); }, repetitions);
    math::Blas::SetNumThreads(1);
    double singleBlas = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::openBlas>::MultiplyScaleAddUpdate(s, M, v, t, u); }, repetitions);
    math::Blas::SetNumThreads(0);
    double multiBlas = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::openBlas>::MultiplyScaleAddUpdate(s, M, v, t, u); }, repetitions);

    std::string type = std::string("<") + typeid(ElementType).name() + ">";
    std::string vector1 = "Vector" + type + "[" + std::to_string(numColumns) + "]";
    std::string vector2 = "Vector" + type + "[" + std::to_string(numRows) + "]";
    std::string matrix = "Matrix" + type + "[" + std::to_string(numRows) + ", " + std::to_string(numColumns) + "]";
    std::string functionName = "MultiplyScaleAddUpdate(scalar, " + matrix + ", " + vector1 + ", scalar, " +vector2 + ")";
    PrintLine(functionName, native, singleBlas, multiBlas);
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void ProfileMatrixMatrixMultiplyScaleAddUpdate(size_t numRows, size_t numColumns, size_t numColumns2, size_t repetitions, std::string seed)
{
    auto engine = utilities::GetRandomEngine(seed);
    std::uniform_real_distribution<ElementType> uniform(-1, 1);
    auto generator = [&]() { return uniform(engine); };

    math::Matrix<ElementType, layout1> M(numRows, numColumns);
    M.Generate(generator);

    math::Matrix<ElementType, layout2> N(numColumns, numColumns2);
    N.Generate(generator);

    math::Matrix<ElementType, layout1> T(numRows, numColumns2);
    T.Generate(generator);

    auto a = generator();
    auto b = generator();

    double native = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::native>::MultiplyScaleAddUpdate(a, M, N, b, T); }, repetitions);
    math::Blas::SetNumThreads(1);
    double singleBlas = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::openBlas>::MultiplyScaleAddUpdate(a, M, N, b, T); }, repetitions);
    math::Blas::SetNumThreads(0);
    double multiBlas = GetTime([&]() { math::Internal::MatrixOperations<math::ImplementationType::openBlas>::MultiplyScaleAddUpdate(a, M, N, b, T); }, repetitions);

    std::string type = std::string("<") + typeid(ElementType).name() + ">";
    std::string matrix1 = "Matrix" + type + "[" + std::to_string(numRows) + ", " + std::to_string(numColumns) + "]";
    std::string matrix2 = "Matrix" + type + "[" + std::to_string(numColumns) + ", " + std::to_string(numColumns2) + "]";
    std::string matrix3 = "Matrix" + type + "[" + std::to_string(numRows) + ", " + std::to_string(numColumns2) + "]";
    std::string functionName = "MultiplyScaleAddUpdate(scalar, " + matrix1 + ", " + matrix2 + ", scalar, " + matrix3 + ")";
    PrintLine(functionName, native, singleBlas, multiBlas);
}
