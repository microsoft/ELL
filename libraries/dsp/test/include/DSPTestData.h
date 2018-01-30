
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPTestData.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

std::vector<double> GetFFTTestData_32();
std::vector<double> GetFFTTestData_64();
std::vector<double> GetFFTTestData_128();
std::vector<double> GetFFTTestData_256();
std::vector<double> GetFFTTestData_512();
std::vector<double> GetFFTTestData_1024();
std::vector<double> GetRealFFT_32();
std::vector<double> GetRealFFT_64();
std::vector<double> GetRealFFT_128();
std::vector<double> GetRealFFT_256();
std::vector<double> GetRealFFT_512();
std::vector<double> GetRealFFT_1024();
std::vector<std::vector<double>> GetDCTReference_II_8();
std::vector<std::vector<double>> GetDCTReference_II_norm_8();
std::vector<std::vector<double>> GetDCTReference_II_64();
std::vector<std::vector<double>> GetDCTReference_II_norm_64();
std::vector<std::vector<double>> GetDCTReference_II_128();
std::vector<std::vector<double>> GetDCTReference_II_norm_128();
std::vector<std::vector<double>> GetDCTReference_III_64_40();
std::vector<std::vector<double>> GetDCTReference_III_64_13();
std::vector<std::vector<double>> GetDCTReference_III_128_40();
std::vector<std::vector<double>> GetDCTReference_III_128_13();
std::vector<std::vector<double>> GetMelReference_16000_512_128();
std::vector<std::vector<double>> GetMelReference_16000_512_40();
std::vector<std::vector<double>> GetMelReference_16000_512_13();
std::vector<std::vector<double>> GetMelReference_8000_512_128();
std::vector<std::vector<double>> GetMelReference_8000_512_40();
std::vector<std::vector<double>> GetMelReference_8000_512_13();
std::vector<double> GetReferenceHammingWindow_128_symmetric();
std::vector<double> GetReferenceHammingWindow_301_symmetric();
std::vector<double> GetReferenceHammingWindow_1000_symmetric();
std::vector<double> GetReferenceHammingWindow_128_periodic();
std::vector<double> GetReferenceHammingWindow_301_periodic();
std::vector<double> GetReferenceHammingWindow_1000_periodic();
