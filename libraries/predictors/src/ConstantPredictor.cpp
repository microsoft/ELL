////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConstantPredictor.h"

// stl
#include <string>

namespace predictors
{
    ConstantPredictor::ConstantPredictor(double value) : _value(value)
    {}

    utilities::ObjectDescription ConstantPredictor::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<ConstantPredictor>("Constant predictor");
        description.AddProperty<decltype(_value)>("value", "The value");
        return description;
    }

    utilities::ObjectDescription ConstantPredictor::GetDescription() const
    {
        auto description = GetTypeDescription();
        description["value"] << _value;
        return description;
    }

    void ConstantPredictor::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        description["value"] >> _value;
    }

    void ConstantPredictor::Print(std::ostream& os) const
    {
        os << "Constant predictor = " << _value;
    }

    void ConstantPredictor::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ');
        Print(os);
        os << "\n";
    }
}