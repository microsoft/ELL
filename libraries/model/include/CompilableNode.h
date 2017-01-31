////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"

// utilities
#include "TypeName.h"

// stl
#include <string>

namespace ell
{
namespace model
{
    class MapCompiler;
    class IRMapCompiler;

    class CompilableNode : public Node
    {
    public:
        /// <summary> Compile the node with the given compiler </summary>
        ///
        /// <param name="compiler"> The compiler to use when compiling the node </param>
        virtual void Compile(MapCompiler& compiler);

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const { return true; }

    protected:
        CompilableNode(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs) : Node(inputs, outputs) {}
        virtual void Compile(IRMapCompiler& compiler) = 0;
        virtual ~CompilableNode() = default;
    };
}
}

// #include "../tcc/CompilableNode.tcc"
