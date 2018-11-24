////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NodesTestData.cpp (nodes_test)
//  Authors:  Chuck Jacobs, Byron Changuion, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NodesTestData.h"

/*

import numpy as np
import scipy

np.random.seed(123)
data = np.random.randint(-8, 9, (10, 10))
filter = np.random.randint(-4, 5, (3,3))
corr_result = scipy.signal.correlate(data, filter, mode='same')
conv_result = scipy.signal.convolve(data, filter, mode='same')
corr_result_valid = scipy.signal.correlate(data, filter, mode='valid')
conv_result_valid = scipy.signal.convolve(data, filter, mode='valid')

print(data)
print(filter)
print(corr_result)
print(conv_result)
print(corr_result_valid)
print(conv_result_valid)

>>> print(data)
[[ 5 -6 -6 -2  2 -7 -8  7  1 -8]
 [ 6 -8  7  6 -4 -8  8 -4 -5 -6]
 [-1 -6  7  8 -1  1 -5 -2 -7 -6]
 [-7  4  0 -5  2 -3 -8  3 -6  2]
 [ 5 -4  7  3  4 -2  5  8 -2  6]
 [-1  3 -1 -7  3 -3  4 -7  4  1]
 [ 8 -5 -5  3 -1  1 -6 -5  3 -5]
 [-2  1  6 -2 -2  4 -2  6  2 -5]
 [-5 -7  7  5  8 -3  0  5  6  7]
 [ 0  5 -8 -5 -7 -4 -1 -2 -7  4]]

>>> print(filter)
[[-1 -1  3]
 [ 2  4  2]
 [ 0  0  3]]

>>> print(corr_result)
[[-16  -5 -22 -28 -34 -16 -44  -1 -16 -30]
 [-33  -2  54  41 -38 -58  38 -27 -90 -27]
 [-34  11  36  25 -21   4 -25 -69 -47 -27]
 [-49  51  30 -22 -18 -24 -10 -36  -5   9]
 [ 40   8 -14  54   3  -1  31  37  32  24]
 [-30  13   3 -25 -18  -3   2 -22   1   8]
 [ 35  -1 -53  11  11  -4 -36  -5 -17 -19]
 [-50  15  56  23 -12 -10  17  62  18 -14]
 [-14 -29  -4  19  40  -7  14  13  37  43]
 [ -6  37 -17 -38 -68 -37   2 -11 -14 -11]]

>>> print(conv_result)
[[ 10  -7 -77   3  20 -52 -60  47   1 -39]
 [ 15   5 -27  26  -2 -17  -3 -40 -12 -46]
 [-13 -19  25  68  18  -3 -50 -29 -43 -73]
 [-21  11 -42  19  23 -18 -48 -22   0 -37]
 [ 10 -18  55  35 -18  24  17  29   3  13]
 [ -1  57 -37 -20  10  16   4 -13  15  20]
 [ 23 -30 -16  19 -25  -9 -33 -28  -8   9]
 [  6  21 -26  -7  15  32   1  -5  -3   4]
 [-39 -27  55  56  26 -18   7  32  63  21]
 [ 10 -11 -53 -29 -31  -8 -25 -24  -9  20]]

>>> print(corr_result_valid)
[[ -2  54  41 -38 -58  38 -27 -90]
 [ 11  36  25 -21   4 -25 -69 -47]
 [ 51  30 -22 -18 -24 -10 -36  -5]
 [  8 -14  54   3  -1  31  37  32]
 [ 13   3 -25 -18  -3   2 -22   1]
 [ -1 -53  11  11  -4 -36  -5 -17]
 [ 15  56  23 -12 -10  17  62  18]
 [-29  -4  19  40  -7  14  13  37]]

>>> print(conv_result_valid)
[[  5 -27  26  -2 -17  -3 -40 -12]
 [-19  25  68  18  -3 -50 -29 -43]
 [ 11 -42  19  23 -18 -48 -22   0]
 [-18  55  35 -18  24  17  29   3]
 [ 57 -37 -20  10  16   4 -13  15]
 [-30 -16  19 -25  -9 -33 -28  -8]
 [ 21 -26  -7  15  32   1  -5  -3]
 [-27  55  56  26 -18   7  32  63]]

*/

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestData()
{
    return {
        5, -6, -6, -2, 2, -7, -8, 7, 1, -8, 6, -8, 7, 6, -4, -8, 8, -4, -5, -6, -1, -6, 7, 8, -1, 1, -5, -2, -7, -6, -7, 4, 0, -5, 2, -3, -8, 3, -6, 2, 5, -4, 7, 3, 4, -2, 5, 8, -2, 6, -1, 3, -1, -7, 3, -3, 4, -7, 4, 1, 8, -5, -5, 3, -1, 1, -6, -5, 3, -5, -2, 1, 6, -2, -2, 4, -2, 6, 2, -5, -5, -7, 7, 5, 8, -3, 0, 5, 6, 7, 0, 5, -8, -5, -7, -4, -1, -2, -7, 4
    };
}

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestFilter()
{
    return {
        -1, -1, 3, 2, 4, 2, 0, 0, 3
    };
}

template <typename ValueType>
std::vector<ValueType> GetCorrelationTestResultSame()
{
    return {
        -16, -5, -22, -28, -34, -16, -44, -1, -16, -30, -33, -2, 54, 41, -38, -58, 38, -27, -90, -27, -34, 11, 36, 25, -21, 4, -25, -69, -47, -27, -49, 51, 30, -22, -18, -24, -10, -36, -5, 9, 40, 8, -14, 54, 3, -1, 31, 37, 32, 24, -30, 13, 3, -25, -18, -3, 2, -22, 1, 8, 35, -1, -53, 11, 11, -4, -36, -5, -17, -19, -50, 15, 56, 23, -12, -10, 17, 62, 18, -14, -14, -29, -4, 19, 40, -7, 14, 13, 37, 43, -6, 37, -17, -38, -68, -37, 2, -11, -14, -11
    };
}

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestResultSame()
{

    return {
        10, -7, -77, 3, 20, -52, -60, 47, 1, -39, 15, 5, -27, 26, -2, -17, -3, -40, -12, -46, -13, -19, 25, 68, 18, -3, -50, -29, -43, -73, -21, 11, -42, 19, 23, -18, -48, -22, 0, -37, 10, -18, 55, 35, -18, 24, 17, 29, 3, 13, -1, 57, -37, -20, 10, 16, 4, -13, 15, 20, 23, -30, -16, 19, -25, -9, -33, -28, -8, 9, 6, 21, -26, -7, 15, 32, 1, -5, -3, 4, -39, -27, 55, 56, 26, -18, 7, 32, 63, 21, 10, -11, -53, -29, -31, -8, -25, -24, -9, 20
    };
}

template <typename ValueType>
std::vector<ValueType> GetConvolutionTestResultValid()
{
    return {
        -2, 54, 41, -38, -58, 38, -27, -90, 11, 36, 25, -21, 4, -25, -69, -47, 51, 30, -22, -18, -24, -10, -36, -5, 8, -14, 54, 3, -1, 31, 37, 32, 13, 3, -25, -18, -3, 2, -22, 1, -1, -53, 11, 11, -4, -36, -5, -17, 15, 56, 23, -12, -10, 17, 62, 18, -29, -4, 19, 40, -7, 14, 13, 37
    };
}

template <typename ValueType>
std::vector<ValueType> GetCorrelationTestResultValid()
{
    return {
        5, -27, 26, -2, -17, -3, -40, -12, -19, 25, 68, 18, -3, -50, -29, -43, 11, -42, 19, 23, -18, -48, -22, 0, -18, 55, 35, -18, 24, 17, 29, 3, 57, -37, -20, 10, 16, 4, -13, 15, -30, -16, 19, -25, -9, -33, -28, -8, 21, -26, -7, 15, 32, 1, -5, -3, -27, 55, 56, 26, -18, 7, 32, 63
    };
}

//
// Explicit instantiations
//
template std::vector<float> GetConvolutionTestData();
template std::vector<float> GetConvolutionTestFilter();
template std::vector<float> GetConvolutionTestResultValid();
template std::vector<float> GetConvolutionTestResultSame();
template std::vector<float> GetCorrelationTestResultValid();
template std::vector<float> GetCorrelationTestResultSame();

template std::vector<double> GetConvolutionTestData();
template std::vector<double> GetConvolutionTestFilter();
template std::vector<double> GetConvolutionTestResultValid();
template std::vector<double> GetConvolutionTestResultSame();
template std::vector<double> GetCorrelationTestResultValid();
template std::vector<double> GetCorrelationTestResultSame();
