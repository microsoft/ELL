////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TargetDevice.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

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

        /// <summary> Indicates if the target device is a Linux system </summary>
        bool IsLinux() const;

        /// <summary> Indicates if the target device is a macOS system </summary>
        bool IsMacOS() const;
    };

    /// <summary> Create a TargetDevice from a device name. </summary>
    TargetDevice GetTargetDevice(std::string deviceName);

    /// <summary> Create a fully-specified TargetDevice from a partially-specified one. </summary>
    /// Typically, this function is used after manually filling in a subset of the fields of a
    /// `TargetDevice` struct, in order to fill in reasonable values for the remaining fields.
    /// Note that the structs returned from `GetTargetDevice(deviceName)` are fully-specified, and don't
    /// require you to call this function to complete them.
    void CompleteTargetDevice(TargetDevice& partialDeviceInfo);
} // namespace emitters
} // namespace ell
