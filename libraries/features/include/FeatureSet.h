////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FeatureSet.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Feature.h"
#include "DoubleVector.h"
#include "InputFeature.h"
#include "CoordinateList.h"
#include "Model.h"

#include <string>
#include <vector>
#include <memory>

namespace features
{    
//    using DataVector = linear::DoubleVector; 
    typedef linear::DoubleVector DataVector;
    
    class FeatureSet
    {
    public:
        FeatureSet();
        ~FeatureSet() {};
        
        void Reset();
        bool ProcessInputData(const DataVector& inData) const; // Returns true if we generated output (in which case, call GetOutput())
        // Note: to deal with buffering nodes (e.g., FFT), need to split ProcessInputData into 2 or 3 phases:
        // SetInput(...) // triggers dirty calc cascade
        // GetOutput() // sets dirty bit off
        bool HasOutput() const; // Returns dirty bit value. If we have multiple output features, then this should be a function on the feature node
        DataVector GetOutput() const;
        
        std::shared_ptr<InputFeature> GetInputFeature() const;
        std::shared_ptr<Feature> GetOutputFeature() const;
        void SetOutputFeature(const std::shared_ptr<Feature>& output);

        std::shared_ptr<Feature> GetFeature(const std::string& featureId) const;

        // Creates a new feature and adds it to the FeatureSet
        template <typename FeatureType, typename... Args>
        std::shared_ptr<FeatureType> CreateFeature(Args... args);

        // Function for creating a feature from a string description
        std::shared_ptr<Feature> CreateFeatureFromDescription(const std::vector<std::string>& description);

        void Deserialize(std::istream& inStream);
        void Serialize(std::ostream& outStream) const; // Serializes all features in the graph
        void SerializeActiveGraph(std::ostream& outStream) const; // Serializes only the features needed to recreate output

        // Visits all features in the graph in dependency order
        // (that is, a feature is never visited unless its inputs have all been visted first)
        template <typename Visitor>
        void Visit(Visitor& visitor) const; 

        // Visits active features in the graph in dependency order
        // (that is, a feature is never visited unless its inputs have all been visted first)
        template <typename Visitor>
        void VisitActiveGraph(Visitor& visitor) const;

        /// <summary> Adds the feature set to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        /// <returns> The output coordinates </returns>
        layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    private:
        Feature::FeatureMap _featureMap;
        std::shared_ptr<InputFeature> _inputFeature;
        std::shared_ptr<Feature> _outputFeature;
        mutable size_t _numItemsProcessed = 0;
    };
}

#include "../tcc/FeatureSet.tcc"
