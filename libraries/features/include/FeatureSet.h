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
    typedef linear::DoubleVector DataVector; // TODO: This ought to really be an IDataVector or something more general
    
    /// <summary>
    /// A `FeatureSet` (or _featurizer_) encodes transformations of data, and is used to transformation
    /// an input time series into feature vectors for a predictor. 
    /// Its implementation is that of a dataflow graph with one special `InputFeature` node, and one designated
    /// output node. Each node produces takes input (typically from other `Feature` nodes) and produces its output.
    /// </summary>
    class FeatureSet
    {
    public:
        FeatureSet();
        ~FeatureSet() {};
        
        /// <summary> Supplies input data for the featurizer to process. </summary>
        /// <returns> true if the featurizer has new output as a result. </returns>
        bool ProcessInputData(const DataVector& inData) const; // Returns true if we generated output (in which case, call GetOutput())
        // Note: to deal with buffering nodes (e.g., FFT), need to split ProcessInputData into 2 or 3 phases:
        // SetInput(...) // triggers dirty calc cascade
        // GetOutput() // sets dirty bit off
        /// <summary> Indicates whether there is new output data available. </summary>
        bool HasOutput() const; // Returns dirty bit value. If we have multiple output features, then this should be a function on the feature node
        /// <summary> Get the most recently-computed output feature vector </summary>
        DataVector GetOutput() const;

        /// <summary> Resets the internal state of the featurizer. Typically called after an interruption in input signal data. </summary>          
        void Reset();

        /// <summary> Returns the input for the featurizer. </summary>
        std::shared_ptr<InputFeature> GetInputFeature() const;
        
        /// <summary> Gets the output feature for the featurizer. </summary>
        std::shared_ptr<Feature> GetOutputFeature() const;
        
        /// <summary> Sets the output feature for the featurizer. </summary>
        void SetOutputFeature(const std::shared_ptr<Feature>& output);

        /// <summary> Retrieves a feature by its ID </summary>
        std::shared_ptr<Feature> GetFeature(const std::string& featureId) const;

        /// <summary> Creates a new feature and adds it to the featurizer </summary>
        template <typename FeatureType, typename... Args>
        std::shared_ptr<FeatureType> CreateFeature(Args... args);

        /// <summary> Creates feature from a string description. Used by serialization and UI code. </summary>
        std::shared_ptr<Feature> CreateFeatureFromDescription(const std::vector<std::string>& description);

        /// <summary> Loads the featurizer from a stream. </summary>
        void Deserialize(std::istream& inStream);
        
        /// <summary> Saves the featurizer to a stream. </summary>
        void Serialize(std::ostream& outStream) const;
        
        /// <summary> Saves the featurizer to a stream, but only including the parts required to compute the output. </summary>
        void SerializeActiveGraph(std::ostream& outStream) const;

        /// <summary> Visits all features in the graph in dependency order
        ///           (that is, a feature is never visited unless its inputs have all been visted first) </summary>
        /// <param name="visitor"> A visitor functor called for each `Feature` node in the graph. It should be compatible with the function:
        ///                        `void visitFunction(const Feature& feature)` </param>
        template <typename Visitor>
        void Visit(Visitor& visitor) const; 

        /// <summary> Visits active features in the graph in dependency order
        ///           (that is, a feature is never visited unless its inputs have all been visted first) </summary>
        template <typename Visitor>
        void VisitActiveGraph(Visitor& visitor) const;

        /// <summary> Adds the feature set to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The coordinates in the model that supply data to the input feature. </param>
        /// <returns> The model output coordinates representing the output of the featurizer </returns>
        layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    private:
        Feature::FeatureMap _featureMap;
        std::shared_ptr<InputFeature> _inputFeature;
        std::shared_ptr<Feature> _outputFeature;
        mutable size_t _numItemsProcessed = 0;
    };
}

#include "../tcc/FeatureSet.tcc"
