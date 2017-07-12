////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <string>

namespace ell
{
namespace emitters
{
    /// <summary> Properties of a target device. </summary>
    struct TargetDevice
    {
        std::string deviceName = "";
        std::string triple = "";
        std::string architecture = "";
        std::string dataLayout = "";
        std::string cpu = "";
        std::string features = "";
        size_t numBits = 0;

        /// <summary> Indicates if the target device is a Windows system </summary>
        bool IsWindows() const;
    };
}
}
