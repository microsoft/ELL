////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ClockInterface.h (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <chrono>

/// <summary>
/// Simple C wrapper for the C++ standard library's chrono, intended to be called from IR
/// This is also a reference implementation that is replaceable for a given environment.
/// </summary>
extern "C" {

double ELL_GetSteadyClockMilliseconds()
{
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

double ELL_GetSystemClockMilliseconds()
{
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

}
