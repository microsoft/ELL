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

    void SingleElementThresholdPredictor::Serialize(utilities::Serializer& serializer) const
    {
        serializer.Serialize("index", _index);
        serializer.Serialize("threshold", _threshold);
    }

    void SingleElementThresholdPredictor::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        serializer.Deserialize("index", _index, context);
        serializer.Deserialize("threshold", _threshold, context);
    }

    void SingleElementThresholdPredictor::PrintLine(std::ostream & os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "index = " << _index << ", threshold = " << _threshold << "\n";
    }
}