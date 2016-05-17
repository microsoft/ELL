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
    /// A `FeatureSet` (or _featurizer_) encodes a transformation from an input time series (a series of vector-valued inputs)
    /// to a set of output vectors. While the primary use for the featurizer is to transform input data into feature vectors
    /// for a predictor or trainer, there isn't anything particularly "feature"-specific about it. The features could represent
    /// denoising preprocessing filters, or post-prediction calibration as well. 
    /// Its implementation is that of a dataflow graph with one designated input node (an instance of the `InputFeature` class),
    /// and one designated output node. Each node produces takes input (typically from other `Feature` nodes) and produces its output.
    /// </summary>
    /// <remarks>
    /// Note: To deal with buffering nodes that don't always output a value when they get new input (e.g., FFT), we can't 
    /// have  a single interface function for processing data (e.g., `double Compute(vec input)`), but instead we have to 
    /// split it into 2 or 3 functions:
    /// - void SetInput(vec input) -- returns true if we produced new data. 
    /// - vec GetOutput() -- returns output and clears the 'has data' flag
    ///
    /// Note: The input data is typically assumed to be a continuous stream of data, and to deal with interruptions in the stream which
    /// may invalidate the internal state of the features, there is a `Reset()` function which can be used to reinitialize the
    /// featurizer in case of such an interruption. It would be necessary to call `Reset` when switching between datasets as well.
    ///
    /// Here's an example of how one would produce a simple (7-dimensional) feature vector from some input accelerometer data:
    /// 
    /// > auto featurizer = FeatureSet();
    /// > auto input = featurizer.CreateFeature<InputFeature>(3); // input dataset dimension expected to be 3
    /// > auto gravity = featurizer.CreateFeature<IirFilterFeature>(input, {0.1}, {0.9}); // a simple lowpass filter
    /// > auto signal = featurizer.CreateFeature<SubtractFeature(input, gravity); // remove gravity from the raw input, call this the 'signal'
    /// > auto channelVariance = featurizer.CreateFeature<VarianceFeature>(signal, 16); // variance over a 16-sample window (computed separately for each dimension)
    /// > auto energy = featurizer.CreateFeature<Magnitude>(signal);
    /// > auto energyVariance = featurizer.CreateFeature<VarianceFeature>(energy, 16); // variance of the signal magnitude over a 16-sample window
    /// > auto outputFeature = featurizer.CreateFeature<ConcatFeature>(gravity, channelVariance, energyVariance);
    /// </remarks>
    class FeatureSet
    {
    public:
        /// <summary> Default constructor </summary>
        FeatureSet() : _numItemsProcessed(0) {}        
        ~FeatureSet() = default;

        /// <summary> Supplies input data for the featurizer to process. </summary>
        /// <returns> true if the featurizer has new output as a result. </returns>
        bool ProcessInputData(const DataVector& inData) const; // Returns true if we generated output (in which case, call GetOutput())

        /// <summary> Indicates whether there is new output data available. </summary>
        bool HasOutput() const { return _numItemsProcessed >= _outputFeature->GetWarmupTime() && _outputFeature->HasOutput(); }    
    
        /// <summary> Get the most recently-computed output feature vector </summary>
        DataVector GetOutput() const { return _outputFeature->GetOutput(); }
    
        /// <summary> Resets the internal state of the featurizer. Typically called after an interruption in input signal data. </summary>          
        void Reset();

        /// <summary> Returns the input for the featurizer. </summary>
        InputFeature* GetInputFeature() const { return _inputFeature; }

        /// <summary> Gets the output feature for the featurizer. </summary>
        Feature* GetOutputFeature() const { return _outputFeature; }

        /// <summary> Sets the output feature for the featurizer. </summary>
        void SetOutputFeature(Feature* output) { _outputFeature = output; }

        /// <summary> Retrieves a feature by its ID </summary>
        Feature* GetFeature(const std::string& featureId) const;

        /// <summary> Creates a new feature and adds it to the featurizer </summary>
        template <typename FeatureType, typename... Args>
        FeatureType* CreateFeature(Args... args);

        /// <summary> Creates feature from a string description. Used by serialization and UI code. </summary>
        Feature* CreateFeatureFromDescription(const std::vector<std::string>& description);
        
        /// <summary> 
        /// Visits all features in the graph in dependency order (that is, a feature is never visited unless
        /// its inputs have all been visted first). 
        /// </summary>
        /// <param name="visitor">
        /// A visitor functor called for each `Feature` node in the graph. It should be compatible with the function:
        /// `void visitFunction(const Feature& feature)`
        /// </param>
        template <typename Visitor>
        void Visit(Visitor& visitor) const; 

        /// <summary> 
        /// Visits the active features in the graph in dependency order (that is, a feature is never visited unless
        /// its inputs have all been visted first). The "active graph" is the subset of features necessary to 
        /// produce the output. 
        /// </summary>
        template <typename Visitor>
        void VisitActiveGraph(Visitor& visitor) const;

        /// <summary> Adds the feature set to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The coordinates in the model that supply data to the input feature. </param>
        /// <returns> The model output coordinates representing the output of the featurizer </returns>
        layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    private:
        std::vector<std::unique_ptr<Feature>> _features;
        Feature::FeatureMap _featureMap;
        InputFeature* _inputFeature;
        Feature* _outputFeature;
        mutable size_t _numItemsProcessed = 0;
    };
}

#include "../tcc/FeatureSet.tcc"
