////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GlobalOptimizerOptions.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        /// <summary> Optimization parameters supplied by client code </summary>
        class OptimizerOptions
        {
        public:
            /// <summary> Returns `true` if the requestion option is present </summary>
            bool HasOption(std::string name);

            /// <summary> Returns the value for the named option, or the supplied value if not present (the empty string by default). </summary>
            std::string GetOption(std::string name, std::string defaultValue = "");

            /// <summary> Sets the value for the named option. </summary>
            void SetOption(std::string name, std::string value);

        private:
            std::unordered_map<std::string, std::string> _options;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell
