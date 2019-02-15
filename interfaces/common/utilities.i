////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%include "stl.i"

%ignore ell::utilities::operator<<;
%ignore ell::utilities::MakeAnyIterator;
%ignore ell::utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include <utilities/include/AnyIterator.h>
#include <utilities/include/RandomEngines.h>
#include <utilities/include/StlIterator.h>
#include <utilities/include/ObjectArchive.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/Archiver.h>
#include <utilities/include/JsonArchiver.h>
#include <utilities/include/XmlArchiver.h>
#include <utilities/include/UniqueId.h>
#include <utilities/include/Variant.h>
%}

// ignores
%ignore std::enable_if_t<>;
%ignore std::hash<ell::utilities::UniqueId>;

%ignore ell::utilities::JsonUtilities;
%ignore ell::utilities::Variant::Variant(ell::utilities::Variant&&);
%ignore ell::utilities::CompressedIntegerList;

namespace ell { namespace utilities {} };

// SWIG can't interpret StlIterator.h, so we need to include a simpler signature of the class
template <typename IteratorType, typename ValueType>
class ell::utilities::StlIterator
{
public:
    StlIterator();
    StlIterator(IteratorType begin, IteratorType end);
    bool IsValid() const;
    bool HasSize() const;
    size_t NumIteratesLeft() const;
    void Next();
    const ValueType& Get() const;
};

// SWIG can't interpret StlIndexValueIterator.h, so we need to include a simpler signature of the class
template <typename IteratorType, typename ValueType>
class ell::utilities::StlIndexValueIterator
{
public:
    StlIndexValueIterator();
    StlIndexValueIterator(IteratorType begin, IteratorType end);
    bool IsValid() const;
    bool HasSize() const;
    size_t NumIteratesLeft() const;
    void Next();
    linear::IndexValue Get() const;
};


// utilities
%include <utilities/include/TypeFactory.h>
%include <utilities/include/CompressedIntegerList.h>
%include <utilities/include/Archiver.h>
%include <utilities/include/Variant.h>
%include <utilities/include/ObjectArchive.h>
%include <utilities/include/IArchivable.h>
%include <utilities/include/JsonArchiver.h>
%include <utilities/include/XmlArchiver.h>
%include <utilities/include/UniqueId.h>
%include <utilities/include/Variant.h>
%include <utilities/include/AnyIterator.h>
%include <utilities/include/RandomEngines.h>

// wrap print
WRAP_OSTREAM_OUT_TO_STR(ell::utilities::UniqueId)
