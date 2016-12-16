////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstantPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConstantPredictor.h"

// stl
#include <string>

namespace ell
{
namespace predictors
{
    ConstantPredictor::ConstantPredictor(double value)
        : _value(value)
    {
    }

    void ConstantPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["value"] << _value;
    }

    void ConstantPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["value"] >> _value;
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
}
