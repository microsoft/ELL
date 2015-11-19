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
        class ChildPair
        {
        public:

            /// Default constructor
            ///
            ChildPair();

            /// Constructs the pair of Children
            ///
            ChildPair(uint64 child0, uint64 child1);

            /// \returns Child 0
            ///
            uint64 GetChild0() const;

            /// \returns Child 1
            ///
            uint64 GetChild1() const;

            /// Serializes the Mapping in json format
            ///
            virtual void Serialize(JsonSerializer& serializer) const;

            /// Deserializes the Mapping in json format
            ///
            virtual void Deserialize(JsonSerializer& serializer);

        private:
            uint64 _child0;
            uint64 _child1;
        };

        /// Default constructor
        ///
        DecisionTreePath();

        /// Constructs a decision tree from an iterator that has methods ...
        ///
        template<typename ChildrenIteratorType>
        DecisionTreePath(ChildrenIteratorType iter, ChildrenIteratorType end, uint64 input_index_offset);

        /// \returns A reference to the Children vector
        ///
        vector<ChildPair>& Children();

        /// \returns A reference to the Children vector
        ///
        const vector<ChildPair>& Children() const;

        /// \returns The input index offset
        ///
        uint64 GetInputIndexOffset() const;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual uint64 GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
        ///    
        virtual uint64 GetOutputDim() const;

        /// \returns The number of vertices
        ///
        uint64 GetNumVertices() const;

        /// \returns The number of interior vertices
        ///
        uint64 GetNumInteriorVertices() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _current_version);

    protected:
        vector<ChildPair> _children;
        uint64 _input_index_offset;

        static const int _current_version = 1;
    };
}

#include "../tcc/DecisionTreePath.tcc"
