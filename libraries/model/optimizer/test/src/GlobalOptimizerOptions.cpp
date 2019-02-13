////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GlobalOptimizerOptions.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GlobalOptimizerOptions.h"

#include <utilities/include/Exception.h>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        //
        // OptimizerOptions
        //
        bool OptimizerOptions::HasOption(std::string name)
        {
            return _options.find(name) != _options.end();
        }

        std::string OptimizerOptions::GetOption(std::string name, std::string defaultValue)
        {
            auto iter = _options.find(name);
            if (iter == _options.end())
            {
                return defaultValue;
            }
            return iter->second;
        }

        void OptimizerOptions::SetOption(std::string name, std::string value)
        {
            _options[name] = value;
        }
    } // namespace optimizer
} // namespace model
} // namespace ell
