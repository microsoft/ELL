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
    class Feature;
    using FeatureMap = std::unordered_map<std::string, std::shared_ptr<Feature>>;

    //
    // Feature base class
    //
    struct ctor_enable {}; // TODO: figure out how to make this be a protected member of Feature without SWIG complaining

    class Feature : public std::enable_shared_from_this<Feature>
    {
    public:
        Feature();
        Feature(std::string id);
        Feature(const std::vector<std::shared_ptr<Feature>>& inputs);
        Feature(std::string Id, const std::vector<std::shared_ptr<Feature>>& inputs);
        virtual ~Feature();

        std::string Id() const;
        size_t NumColumns() const;
        virtual bool HasOutput() const;
        std::vector<double> GetOutput() const;

        virtual void Reset();
        virtual size_t WarmupTime() const;
        virtual size_t ColumnDelay(int column) const;  // TODO: remove this?

        virtual std::vector<std::string> GetColumnDescriptions() const;
        virtual std::vector<std::string> GetDescription() const;
        const std::vector<std::shared_ptr<Feature>>& GetInputFeatures() const;

        static std::vector<std::string> GetRegisteredTypes();

        // TODO: get rid of this by adding its functionality to Allocate
        //       Allocate will have to take a vector of input features plus args...
        void AddDependent(std::shared_ptr<Feature> f); // TODO: figure out how to make this protected
        
    protected:
        // virtual methods:
        virtual std::vector<double> ComputeOutput() const = 0;
        virtual void AddDescription(std::vector<std::string>& description) const;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const = 0;

        virtual std::string FeatureType() const = 0;
        static void RegisterDeserializeFunction(std::string class_name, std::function<std::shared_ptr<Feature>(std::vector<std::string>, FeatureMap&)> create_fn);
        void SetDirty(bool dirty) const;
        bool IsDirty() const;
        void AddInputFeature(std::shared_ptr<Feature> inputFeature);
        std::vector<std::shared_ptr<Feature>> _inputFeatures; // parents
        mutable size_t _numColumns = 0; // TODO: rename to _size (?)
        mutable std::vector<double> _cachedValue;

    private:
        mutable bool _isDirty = true;
        std::vector<std::shared_ptr<Feature>> _dependents; // children
        mutable std::string _id;
        int _instanceId = 0;

        static int _instanceCount;
        // TODO: have creation function take a const FeatureMap&
        static std::unordered_map<std::string, std::function<std::shared_ptr<Feature>(std::vector<std::string>, FeatureMap&)>> _createTypeMap;

        std::shared_ptr<InputFeature> FindInputFeature() const;
        void Serialize(std::ostream& outStream) const;
        static std::shared_ptr<Feature> FromDescription(const std::vector<std::string>& description, FeatureMap& deserializedFeatureMap);
        friend class FeatureSet;
    };

    //
    // RegisteredFeature
    //
    // Superclass for any serializable feature. This class uses the CRTP, and its template parameter is the subclass itself.
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
