// DecisionTreePath.cpp

#include "DecisionTreePath.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;
using std::fill;

namespace mappings
{

    DecisionTreePath::ChildPair::ChildPair()
    {}

    DecisionTreePath::ChildPair::ChildPair(int child0, int child1) :
        _child0(child0), _child1(child1)
    {}

    int DecisionTreePath::ChildPair::GetChild0() const
    {
        return _child0;
    }

    int DecisionTreePath::ChildPair::GetChild1() const
    {
        return _child1;
    }

    void DecisionTreePath::ChildPair::Serialize(JsonSerializer& serializer) const
    {
        serializer.Write("child0", _child0);
        serializer.Write("child1", _child1);
    }

    void DecisionTreePath::ChildPair::Deserialize(JsonSerializer& serializer)
    {
        serializer.Read("child0", _child0);
        serializer.Read("child1", _child1);
    }

    DecisionTreePath::DecisionTreePath() 
    {
        _type = Mapping::types::decisionTreePath;
    }

    vector<DecisionTreePath::ChildPair>& DecisionTreePath::Children()
    {
        return _children;
    }

    const vector<DecisionTreePath::ChildPair>& DecisionTreePath::Children() const
    {
        return _children;
    }

    int DecisionTreePath::GetInputIndexOffset() const
    {
        return _input_index_offset;
    }

    void DecisionTreePath::Apply(const double* input, double* output) const
    {
        // set the vector to zero
        fill(output, output+GetOutputDim(), 0.0);

        // set indicators for interior nodes
        int index = 0;
        while (index < GetNumInteriorVertices())
        {
            output[index] = 1.0;
            double input_value = input[_input_index_offset + index];

            if(input_value <= 0)
            {
                index = _children[index].GetChild0();
            }
            else
            {
                index = _children[index].GetChild1();
            }
        }

        // set indicator for leaf
        output[index] = 1.0;
    }

    int DecisionTreePath::GetMinInputDim() const
    {
        return _input_index_offset + GetNumInteriorVertices();
    }

    int DecisionTreePath::GetOutputDim() const
    {
        return GetNumVertices();
    }

    int DecisionTreePath::GetNumVertices() const
    {
        return GetNumInteriorVertices() * 2 + 1;
    }

    int DecisionTreePath::GetNumInteriorVertices() const
    {
        return (int)_children.size();
    }
    
    void DecisionTreePath::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("Children", _children);
        serializer.Write("input_index_offset", _input_index_offset);
    }

    void DecisionTreePath::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("Children", _children);
            serializer.Read("input_index_offset", _input_index_offset);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
