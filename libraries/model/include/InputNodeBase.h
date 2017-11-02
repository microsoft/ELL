////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNodeBase.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableNode.h"
#include "InputPort.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "OutputPort.h"

// math
#include "Tensor.h"

// utilities
#include "IArchivable.h"
#include "TypeName.h"

#include <memory>
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    /// <summary> Base class for a node that represents an input to the system. </summary>
    class InputNodeBase : public CompilableNode
    {
    public:
        /// <summary> Gets the output port. </summary>
        ///
        /// <returns> The output port. </returns>
        const OutputPortBase& GetOutputPort() const { return _outputBase; }

        /// <summary> Returns the dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() { return _outputBase.Size(); }

        /// <summary> Gets the output type of this node </summary>
        ///
        /// <returns> The output type of this node </returns>
        Port::PortType GetOutputType() const { return _outputBase.GetType(); }

        /// <summary> Gets the output shape </summary>
        ///
        /// <returns> The output shape </returns>
        math::TensorShape GetShape() const { return _shape; }

    protected:
        InputNodeBase(OutputPortBase& output, ell::math::TensorShape shape);
        virtual bool ShouldCompileInline() const override { return true; }
        virtual bool HasState() const override { return false; }
        void SetShape(const ell::math::TensorShape& shape) { _shape = shape; }  // STYLE discrepancy
        virtual ell::utilities::ArchiveVersion GetArchiveVersion() const override;
    private:
        OutputPortBase& _outputBase;
        ell::math::TensorShape _shape;
    };

}
}
