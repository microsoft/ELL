// DecisionTreePath.cpp

#include "DecisionTreePath.h"
#include <stdexcept>
#include <string>

using std::runtime_error;
using std::to_string;
using std::fill;

namespace mappings
{

    DecisionTreePath::Childs::Childs()
    {}

    DecisionTreePath::Childs::Childs(int child0, int child1) :
        _child0(child0), _child1(child1)
    {}

    int DecisionTreePath::Childs::get_child0() const
    {
        return _child0;
    }

    int DecisionTreePath::Childs::get_child1() const
    {
        return _child1;
    }

    void DecisionTreePath::Childs::Serialize(JsonSerializer& js) const
    {
        js.write("child0", _child0);
        js.write("child1", _child1);
    }

    void DecisionTreePath::Childs::Deserialize(JsonSerializer& js)
    {
        js.read("child0", _child0);
        js.read("child1", _child1);
    }

    DecisionTreePath::DecisionTreePath() 
    {
        _type = Mapping::types::DecisionTreePath;
    }

    vector<DecisionTreePath::Childs>& DecisionTreePath::Children()
    {
        return _children;
    }

    const vector<DecisionTreePath::Childs>& DecisionTreePath::Children() const
    {
        return _children;
    }

    int DecisionTreePath::GetInputIndexOffset() const
    {
        return _input_index_offset;
    }

    void DecisionTreePath::apply(const double* input, double* output) const
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
                index = _children[index].get_child0();
            }
            else
            {
                index = _children[index].get_child1();
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
    
    void DecisionTreePath::Serialize(JsonSerializer& js) const
    {
        // version 1
        Mapping::SerializeHeader(js, 1);
        js.write("Children", _children);
        js.write("input_index_offset", _input_index_offset);
    }

    void DecisionTreePath::Deserialize(JsonSerializer& js, int version)
    {
        if (version == 1)
        {
            js.read("Children", _children);
            js.read("input_index_offset", _input_index_offset);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
