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
        constexpr utilities::ArchiveVersion currentArchiveVersion = { utilities::ArchiveVersionNumbers::v2 };
    } // namespace

    InputNodeBase::InputNodeBase(OutputPortBase& output) :
        CompilableNode({}, { &output }),
        _outputBase(output)
    {
    }

    InputNodeBase::InputNodeBase(InputPortBase& input, OutputPortBase& output) :
        CompilableNode({ &input }, { &output }),
        _outputBase(output)
    {
    }

    MemoryShape InputNodeBase::GetShape() const
    {
        return _outputBase.GetMemoryLayout().GetActiveSize();
    }

    void InputNodeBase::SetShape(const MemoryShape& shape)
    {
        _outputBase.SetMemoryLayout({ shape });
    }

    PortMemoryLayout InputNodeBase::GetMemoryLayout() const
    {
        return _outputBase.GetMemoryLayout();
    }

    void InputNodeBase::SetMemoryLayout(const PortMemoryLayout& layout)
    {
        _outputBase.SetMemoryLayout(layout);
    }

    ell::utilities::ArchiveVersion InputNodeBase::GetArchiveVersion() const
    {
        return std::max(currentArchiveVersion, CompilableNode::GetArchiveVersion());
    }

    bool InputNodeBase::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version >= currentArchiveVersion && CompilableNode::CanReadArchiveVersion(version);
    }
} // namespace model
} // namespace ell
