// DecisionTreePath.h
#pragma once

#include "Mapping.h"
#include <vector>

using std::vector;

namespace mappings
{
    
    class DecisionTreePath : public Mapping
    {
    public:

        /// Represents a pair of Children of a binary tree
        ///
        class Childs
        {
        public:

            /// Default constructor
            ///
            Childs();

            /// Constructs the pair of Children
            ///
            Childs(int child0, int child1);

            /// \returns Child 0
            ///
            int get_child0() const;

            /// \returns Child 1
            ///
            int get_child1() const;

            /// Serializes the Mapping in json format
            ///
            virtual void Serialize(JsonSerializer& js) const;

            /// Deserializes the Mapping in json format
            ///
            virtual void Deserialize(JsonSerializer& js);

        private:
            int _child0;
            int _child1;
        };

        /// Default constructor
        ///
        DecisionTreePath();

        /// Constructs a decision tree from an iterator that has methods ...
        ///
        template<typename ChildrenIteratorType>
        DecisionTreePath(ChildrenIteratorType iter, ChildrenIteratorType end, int input_index_offset);

        /// \returns A reference to the Children vector
        ///
        vector<Childs>& Children();

        /// \returns A reference to the Children vector
        ///
        const vector<Childs>& Children() const;

        /// \returns The input index offset
        ///
        int GetInputIndexOffset() const;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the apply function assumes that the output array is at least this long
        ///    
        virtual int GetOutputDim() const;

        /// \returns The number of vertices
        ///
        int GetNumVertices() const;

        /// \returns The number of interior vertices
        ///
        int GetNumInteriorVertices() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& js) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& js, int version = _current_version);

    protected:
        vector<Childs> _children;
        int _input_index_offset;

        static const int _current_version = 1;
    };
}

#include "../tcc/DecisionTreePath.tcc"
