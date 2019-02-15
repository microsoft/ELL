////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MapCompilerOptions.h (model)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <emitters/include/CompilerOptions.h>

#include <utilities/include/PropertyBag.h>

#include <string>

namespace ell
{
namespace model
{
    /// <summary> Map-specific compiler settings </summary>
    struct MapCompilerOptions
    {
        MapCompilerOptions() = default;

        /// <summary> Constructor from a property bag </summary>
        explicit MapCompilerOptions(const utilities::PropertyBag& properties);

        /// <summary> Create a new `MapCompilerOptions` by adding or overriding the options in the given `PropertyBag` </summary>
        [[nodiscard]] MapCompilerOptions AppendOptions(const utilities::PropertyBag& properties) const;

        // global options
        std::string moduleName = "ELL";
        std::string mapFunctionName = "predict";
        std::string sourceFunctionName;
        std::string sinkFunctionName;
        bool verifyJittedModule = false;
        bool profile = false;

        // per-node options
        bool inlineNodes = false;

        // lower-level emitters settings
        emitters::CompilerOptions compilerSettings;

    private:
        void AddOptions(const utilities::PropertyBag& properties);
    };
} // namespace model
} // namespace ell
