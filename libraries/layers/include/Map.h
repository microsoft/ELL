// Map.h

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// common
#include "types.h"
#include "RealArray.h"

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include <iostream>
using std::istream;

namespace layers
{
    class Map
    {
    public:

        class Iterator : public IIndexValueIterator
        {
        public:

            /// Default copy ctor
            ///
            Iterator(const Iterator&) = default;

            /// Default move ctor
            ///
            Iterator(Iterator&&) = default;

            /// \returns True if the iterator is currently pointing to a valid iterate
            ///
            bool IsValid() const;

            /// Proceeds to the Next iterate
            ///
            void Next();

            /// \returns The current index-value pair
            ///
            IndexValue Get() const;

        protected:
            shared_ptr<vector<DoubleArray>> _spOutputs;
            const vector<Coordinate>& _outputCoordinates;
            uint64 _index;

            /// private ctor, can only be called from Map class
            Iterator(shared_ptr<vector<DoubleArray>> spOutput, const vector<Coordinate>& outputCoordinates);
            friend Map;
        };

        /// Ctor
        ///
        Map() = default;
        
        /// Ctor
        ///
        Map(uint64 inputLayerSize);

        /// default copy ctor
        ///
        Map(const Map&) = default;

        /// default move ctor
        ///
        Map(Map&&) = default;

        /// Default assignment operator
        ///
        Map& operator=(const Map&) = default;

        /// Default assignment move operator
        ///
        Map& operator=(Map&&) = default;

        /// Computes the Map
        ///
        template<typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        Iterator Compute(IndexValueIteratorType IndexValueIterator, const vector<Coordinate>& outputCoordinates) const;

        /// Adds a shared layer to the map
        /// \returns The row index of the added layer
        uint64 PushBack(shared_ptr<Layer> layer);

        /// \returns The number of layers in the map
        ///
        uint64 NumLayers() const;

        /// \returns The number of elements in a specified layer
        ///
        uint64 LayerSize(uint64 index) const;

        /// Serializes the Map in json format
        ///
        void Serialize(JsonSerializer& serializer) const;

        /// Serializes the Map in json format
        ///
        void Serialize(ostream& os) const;

        /// Deserializes the Map in json format
        ///
        void Deserialize(JsonSerializer& serializer);

        /// Static function for deserializing shared_ptr<Layer>
        ///
        static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up);

    protected:
        vector<shared_ptr<Layer>> _layers;

        shared_ptr<vector<DoubleArray>> AllocateOutputs() const;
    };
}

#include "../tcc/Map.tcc"

