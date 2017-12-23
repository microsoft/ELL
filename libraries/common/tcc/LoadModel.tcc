////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadModel.tcc (common)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Archiver.h"
#include "Files.h"
#include "JsonArchiver.h"
#include "Exception.h"

namespace ell
{
namespace common
{
    // STYLE internal use only from .tcc, so not declared inside header file
    template <typename UnarchiverType>
    model::Map LoadArchivedMap(std::istream& stream)
    {
        try
        {
            utilities::SerializationContext context;
            RegisterNodeTypes(context);
            RegisterMapTypes(context);
            UnarchiverType unarchiver(stream, context);
            model::Map map;
            unarchiver.Unarchive(map);
            return map;
        }
        catch (const ell::utilities::Exception& ex)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read file: " + ex.GetMessage());
        }
    }
}
}