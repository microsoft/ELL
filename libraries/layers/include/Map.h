// Map.h

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// common
#include "types.h"

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
            shared_ptr<vector<vector<double>>> _spOutputs;
            const vector<Coordinate>& _outputCoordinates;
            uint64 _index;

            /// private ctor, can only be called from Map class
            Iterator(shared_ptr<vector<vector<double>>> spOutput, const vector<Coordinate>& outputCoordinates);
            friend Map;
        };

        /// Ctor
        ///
        Map() = default;
        
        /// Ctor
        ///
        Map(uint64 inputLayerSize);
    
        /// Computes the Map
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
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

        /// reads a Map from a stream
        ///
        template<typename MapType = Map>
        static shared_ptr<MapType> Deserialize(istream& is); // TODO: why does this return a shared_ptr, while the dataset loader returns a moved object

        /// Static function for deserializing shared_ptr<Layer>
        ///
        static void DeserializeLayers(JsonSerializer& serializer, shared_ptr<Layer>& up);

    protected:
        vector<shared_ptr<Layer>> _layers;

        shared_ptr<vector<vector<double>>> AllocateOutputs() const;
    };
}

#include "../tcc/Map.tcc"

