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
    /// This `ctor_enable` tag struct is required by a technique to create the equivalent of a protected constructor that can
    /// be called by the base class. The idea is to have this `ctor_enable` type be a protected member of the base
    /// class, so the only code that can create one would be something in the class hierarchy. Therefore, the only code
    /// that can call a public function that contains a `ctor_enable` parameter would be some class in the hierarchy. 
    struct ctor_enable {}; // TODO: figure out how to make this be a protected member of Feature without SWIG complaining

    /// <summary>
    /// Base class for all features. 
    /// Users of the library will generally only interact with features via the featurizer (`FeatureSet`) object that
    /// owns the individual features.  
    /// </summary>
    class Feature
    {
    public:
        Feature();
        Feature(std::string id);
        Feature(const std::vector<Feature*>& inputs);
        Feature(std::string Id, const std::vector<Feature*>& inputs);
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
        virtual size_t WarmupTime() const;
        
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
        
        const std::vector<Feature*>& GetInputFeatures() const;

        /// <summary> Returns a list of the feature types known to the system. Useful for populating a GUI. </summary>
        static std::vector<std::string> GetRegisteredTypes();
        
        using FeatureMap = std::unordered_map<std::string, Feature*>;
        using DeserializeFunction = std::function<std::unique_ptr<Feature>(std::vector<std::string>, FeatureMap&)>; // TODO: have creation (deserialization) function take a const FeatureMap&

        /// <summary> 
        /// Adds a feature to our list of dependent features. Not meant to be called by client code.
        /// We need to keep track of dependents for two reasons:
        ///   1) traversing the whole graph (not just the active graph)
        ///   2) propagating the 'dirty' flag when new input arrives  
        /// </summary> 
        void AddDependent(Feature* f); // TODO: figure out how to make this protected

    protected:
        /// <summary> Virtual methods that implement feature-dependent things </summary>
        virtual std::vector<double> ComputeOutput() const = 0;
        virtual void AddToDescription(std::vector<std::string>& description) const {};
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const = 0;        
        virtual std::string FeatureType() const = 0;
        
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
        InputFeature* FindInputFeature() const; // TODO: Remove this, it's unnecessary now (?)

        mutable bool _isDirty = true;
        std::vector<Feature*> _dependents; // children
        std::string _id;
        int _instanceId = 0;

        static int _instanceCount;
        static std::unordered_map<std::string, DeserializeFunction> _createTypeMap;
    };

    //
    // RegisteredFeature
    //
    // Superclass for any serializable feature. This class uses the CRTP, so for any subclass, the template parameter is the subclass itself.
    // e.g., if MyFeature is a subclass of RegisteredFeature<>, it needs to be declared as: class MyFeature : public RegisteredFeature<MyFeature>
    template <typename FeatureT>
    class RegisteredFeature : public Feature
    {
    public:
        static void RegisterFeature();
        
    protected:
        virtual std::string FeatureType() const final;

        RegisteredFeature(const std::vector<Feature*>& inputs);
        
        // Allocates a new unique_ptr to a feature of type FeatureType
        template <typename ... Args>
        static std::unique_ptr<FeatureT> Allocate(const std::vector<Feature*>& inputs, Args... args);

    };
}

#include "../tcc/Feature.tcc"
