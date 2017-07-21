////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstantPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// utilities
#include "IArchivable.h"

//stl
#include <iostream>

namespace ell
{
namespace predictors
{
    /// <summary> A predictor that ignores its input and outputs a constant number. This class is used to define decision trees. </summary>
    class ConstantPredictor : public IPredictor<double>, public utilities::IArchivable
    {
    public:
        ConstantPredictor() = default;

        /// <summary> Constructs an instance of ConstantPredictor. </summary>
        ///
        /// <param name="value"> The constant output value. </param>
        ConstantPredictor(double value);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ConstantPredictor"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> A function that ignores its input and returns a constant value. </summary>
        ///
        /// <returns> A constant value. </returns>
        template <typename AnyType>
        double Predict(const AnyType&) const
        {
            return _value;
        }

        /// <summary> Gets the constant value. </summary>
        ///
        /// <returns> The constant value. </returns>
        double GetValue() const { return _value; }

        /// <summary> Prints a representation of the predictor to the output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const;

        /// <summary> Prints a representation of the predictor to the output stream. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        /// <param name="tabs"> The number of tabs. </param>
        void PrintLine(std::ostream& os, size_t tabs) const;

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        double _value = 0.0;
    };
}
}
