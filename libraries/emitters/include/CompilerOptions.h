////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerOptions.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TargetDevice.h"

#include <utilities/include/Optional.h>
#include <utilities/include/PropertyBag.h>
#include <utilities/include/StringUtil.h>

namespace ell
{
namespace emitters
{
    /// <summary> List of possible BLAS implementations (for enabling implementation-specific features). </summary>
    enum class BlasType
    {
        unknown = 0,
        openBLAS,
        atlas
    };

    std::string ToString(BlasType t);

    /// <summary> Standard compiler switches. </summary>
    struct CompilerOptions
    {
        CompilerOptions() = default;

        /// <summary> Constructor from a property bag. </summary>
        explicit CompilerOptions(const utilities::PropertyBag& properties);

        /// <summary> Create a new `CompilerOptions` by adding or overriding the options in the given `PropertyBag` </summary>
        [[nodiscard]] CompilerOptions AppendOptions(const utilities::PropertyBag& properties) const;

        // global options
        /// <summary> Optimize output code using LLVM. </summary>
        bool optimize = true;

        /// <summary> The specific BLAS implementation to link to ('unknown' will choose whatever is available). </summary>
        BlasType blasType = BlasType::unknown;

        /// <summary> Generate position independent code (equivalent to -fPIC). </summary>
        utilities::Optional<bool> positionIndependentCode;

        /// <summary> Emit profiling code, </summary>
        bool profile = false;

        /// <summary> Enable ELL's parallelization. </summary>
        bool parallelize = false;

        /// <summary> Use thread pool for parallelization (if parallelization enabled). </summary>
        bool useThreadPool = true;

        /// <summary> Maximum num of parallel threads. </summary>
        int maxThreads = 4;

        /// <summary> Allow emitting more efficient code that isn't necessarily IEEE-754 compatible. </summary>
        bool useFastMath = true;

        /// <summary> Allow printing of diagnostic messages from the compiled model. </summary>
        bool includeDiagnosticInfo = false;

        /// <summary> Name of the target device. </summary>
        TargetDevice targetDevice = { "host" };

        // Options that can be changed during code generation (e.g., per function)
        /// <summary> Emit code that calls an external BLAS library. </summary>
        bool useBlas = true;

        /// <summary> Explicitly unroll loops in certain cases. </summary>
        bool unrollLoops = false;

        /// <summary> Emit inline code for common operations. </summary>
        bool inlineOperators = true;

        /// <summary> Enable ELL's vectorization </summary>
        bool allowVectorInstructions = false;

        /// <summary> Size of vector units. </summary>
        int vectorWidth = 4;

        /// <summary> Emit debug code. </summary>
        bool debug = false;

        /// <summary> The name of the file being compiled. </summary>
        std::string modelFile;

        /// <summary> The byte alignment to use for global values. </summary>
        int globalValueAlignment = 32;

        /// <summary> Skip ELLCode optimization. </summary>
        bool skip_ellcode = false;

    private:
        void AddOptions(const utilities::PropertyBag& properties);
    };

} // namespace emitters

namespace utilities
{
    template <>
    emitters::BlasType FromString<emitters::BlasType>(const std::string& s);
}
} // namespace ell
