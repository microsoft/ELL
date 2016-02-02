// Map.h

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// common
#include "types.h"
#include "RealArray.h"

// stl
#include <vector>
#include <memory>
#include <type_traits>

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
            std::shared_ptr<std::vector<types::DoubleArray>> _spOutputs;
            const CoordinateList _outputCoordinates;
            uint64 _index;

            /// private ctor, can only be called from Map class
            Iterator(std::shared_ptr<std::vector<types::DoubleArray>> spOutput, const CoordinateList& outputCoordinates);
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

        /// default virtual destructor
        ///
        virtual ~Map() = default;
        
        /// Default assignment operator
        ///
        Map& operator=(const Map&) = default;

        /// Default assignment move operator
        ///
        Map& operator=(Map&&) = default;

        /// Computes the Map
        ///
        template <typename IndexValueIteratorType, typename concept = std::enable_if_t<std::is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        Iterator Compute(IndexValueIteratorType IndexValueIterator, const CoordinateList& outputCoordinates) const;

        /// Adds a shared layer to the map
        /// \returns The row index of the added layer
        uint64 PushBack(std::shared_ptr<Layer> layer);

        /// \returns The number of layers in the map
        ///
        uint64 NumLayers() const;

        /// \returns a Layer of a specified template type
        ///
        template<typename LayerType = Layer>
        std::shared_ptr<const LayerType> GetLayer(uint64 layerIndex) const;

        /// \returns A list of the coordinates in this layer
        ///
        CoordinateList GetCoordinateList(uint64 layerIndex) const;

        /// Serializes the Map in json format
        ///
        void Serialize(utilities::JsonSerializer& serializer) const;

        /// Serializes the Map in json format
        ///
        void Serialize(ostream& os) const;

        /// Deserializes the Map in json format
        ///
        virtual void Deserialize(utilities::JsonSerializer& serializer);

        /// Static function for deserializing std::shared_ptr<Layer>
        ///
        static void DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<Layer>& up);

    protected:
        std::vector<std::shared_ptr<Layer>> _layers;

        std::shared_ptr<std::vector<types::DoubleArray>> AllocateOutputs() const;
    };
}

#include "../tcc/Map.tcc"
