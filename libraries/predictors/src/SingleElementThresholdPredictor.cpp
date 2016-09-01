////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdPredictor.h"

namespace predictors
{
    SingleElementThresholdPredictor::SingleElementThresholdPredictor(size_t index, double threshold) : _index(index), _threshold(threshold)
    {}

    utilities::ObjectDescription SingleElementThresholdPredictor::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<SingleElementThresholdPredictor>("Single-element threshold predictor");
        description.AddProperty<decltype(_index)>("index", "The index of the element");
        description.AddProperty<decltype(_threshold)>("threshold", "The threshold");
        return description;
    }

    utilities::ObjectDescription SingleElementThresholdPredictor::GetDescription() const
    {
        auto description = GetTypeDescription();
        description["index"] << _index;
        description["threshold"] << _threshold;
        return description;
    }

    void SingleElementThresholdPredictor::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        description["index"] >> _index;
        description["threshold"] >> _threshold;
    }

    void SingleElementThresholdPredictor::PrintLine(std::ostream & os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "index = " << _index << ", threshold = " << _threshold << "\n";
    }
}