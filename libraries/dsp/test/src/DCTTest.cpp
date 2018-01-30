////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCTTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DSPTestData.h"

// dsp
#include "DCT.h"

// math
#include "Matrix.h"
#include "MatrixOperations.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <type_traits>
#include <vector>

using namespace ell;
using namespace math;
using namespace dsp;

// clang-format off
const std::vector<std::vector<double>> dct_precomputed = {
    { 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  },  
    { 0.992708874098f,  0.935016242685f,  0.822983865894f, 0.663122658241f, 0.464723172044f, 0.239315664288f, 6.12323399574e-17f, -0.239315664288f, -0.464723172044f, -0.663122658241f, -0.822983865894f, -0.935016242685f, -0.992708874098f, }, 
    { 0.970941817426f, 0.748510748171f, 0.354604887043f, -0.120536680255f, -0.568064746731f, -0.885456025653f, -1.0f, -0.885456025653f, -0.568064746731f, -0.120536680255f, 0.354604887043f, 0.748510748171f, 0.970941817426f, }, 
    { 0.935016242685f, 0.464723172044f, -0.239315664288f, -0.822983865894f, -0.992708874098f, -0.663122658241f, -1.83697019872e-16f, 0.663122658241f, 0.992708874098f, 0.822983865894f, 0.239315664288f, -0.464723172044f, -0.935016242685f, }, 
    { 0.885456025653f, 0.120536680255f, -0.748510748171f, -0.970941817426f, -0.354604887043f, 0.568064746731f, 1.0f, 0.568064746731f, -0.354604887043f, -0.970941817426f, -0.748510748171f, 0.120536680255f, 0.885456025653f, }, 
    { 0.822983865894f, -0.239315664288f, -0.992708874098f, -0.464723172044f, 0.663122658241f, 0.935016242685f, 3.06161699787e-16f, -0.935016242685f, -0.663122658241f, 0.464723172044f, 0.992708874098f, 0.239315664288f, -0.822983865894f, }, 
    { 0.748510748171f, -0.568064746731f, -0.885456025653f, 0.354604887043f, 0.970941817426f, -0.120536680255f, -1.0f, -0.120536680255f, 0.970941817426f, 0.354604887043f, -0.885456025653f, -0.568064746731f, 0.748510748171f, }, 
    { 0.663122658241f, -0.822983865894f, -0.464723172044f, 0.935016242685f, 0.239315664288f, -0.992708874098f, -4.28626379702e-16f, 0.992708874098f, -0.239315664288f, -0.935016242685f, 0.464723172044f, 0.822983865894f, -0.663122658241f, }, 
    { 0.568064746731f, -0.970941817426f, 0.120536680255f, 0.885456025653f, -0.748510748171f, -0.354604887043f, 1.0f, -0.354604887043f, -0.748510748171f, 0.885456025653f, 0.120536680255f, -0.970941817426f, 0.568064746731f, }, 
    { 0.464723172044f, -0.992708874098f, 0.663122658241f, 0.239315664288f, -0.935016242685f, 0.822983865894f, 5.51091059616e-16f, -0.822983865894f, 0.935016242685f, -0.239315664288f, -0.663122658241f, 0.992708874098f, -0.464723172044f, }, 
    { 0.354604887043f, -0.885456025653f, 0.970941817426f, -0.568064746731f, -0.120536680255f, 0.748510748171f, -1.0f, 0.748510748171f, -0.120536680255f, -0.568064746731f, 0.970941817426f, -0.885456025653f, 0.354604887043f, }, 
    { 0.239315664288f, -0.663122658241f, 0.935016242685f, -0.992708874098f, 0.822983865894f, -0.464723172044f, -2.44991257893e-15f, 0.464723172044f, -0.822983865894f, 0.992708874098f, -0.935016242685f, 0.663122658241f, -0.239315664288f, }, 
    { 0.120536680255f, -0.354604887043f, 0.568064746731f, -0.748510748171f, 0.885456025653f, -0.970941817426f, 1.0f, -0.970941817426f, 0.885456025653f, -0.748510748171f, 0.568064746731f, -0.354604887043f, 0.120536680255f, }, 
};
// clang-format on

template <typename ValueType, typename ValueType2>
RowMatrix<ValueType> GetRowMatrix(const std::vector<std::vector<ValueType2>>& values)
{
    auto numRows = values.size();
    auto numColumns = values[0].size();
    RowMatrix<ValueType> result(numRows, numColumns);
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        const auto& row = values[rowIndex];
        if (row.size() != numColumns)
        {
            throw;
        }
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            result(rowIndex, columnIndex) = static_cast<ValueType>(row[columnIndex]);
        }
    }
    return result;
}

void TestDCTMatrix(const std::vector<std::vector<double>>& refMatrixValues, bool normalize = false)
{
    const float epsilon = 1e-4f;
    auto refMatrix = GetRowMatrix<float>(refMatrixValues);
    auto dctMatrix = GetDCTMatrix<float>(refMatrix.NumRows(), refMatrix.NumColumns(), normalize);
    testing::ProcessTest("Testing DCT generation", dctMatrix.IsEqual(refMatrix, epsilon));

    if (!dctMatrix.IsEqual(refMatrix, epsilon))
    {
        // std::cout << "Error, DCT test failed\n";
        // std::cout << "Ref matrix:\n";
        // std::cout << refMatrix << "\n\n";
        // std::cout << "Out matrix:\n";
        // std::cout << dctMatrix << "\n\n";
        // dctMatrix -= refMatrix;
        // std::cout << "Difference matrix:\n";
        // std::cout << dctMatrix << "\n\n";
    }
}

void TestDCT()
{
    TestDCTMatrix(dct_precomputed);

    // DCT-II vs. scipy
    TestDCTMatrix(GetDCTReference_II_8());
    TestDCTMatrix(GetDCTReference_II_64());
    TestDCTMatrix(GetDCTReference_II_128());

    // normalized DCT-II vs. scipy
    TestDCTMatrix(GetDCTReference_II_norm_8(), true);
    TestDCTMatrix(GetDCTReference_II_norm_64(), true);
    TestDCTMatrix(GetDCTReference_II_norm_128(), true);

    // test vs. librosa (DCT-III)
    // Disabling these tests for now --- librosa computes the DCT-III, and we compute DCT-II.
    // Once we add normalization and inverse, we will be able to generate DCT-III matrices to test against.
    // TestDCTMatrix(GetDCTReference_III_64_13());
    // TestDCTMatrix(GetDCTReference_III_64_40());
    // TestDCTMatrix(GetDCTReference_III_128_13());
    // TestDCTMatrix(GetDCTReference_III_128_40());
}
