////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FeatureSet.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FeatureSet.h"
#include "Feature.h"
#include "InputFeature.h"
#include "StringUtil.h"
#include "DoubleVector.h"

#include <vector>
#include <string>
#include <sstream>
#include <cassert>

namespace features
{
    //
    // FeatureSet class
    //
    FeatureSet::FeatureSet() : _numItemsProcessed(0) {}

    void FeatureSet::Reset()
    {
        _numItemsProcessed = 0;
        if (_outputFeature)
        {
            _outputFeature->Reset();
        }
    }

    bool FeatureSet::ProcessInputData(const DataVector& inst) const
    {
        if (_inputFeature && _outputFeature)
        {
            _inputFeature->SetValue(static_cast<std::vector<double>>(inst)); // TODO: standardize on a vector type to use
            auto newFeatures = _outputFeature->Eval();
            auto hasOutput = HasOutput();
            ++_numItemsProcessed;
            return hasOutput;
        }

        return false;
    }
    
    bool FeatureSet::HasOutput() const
    {
        return _numItemsProcessed >= _outputFeature->WarmupTime() && _outputFeature->HasOutput();    
    }
    
    DataVector FeatureSet::GetOutput() const
    {
        return _outputFeature->Eval();    
    }
    
    void FeatureSet::Deserialize(std::istream& inStream)
    {
        // read in lines 1 at a time
        std::string lineBuffer;
        std::shared_ptr<Feature> lastFeature = nullptr;
        while (std::getline(inStream, lineBuffer))
        {
            lineBuffer = TrimString(lineBuffer);
            if (lineBuffer[0] == '#') // skip comments
            {
                continue;
            }

            std::stringstream lineStream(lineBuffer);

            // split each line into a std::vector of strings
            std::vector<std::string> description;
            std::string field;
            while (getline(lineStream, field, '\t'))
            {
                description.push_back(field);
            }

            if (description.size() == 0)
            {
                continue;
            }
            else if (description.size() < 2)
            {
                throw std::runtime_error("Malformed feature specification for line: " + lineBuffer);
            }

            std::string featureId = TrimString(description[0]);
            std::string featureClass = TrimString(description[1]);

            auto createFunction = Feature::_createTypeMap[featureClass];
            if (createFunction == nullptr)
            {
                std::string error_msg = std::string("Error deserializing feature description: unknown feature type '") + featureClass + "'";
                throw std::runtime_error(error_msg);
            }

            lastFeature = createFunction(description, _featureMap);
            // TODO: throw an exception if new feature's ID isn't unique
            _featureMap[featureId] = lastFeature;
        }

        _outputFeature = lastFeature;
        if (_outputFeature != nullptr)
        {
            _inputFeature = _outputFeature->FindInputFeature();
        }
        else
        {
            _inputFeature = nullptr;
        }        
    }
    
    void FeatureSet::Serialize(std::ostream& outStream) const
    {
        // TODO: actually serialize all the features in the map
        if(_outputFeature != nullptr)
        {
            SerializeActiveGraph(outStream);
        }        
        else
        {
            throw std::runtime_error("No output feature to serialize");
        }
    }
 
    void FeatureSet::SerializeActiveGraph(std::ostream& outStream) const
    {
        if(_outputFeature != nullptr)
        {
            // Testing: write header with delay for each column
            outStream << "#delay:";
            for (int columnIndex = 0; columnIndex < _outputFeature->NumColumns(); columnIndex++)
            {
                outStream << "\t" << _outputFeature->ColumnDelay(columnIndex);
            }
            outStream << std::endl;

            auto serializer = [&outStream](const Feature& f)
            {
                f.Serialize(outStream);
            };

            Visit(serializer);
        }
        else
        {
            throw std::runtime_error("No output feature to serialize");
        }
    }

    std::shared_ptr<InputFeature> FeatureSet::GetInputFeature() const
    {
        return _inputFeature;
    }

    std::shared_ptr<Feature> FeatureSet::GetOutputFeature() const
    {
        return _outputFeature;
    }
    
    void FeatureSet::SetOutputFeature(const std::shared_ptr<Feature>& output)
    {
        _outputFeature = output;
    }

    std::shared_ptr<Feature> FeatureSet::GetFeature(const std::string& featureId) const
    {
        auto it = _featureMap.find(featureId);
        if (it != _featureMap.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    std::vector<std::string> FeatureSet::GetFeatureIds() const
    {
        std::vector<std::string> result;

        for (const auto& f : _featureMap)
        {
            result.push_back(f.first);
        }
        return result;
    }



    std::shared_ptr<Feature> FeatureSet::CreateFeatureFromDescription(const std::vector<std::string>& description)
    {
        auto result = Feature::FromDescription(description, _featureMap);
        _featureMap[result->Id()] = result;
        return result;
    }

} // end namepsace
