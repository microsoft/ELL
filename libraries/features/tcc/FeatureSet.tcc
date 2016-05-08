////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FeatureSet.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unordered_set>
#include <stdexcept>

namespace
{
    template <typename ContainerType>
    class ReverseRange
    {
    public:
        ReverseRange(ContainerType& container) : _begin(container.rbegin()), _end(container.rend()) {}
        typename ContainerType::reverse_iterator begin() const { return _begin; }
        typename ContainerType::reverse_iterator end() const { return _end; }
    
    private:
        typename ContainerType::reverse_iterator _begin;
        typename ContainerType::reverse_iterator _end;    
    };
    
    template <typename ContainerType>
    ReverseRange<ContainerType> Reverse(ContainerType& container)
    {
        return ReverseRange<ContainerType>(container);
    }
}

namespace features
{   
    template <typename FeatureType, typename... Args>
    std::shared_ptr<FeatureType> FeatureSet::CreateFeature(Args... args)
    {
        auto feature = FeatureType::Create(args...);

        if(_featureMap.size() == 0)
        {
            if(std::is_same<FeatureType, InputFeature>::value)
            {
                _inputFeature = std::dynamic_pointer_cast<InputFeature>(feature);
            }
            else
            {
                throw std::runtime_error("First feature must be input feature");
            }
        }
                
        // add to map
        _featureMap[feature->Id()] = std::shared_ptr<Feature>(feature);
        return feature;
    }
    
    template <typename Visitor>
    void FeatureSet::Visit(Visitor& visitor) const
    {
        if(_outputFeature == nullptr)
        {
            return;
        }
        
        std::unordered_set<std::shared_ptr<Feature>> visitedFeatures;
        std::vector<std::shared_ptr<Feature>> stack;

        // start with output feature in stack
        // TODO: when we have multiple output features, put them all in the stack
        stack.push_back(_outputFeature);
        while (stack.size() > 0)
        {
            std::shared_ptr<Feature> f = stack.back();

            // check if we've already visited this node
            if (visitedFeatures.find(f) != visitedFeatures.end()) 
            {
                stack.pop_back();
                continue;
            }

            // we can visit this node only if all its input features have been visited already
            bool canVisit = true; 
            for (auto input : f->_inputFeatures)
            {
                canVisit = canVisit && visitedFeatures.find(input) != visitedFeatures.end();
            }

            if (canVisit)
            {
                stack.pop_back();
                visitedFeatures.insert(f);
                
                // explicitly skip output feature (until we visit it manually at the end)                
                if(f != _outputFeature)
                {
                    visitor(*f);                    
                }
                
                // now add children (Note: this partis the only difference between Visit and VisitActiveGraph)
                for(const auto& child: Reverse(f->_dependents)) // TODO: really ought to push these in reverse order
                {
                    // note: this is kind of inefficient --- we're going to push multiple copies of child on the stack. But we'll check if we've visited it already when we pop it off. 
                    stack.push_back(child);
                }
            }
            else // visit f's parents
            {
                for (auto input : Reverse(f->_inputFeatures)) // TODO: really ought to push these in reverse order
                {
                    stack.push_back(input);
                }
            }
        }
        visitor(*_outputFeature);
    }
    
    template <typename Visitor>
    void FeatureSet::VisitActiveGraph(Visitor& visitor) const
    {
        if(_outputFeature == nullptr)
        {
            return;
        }
        
        std::unordered_set<std::shared_ptr<Feature>> visitedFeatures;
        std::vector<std::shared_ptr<Feature>> stack;

        // start with output feature in stack
        // TODO: when we have multiple output features, put them all in the stack (and put this code in FeatureSet)
        stack.push_back(_outputFeature);
        while (stack.size() > 0)
        {
            std::shared_ptr<Feature> f = stack.back();

            // check if we've already visited this node
            if (visitedFeatures.find(f) != visitedFeatures.end())
            {
                stack.pop_back();
                continue;
            }

            // we can visit this node only if all its input features have been visited already
            bool canVisit = true;
            for (auto input : f->_inputFeatures)
            {
                canVisit = canVisit && visitedFeatures.find(input) != visitedFeatures.end();
            }

            if (canVisit)
            {
                stack.pop_back();
                visitedFeatures.insert(f);
                
                visitor(*f);
            }
            else // visit f's input features
            {
                for (auto input : f->_inputFeatures)
                {
                    stack.push_back(input);
                }
            }
        }
    }
}
