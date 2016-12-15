////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SingleElementThresholdPredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// data
#include "DenseDataVector.h"

// utilities
#include "IArchivable.h"

// stl
#include <iostream>

namespace ell
{
namespace predictors
{
    /// <summary> A split rule that compares a single feature to a threshold. </summary>
    class SingleElementThresholdPredictor : public IPredictor<bool>, public utilities::IArchivable
    {
    public:
        using DataVectorType = data::FloatDataVector;

        /// <summary> Constructs a single-element threshold rule. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        /// <param name="threshold"> The threshold. </param>
        SingleElementThresholdPredictor(size_t index = 0, double threshold = 0.0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "SingleElementThresholdPredictor"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Gets the index of the element used to define the rule. </summary>
        ///
        /// <returns> The element index. </returns>
        size_t GetElementIndex() const { return _index; }

        /// <summary> Gets the threshold. </summary>
        ///
        /// <returns> The threshold value. </returns>
        double GetThreshold() const { return _threshold; }

        /// <summary> Evaluates the split rule. </summary>
        ///
        /// <param name="dataVector"> The input vector. </param>
        ///
        /// <returns> The result of the split rule. </returns>
        bool Predict(const DataVectorType& inputVector) const;

        /// <summary> Returns the number of outputs (the max output value plus one). </summary>
        ///
        /// <returns> The number of outputs. </returns>
        size_t NumOutputs() const { return 2; }

        /// <summary> Prints the rule to an output stream on a separate line. </summary>
        ///
        /// <param name="os"> The output stream. </param>
        ///  <param name="tabs"> The number of tabs. </param>
        void PrintLine(std::ostream& os, size_t tabs = 0) const;

    private:
        size_t _index;
        double _threshold;
    };
}
}
