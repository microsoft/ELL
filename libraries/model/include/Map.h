////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Map.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "TypeName.h"
#include "IArchivable.h"

// stl
#include <string>
#include <vector>
#include <array>
#include <utility> // for integer_sequence

namespace emll
{
namespace model
{
    /// <summary> Class that wraps a model and its designated outputs </summary>
    template <typename... OutputPortTypes>
    class Map // : utilities::IArchivable
    {
    public:
        Map(const Model& model,
            const std::tuple<PortElements<OutputPortTypes>...>& outputs,
            const std::array<std::string, sizeof...(OutputPortTypes)>& outputNames);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<OutputPortTypes...>("Map"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        const Model& GetModel() const { return _model; }

        /// <summary> Refines the model wrapped by this map </summary>
        void Refine(const TransformContext& context);

        /// <summary> Computes the output of the map from its current input values </summary>
        std::tuple<std::vector<OutputPortTypes...>> Compute() const;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        // virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        // virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        Model _model;

        std::array<std::string, sizeof...(OutputPortTypes)> _outputNames;
        std::tuple<PortElements<OutputPortTypes...>> _outputs;

    private:
        template <typename OutputElementsType>
        void RemapOutputElement(OutputElementsType& output, ModelTransformer& modelTransformer);

        template <size_t... Sequence>
        void RemapOutputElementsHelper(std::index_sequence<Sequence...>, ModelTransformer& modelTransformer);

        template <typename PortElementsType, typename OutputType>
        void ComputeElements(PortElementsType& elements, OutputType& output) const;

        template <size_t... Sequence>
        void ComputeElementsHelper(std::index_sequence<Sequence...>, std::tuple<std::vector<OutputPortTypes...>>& outputValues) const;
    };
}
}

#include "../tcc/Map.tcc"