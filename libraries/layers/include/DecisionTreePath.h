////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DecisionTreePath.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "CoordinateList.h"

// stl
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

namespace layers
{
    /// <summary> A class that represents a layer that represents the path indicator vector of a decision tree. </summary>
    class DecisionTreePath : public Layer
    {
    public:

        /// <summary> Constructs an instance of DecisionTreePath. </summary>
        ///
        /// <param name="edgeToInteriorNode"> A vector that maps incoming edge indices to interior node indices. </param>
        /// <param name="splitRuleCoordinates"> The coordinates of the input split rules that correspond to interior nodes. </param>
        DecisionTreePath(std::vector<uint64_t> edgeToInteriorNode, CoordinateList splitRuleCoordinates);
        
        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64_t GetOutputDimension() const override;

        /// <summary> Number of interior nodes in the tree. </summary>
        ///
        /// <returns> The number of interior nodes. </returns>
        uint64_t NumInteriorNodes() const;

        /// <summary> Gets the index of the negative outgoing edge from a given node. </summary>
        ///
        /// <param name="nodeIndex"> Zero-based index of the interior node. </param>
        ///
        /// <returns> The negative outgoing edge index. </returns>
        uint64_t GetNegativeOutgoingEdgeIndex(uint64_t interiorNodeIndex) const;

        /// <summary> Gets the index of the positive outgoing edge from a given node. </summary>
        ///
        /// <param name="nodeIndex"> Zero-based index of the interior node. </param>
        ///
        /// <returns> The index of the positive outgoing edge. </returns>
        uint64_t GetPositiveOutgoingEdgeIndex(uint64_t interiorNodeIndex) const;

        /// <summary> Gets the index of the interiorNode from the index of its incoming edge. </summary>
        ///
        /// <param name="incomingEdgeIndex"> Zero-based index of the incoming edge. </param>
        ///
        /// <returns> The index of the interiorNode, or zero if the edge points to a leaf. </returns>
        uint64_t GetInteriorNodeIndex(uint64_t incomingEdgeIndex) const;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="inputs"> The set of vectors holding input values to use </param>
        /// <param name="outputs"> [out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual CoordinateIterator GetInputCoordinateIterator(uint64_t index) const override;

        /// <summary> Returns the minimal required size of an input layer, which is the maximum input element from that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        virtual uint64_t GetRequiredLayerSize(uint64_t layerIndex) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Reads the layer from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) override;

        /// <summary> Writes the layer to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const override;

    protected:
        std::vector<uint64_t> _edgeToInteriorNode;
        CoordinateList _splitRuleCoordinates;
        static const int _currentVersion = 1;
    };
}
