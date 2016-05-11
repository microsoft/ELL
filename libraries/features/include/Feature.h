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
    struct ctor_enable {}; // TODO: figure out how to make this be a protected member of Feature without SWIG complaining

    /// <summary> Base class for features </summary>
    class Feature : public std::enable_shared_from_this<Feature>
    {
    public:
        Feature();
        Feature(std::string id);
        Feature(const std::vector<std::shared_ptr<Feature>>& inputs);
        Feature(std::string Id, const std::vector<std::shared_ptr<Feature>>& inputs);
        virtual ~Feature() {};

        /// <summary> The unique ID of this feature. </summary>
        std::string Id() const;
        
        /// <summary> The dimensionality of this feature. </summary>
        size_t NumColumns() const;
        
        /// <summary> Indicates if this feature has new output to be read. </summary>
        virtual bool HasOutput() const;

        /// <summary> The current output of this feature. </summary>
        std::vector<double> GetOutput() const;

        /// <summary> Resets any internal state </summary>
        virtual void Reset();
        
        /// <summary> Amount of time it takes for this feature to respond to input. </summary>
        virtual size_t WarmupTime() const;

        std::vector<std::string> GetDescription() const;
        virtual std::vector<std::string> GetColumnDescriptions() const;
        const std::vector<std::shared_ptr<Feature>>& GetInputFeatures() const;

        static std::vector<std::string> GetRegisteredTypes();
        
        using FeatureMap = std::unordered_map<std::string, std::shared_ptr<Feature>>;
        using DeserializeFunction = std::function<std::shared_ptr<Feature>(std::vector<std::string>, FeatureMap&)>; // TODO: have creation function take a const FeatureMap&

        void AddDependent(std::shared_ptr<Feature> f); // TODO: figure out how to make this protected

    protected:
        virtual std::vector<double> ComputeOutput() const = 0;
        virtual void AddToDescription(std::vector<std::string>& description) const {};
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const = 0;        
        virtual std::string FeatureType() const = 0;
        
        static void RegisterDeserializeFunction(std::string class_name, DeserializeFunction create_fn);
        bool IsDirty() const;
        void SetDirtyFlag(bool dirty) const;
        void AddInputFeature(std::shared_ptr<Feature> inputFeature);

        std::vector<std::shared_ptr<Feature>> _inputFeatures; // parents
        mutable size_t _numColumns = 0; // TODO: rename to _size (?)
        mutable std::vector<double> _cachedValue;

    private:
        friend class FeatureSet;
        void Serialize(std::ostream& outStream) const;
        static std::shared_ptr<Feature> FromDescription(const std::vector<std::string>& description, FeatureMap& deserializedFeatureMap);
        std::shared_ptr<InputFeature> FindInputFeature() const; // TODO: Remove this, it's unnecessary now (?)

        mutable bool _isDirty = true;
        std::vector<std::shared_ptr<Feature>> _dependents; // children
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

        RegisteredFeature(const std::vector<std::shared_ptr<Feature>>& inputs);
        
        // Allocates a new shared_ptr to a feature of type FeatureType
        template <typename ... Args>
        static std::shared_ptr<FeatureT> Allocate(const std::vector<std::shared_ptr<Feature>>& inputs, Args... args);

    };
}

#include "../tcc/Feature.tcc"
