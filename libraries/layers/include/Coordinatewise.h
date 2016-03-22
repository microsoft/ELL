////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinatewise.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// types
#include "types.h"

// stl
#include <vector>
#include <functional>
#include <string>

namespace layers
{
    /// <summary> A class that represents a layer that performs a coordinatewise operation. </summary>
    class Coordinatewise : public Layer
    {
    public:

        enum class OperationType {add, multiply};

        /// <summary> Constructs an instance of a Coordinatewise layer. </summary>
        Coordinatewise() = default;

        /// <summary> Constructs an single-element instance of a Coordinatewise layer. </summary>
        ///
        /// <param name="value"> The element's value. </param>
        /// <param name="coordinate"> The element's input coordinate. </param>
        /// <param name="operationType"> The type of Coordinatewise layer to construct. </param>
        Coordinatewise(double value, Coordinate coordinate, OperationType operationType);

        /// <summary> Constructs an instance of a Coordinatewise layer. </summary>
        ///
        /// <param name="values"> The value for each element. </param>
        /// <param name="coordinates"> The input coordinate for each element. </param>
        /// <param name="operationType"> The type of Coordinatewise layer to construct. </param>
        Coordinatewise(const std::vector<double>& values, const CoordinateList& coordinates, OperationType operationType);

        /// <summary> Default virtual destructor. </summary>
        virtual ~Coordinatewise() = default;

        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64 Size() const override;

        static const std::string GetOperationName(OperationType type);
        static OperationType GetOperationType(const std::string& name);
        static std::function<double(double, double)> GetOperation(OperationType type);
        OperationType GetOperationType() const;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="layerIndex"> The layer's own zero-based index in the map. </param>
        /// <param name="outputs"> [in,out] The vector where the output values are written. </param>
        virtual void Compute(uint64 layerIndex, std::vector<std::vector<double>>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual Layer::InputCoordinateIterator GetInputCoordinates(uint64 index) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static const char* GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual const char* GetRuntimeTypeName() const override;

        /// <summary> Reads the map from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) override;

        /// <summary> Writes the map to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const override;

        /// <summary> Makes a deep copy of this Layer. </summary>
        ///
        /// <returns> A copy of this Layer. </returns>
        virtual std::unique_ptr<Layer> Clone() const override;

    protected:
        std::vector<double> _values;
        CoordinateList _inputCoordinates;
        OperationType _operationType;
        static const int _currentVersion = 1;
    };
}
