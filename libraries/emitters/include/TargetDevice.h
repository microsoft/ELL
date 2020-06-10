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

        /// <summary> Helper function to test whether the TargetDevice has a particular feature </summary>
        /// <remarks> If this is filled in by LLVM for the host target, the possible features are target dependent
        /// and include, but are not limited to, the following:
        /// X86: cx8, cmov, mmx, fxsr, sse, sse2, sse3, pclmul, ssse3, cx16, sse4.1, sse4.2, movbe, popcnt, aes, rdrnd,
        ///      avx, fma, xsave, f16c, sahf, lzcnt, sse4a, prfchw, xop, lwp, fma4, tbm, mwaitx, 64bit, clzero, wbnoinvd,
        ///      fsgsbase, sgx, bmi, avx2, bmi2, invpcid, rtm, avx512f, avx512dq, rdseed, adx, avx512ifma, clflushopt,
        ///      clwb, avx512pf, avx512er, avx512cd, sha, avx512bw, avx512vl, prefetchwt1, avx512vbmi, pku, waitpkg,
        ///      avx512vbmi2, shstk, gfni, vaes, vpclmulqdq, avx512vnni, avx512bitalg, avx512vpopcntdq, rdpid, cldemote,
        ///      movdiri, movdir64b, enqcmd, pconfig, avx512bf16, xsaveopt, xsavec, xsaves, ptwrite
        /// AArch64: neon, fp-armv8, crc, crypto
        /// ARM: fp16, neon, vfp3, d16, vfp4, hwdiv-arm, hwdiv
        /// </remarks>
        inline bool HasFeature(const std::string& feature) const { return features.find(feature) != std::string::npos; }

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
