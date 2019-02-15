////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerOptions.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelOptimizerOptions.h"

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>

#define ADD_TO_STRING_ENTRY(NAMESPACE, ENTRY) \
    case NAMESPACE::ENTRY:                    \
        return #ENTRY;
#define BEGIN_FROM_STRING if (false)
#define ADD_FROM_STRING_ENTRY(NAMESPACE, ENTRY) else if (s == #ENTRY) return NAMESPACE::ENTRY

namespace ell
{
namespace model
{
    std::string ToString(const PreferredConvolutionMethod& m)
    {
        switch (m)
        {
            ADD_TO_STRING_ENTRY(PreferredConvolutionMethod, automatic);
            ADD_TO_STRING_ENTRY(PreferredConvolutionMethod, diagonal);
            ADD_TO_STRING_ENTRY(PreferredConvolutionMethod, simple);
            ADD_TO_STRING_ENTRY(PreferredConvolutionMethod, winograd);
            ADD_TO_STRING_ENTRY(PreferredConvolutionMethod, unrolled);
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown PreferredConvolutionMethod");
        };
    }

    ModelOptimizerOptions::ModelOptimizerOptions(const utilities::PropertyBag& properties) :
        _options(properties)
    {
    }

    bool ModelOptimizerOptions::HasEntry(const std::string& key) const
    {
        return _options.HasEntry(key);
    }

    const utilities::Variant& ModelOptimizerOptions::GetEntry(const std::string& key) const
    {
        return _options.GetEntry(key);
    }

    utilities::Variant& ModelOptimizerOptions::operator[](const std::string& key)
    {
        return _options[key];
    }

    void AppendOptionsToMetadata(const ModelOptimizerOptions& options, utilities::PropertyBag& properties)
    {
        for (const auto& option : options)
        {
            properties[option.first] = option.second;
        }
    }

    void AppendMetadataToOptions(const utilities::PropertyBag& properties, ModelOptimizerOptions& options)
    {
        for (const auto& property : properties)
        {
            options[property.first] = property.second;
        }
    }
} // namespace model

namespace utilities
{
    template <>
    model::PreferredConvolutionMethod FromString<model::PreferredConvolutionMethod>(const std::string& s)
    {
        BEGIN_FROM_STRING;
        ADD_FROM_STRING_ENTRY(model::PreferredConvolutionMethod, automatic);
        ADD_FROM_STRING_ENTRY(model::PreferredConvolutionMethod, diagonal);
        ADD_FROM_STRING_ENTRY(model::PreferredConvolutionMethod, simple);
        ADD_FROM_STRING_ENTRY(model::PreferredConvolutionMethod, winograd);
        ADD_FROM_STRING_ENTRY(model::PreferredConvolutionMethod, unrolled);

        throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Unknown PreferredConvolutionMethod");
    }
} // namespace utilities
} // namespace ell

#undef ADD_TO_STRING_ENTRY
#undef BEGIN_FROM_STRING
#undef ADD_FROM_STRING_ENTRY
