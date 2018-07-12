////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNodeBase.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNodeBase.h"

namespace ell
{
namespace model
{
    namespace
    {
        //
        // Relevant archive format versions
        //
        constexpr utilities::ArchiveVersion currentArchiveVersion = {utilities::ArchiveVersionNumbers::v2};
    }

    InputNodeBase::InputNodeBase(OutputPortBase& output, MemoryShape shape)
        : CompilableNode({}, { &output }), _outputBase(output)
    {
    }

    InputNodeBase::InputNodeBase(InputPortBase& input, OutputPortBase& output, MemoryShape shape)
        : CompilableNode({ &input }, { &output }), _outputBase(output)
    {
    }

    MemoryShape InputNodeBase::GetShape() const
    {
        return _outputBase.GetMemoryLayout().GetActiveSize();
    }

    void InputNodeBase::SetShape(const MemoryShape& shape)
    {
        _outputBase.SetMemoryLayout({shape});
    }

    ell::utilities::ArchiveVersion InputNodeBase::GetArchiveVersion() const
    {
        return std::max(currentArchiveVersion, CompilableNode::GetArchiveVersion());
    }

    bool InputNodeBase::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version >= currentArchiveVersion && CompilableNode::CanReadArchiveVersion(version);
    }
}
}
