////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCTNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "MapCompiler.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// math
#include "Matrix.h"

// utilities
#include "TypeName.h"
#include "TypeTraits.h"

// stl
#include <cmath>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs a real-valued discrete cosine transform (DCT) on its input </summary>
    ///
    /// <typeparam name="ValueType"> The element type. </typeparam>
    ///
    template <typename ValueType>
    class DCTNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DCTNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        /// <param name="numFilters"> The number of DCT filters to use. Also, the output dimension. </param>
        DCTNode(const model::PortElements<ValueType>& input, size_t numFilters);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DCTNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        void Copy(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // Stored state: size

    private:
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // DCT Matrix
        math::RowMatrix<ValueType> _dctCoeffs;
    };
}
}
