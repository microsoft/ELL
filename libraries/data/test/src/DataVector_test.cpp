////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataVector_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector_test.h"

// data
#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"
#include "AutoDataVector.h"
#include "DataVector.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>

namespace emll
{
    template<typename DataVectorType>
    void IDataVectorTest()
    {
        DataVectorType u{ { 0,12 },{ 3,-7 },{ 4,1 } };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Norm2()", testing::IsEqual(u.Norm2(), std::sqrt(12 * 12 + 7 * 7 + 1 * 1)));

        std::vector<double> w{ 1, 1, 1, 1, 1, 1 };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Dot()", testing::IsEqual(u.Dot(w.data()), 12.0 - 7.0 + 1.0));

        u.AddTo(w.data(), 2);
        std::vector<double> z{ 25, 1, 1, -13, 3, 1 };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::AddTo()", testing::IsEqual(w, z));

        std::stringstream ss;
        u.Print(ss);
        auto sss = ss.str();
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Print()", sss == "0:12\t3:-7\t4:1");
    }

    template<typename DataVectorType>
    void IDataVectorBinaryTest()
    {
        DataVectorType u{ { 0,1 },{ 3,1 },{ 4,1 } };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Norm2()", testing::IsEqual(u.Norm2(), std::sqrt(3)));

        std::vector<double> w{ 1, 2, 3, 4, 5, 6 };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Dot()", testing::IsEqual(u.Dot(w.data()), 1.0 + 4.0 + 5.0));

        u.AddTo(w.data(), 2);
        std::vector<double> z{ 3, 2, 3, 6, 7, 6 };
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::AddTo()", testing::IsEqual(w, z));

        std::stringstream ss;
        u.Print(ss);
        auto sss = ss.str();
        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Print()", sss == "0:1\t3:1\t4:1");
    }

    void IDataVectorTests()
    {
        IDataVectorTest<data::DoubleDataVector>();
        IDataVectorTest<data::FloatDataVector>();
        IDataVectorTest<data::ShortDataVector>();
        IDataVectorTest<data::ByteDataVector>();
        IDataVectorTest<data::SparseDoubleDataVector>();
        IDataVectorTest<data::SparseFloatDataVector>();
        IDataVectorTest<data::SparseShortDataVector>();
        IDataVectorTest<data::SparseByteDataVector>();
        IDataVectorTest<data::AutoDataVector>();

        IDataVectorBinaryTest<data::DoubleDataVector>();
        IDataVectorBinaryTest<data::FloatDataVector>();
        IDataVectorBinaryTest<data::ShortDataVector>();
        IDataVectorBinaryTest<data::ByteDataVector>();
        IDataVectorBinaryTest<data::SparseDoubleDataVector>();
        IDataVectorBinaryTest<data::SparseFloatDataVector>();
        IDataVectorBinaryTest<data::SparseShortDataVector>();
        IDataVectorBinaryTest<data::SparseByteDataVector>();
        IDataVectorBinaryTest<data::AutoDataVector>();
        IDataVectorBinaryTest<data::SparseBinaryDataVector>();
    }

    template <typename DataVectorType1, typename DataVectorType2>
    void ToDataVectorTest(std::initializer_list<double> list)
    {
        const DataVectorType1 v(list);
        auto u = v.DeepCopyAs<DataVectorType2>();
        auto w = v.ToArray();
        auto z = u.ToArray();

        std::string name1 = typeid(DataVectorType1).name();
        std::string name2 = typeid(DataVectorType2).name();

        testing::ProcessTest(name1 + "::DeepCopyAs<" + name2 + ">", testing::IsEqual(w, z, 1.0e-6));
    }

    enum class InitType { fractional, integral, binary };

    template <typename DataVectorType>
    void ToDataVectorTestDispatch(InitType maxType)
    {
        std::initializer_list<double> binaryInit = { 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 };
        std::initializer_list<double> integeralInit = { 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 };
        std::initializer_list<double> fractionalInit = { 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 };
        if (maxType == InitType::binary) integeralInit = binaryInit;
        if (maxType != InitType::fractional) fractionalInit = integeralInit;

        ToDataVectorTest<DataVectorType, data::AutoDataVector>(fractionalInit);
        ToDataVectorTest<DataVectorType, data::DoubleDataVector>(fractionalInit);
        ToDataVectorTest<DataVectorType, data::FloatDataVector>(fractionalInit);
        ToDataVectorTest<DataVectorType, data::ShortDataVector>(integeralInit);
        ToDataVectorTest<DataVectorType, data::ByteDataVector>(integeralInit);
        ToDataVectorTest<DataVectorType, data::SparseDoubleDataVector>(fractionalInit);
        ToDataVectorTest<DataVectorType, data::SparseFloatDataVector>(fractionalInit);
        ToDataVectorTest<DataVectorType, data::SparseShortDataVector>(integeralInit);
        ToDataVectorTest<DataVectorType, data::SparseByteDataVector>(integeralInit);
        ToDataVectorTest<DataVectorType, data::SparseBinaryDataVector>(binaryInit);
    }

    void ToDataVectorTests()
    {
        ToDataVectorTestDispatch<data::AutoDataVector>(InitType::fractional);
        ToDataVectorTestDispatch<data::DoubleDataVector>(InitType::fractional);
        ToDataVectorTestDispatch<data::FloatDataVector>(InitType::fractional);
        ToDataVectorTestDispatch<data::ShortDataVector>(InitType::integral);
        ToDataVectorTestDispatch<data::ByteDataVector>(InitType::integral);
        ToDataVectorTestDispatch<data::SparseDoubleDataVector>(InitType::fractional);
        ToDataVectorTestDispatch<data::SparseFloatDataVector>(InitType::fractional);
        ToDataVectorTestDispatch<data::SparseShortDataVector>(InitType::integral);
        ToDataVectorTestDispatch<data::SparseByteDataVector>(InitType::integral);
        ToDataVectorTestDispatch<data::SparseBinaryDataVector>(InitType::binary);
    }

    void AutoDataVectorTest()
    {
        data::AutoDataVector v1{ 0.123456789, 1.12345678901, 2.3456789012, 3.4567890123 };
        testing::ProcessTest("AutoDataVector ctor", v1.GetInternalType() == data::IDataVector::Type::DoubleDataVector);

        data::AutoDataVector v2{ 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f };
        testing::ProcessTest("AutoDataVector ctor", v2.GetInternalType() == data::IDataVector::Type::FloatDataVector);

        data::AutoDataVector v3{ 1234, 2345, 3456, 4567, 5678, 6789 };
        testing::ProcessTest("AutoDataVector ctor", v3.GetInternalType() == data::IDataVector::Type::ShortDataVector);

        data::AutoDataVector v4{ 10, 20, 30, 40, 50, 60, 70 };
        testing::ProcessTest("AutoDataVector ctor", v4.GetInternalType() == data::IDataVector::Type::ByteDataVector);

        data::AutoDataVector v5{ 0, 0, 0, 0, 0, 1.2345678901, 0, 0, 0 };
        testing::ProcessTest("AutoDataVector ctor", v5.GetInternalType() == data::IDataVector::Type::SparseDoubleDataVector);

        data::AutoDataVector v6{ 0, 0, 0, 0, 0, 1.2f, 0, 0, 0 };
        testing::ProcessTest("AutoDataVector ctor", v6.GetInternalType() == data::IDataVector::Type::SparseFloatDataVector);

        data::AutoDataVector v7{ 0, 0, 0, 0, 0, 1234, 0, 0, 0 };
        testing::ProcessTest("AutoDataVector ctor", v7.GetInternalType() == data::IDataVector::Type::SparseShortDataVector);

        data::AutoDataVector v8{ 0, 0, 0, 0, 0, 10, 0, 0, 0 };
        testing::ProcessTest("AutoDataVector ctor", v8.GetInternalType() == data::IDataVector::Type::SparseByteDataVector);

        data::AutoDataVector v9{ 0, 0, 0, 0, 0, 1, 0, 0, 0 };
        testing::ProcessTest("AutoDataVector ctor", v9.GetInternalType() == data::IDataVector::Type::SparseBinaryDataVector);
    }
}