////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Feature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CoordinateList.h"
#include "Model.h"

#include <string>
#include <vector>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <functional>

namespace features
{
    class InputFeature;

    /// <summary>
    /// Base class for all features. 
    /// Users of the library will generally only interact with features via the featurizer (`FeatureSet`) object that
    /// owns the individual features.  
    /// </summary>
    class Feature
    {
    public:
        virtual ~Feature() {};

        /// <summary> The unique ID of this feature. </summary>
        std::string Id() const;
        
        /// <summary> The output dimensionality of this feature. </summary>
        size_t GetOutputDimension() const;
        
        /// <summary> Indicates if this feature has new output to be read. </summary>
        virtual bool HasOutput() const;

        /// <summary> The current output of this feature. </summary>
        std::vector<double> GetOutput() const;

        /// <summary> Resets any internal state </summary>
        virtual void Reset();
        
        /// <summary> Amount of time it takes for this feature to respond to input. </summary>
        virtual size_t GetWarmupTime() const;
        
        /// <summary> 
        /// Returns a list-of-strings representation of the feature. Used when deserializing a feature set from a file, and
        /// for creating features from a GUI
        /// </summary>
        std::vector<std::string> GetDescription() const;
        
        /// <summary>
        /// Returns a list of strings giving semi-interpretable names for the columns (dimensions) of the output. Intended to
        /// be suitable for use as the header line in an output dataset file. 
        /// </summary>
        virtual std::vector<std::string> GetColumnDescriptions() const;
        
        /// <summary> Returns the set of inputs to this feature </summary>
        const std::vector<Feature*>& GetInputFeatures() const;

        /// <summary> Returns a list of the feature types registered in the global feature registry. </summary>
        static std::vector<std::string> GetRegisteredTypes();
        
        using FeatureMap = std::unordered_map<std::string, Feature*>;
        using DeserializeFunction = std::function<std::unique_ptr<Feature>(std::vector<std::string>, FeatureMap&)>; // TODO: have creation (deserialization) function take a const FeatureMap&
        
        /// <summary> Adds a feature to the global registry of features. Allows deserialization and creation from vector of strings. </summary>
        template <typename FeatureType>
        static void RegisterFeatureType();

    protected:
        Feature(const std::vector<Feature*>& inputs);
        Feature(std::string Id, const std::vector<Feature*>& inputs);

        //Virtual methods that implement feature-dependent things
        virtual std::vector<double> ComputeOutput() const = 0;
        virtual void AddToDescription(std::vector<std::string>& description) const {};
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const = 0;        
        virtual std::string FeatureType() const = 0;
        void AddDependent(Feature* f);

        static void RegisterDeserializeFunction(std::string class_name, DeserializeFunction create_fn);
        bool IsDirty() const;
        void SetDirtyFlag(bool dirty) const;
        void AddInputFeature(Feature* inputFeature);

        std::vector<Feature*> _inputFeatures; // parents
        mutable size_t _outputDimension = 0;
        mutable std::vector<double> _cachedValue;

    private:
        friend class FeatureSet;
        void Serialize(std::ostream& outStream) const;
        static std::unique_ptr<Feature> FromDescription(const std::vector<std::string>& description, FeatureMap& deserializedFeatureMap);

        mutable bool _isDirty = true;
        std::vector<Feature*> _dependents; // children
        std::string _id;
        int _instanceId = 0;

        static int _instanceCount;
        static std::unordered_map<std::string, DeserializeFunction> _createTypeMap;
    };
}

#include "../tcc/Feature.tcc"
