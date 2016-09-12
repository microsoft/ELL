////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%include "stl.i"

%ignore emll::utilities::operator<<;
%ignore emll::utilities::MakeAnyIterator;
%ignore emll::utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include "AnyIterator.h"
#include "RandomEngines.h"
#include "StlIterator.h"
#include "ObjectArchive.h"
#include "IArchivable.h"
#include "Archiver.h"
#include "UniqueId.h"
#include "Variant.h"

#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
#include "LinearPredictor.h"
#include "SGDIncrementalTrainer.h"
%}

// SWIG can't interpret StlIterator.h, so we need to include a simpler signature of the class
template <typename IteratorType, typename ValueType>
class utilities::StlIterator
{
public:

    StlIterator();
    StlIterator(IteratorType begin, IteratorType end);
    bool IsValid() const;
    bool HasSize() const;
    uint64_t NumIteratesLeft() const;
    void Next();
    const ValueType& Get() const;
};

template <typename IteratorType, typename ValueType> class emll::utilities::StlIterator {};

%import "UniqueId.h"
%import "ObjectArchive.h"
%import "Archiver.h"


%include "AnyIterator.h"
%include "RandomEngines.h"
%include "RowDataset.h"

%include "SGDIncrementalTrainer_wrap.h"

%include "IArchivable.h"
%include "UniqueId.h"
%include "Variant.h"

WRAP_OSTREAM_OUT_TO_STR(emll::utilities::UniqueId)

// This is necessary for us to avoid leaking memory:
#ifndef SWIGXML
%template () std::vector<emll::dataset::GenericSupervisedExample>;
%template () emll::utilities::StlIterator<typename std::vector<emll::dataset::GenericSupervisedExample>::const_iterator, emll::dataset::GenericSupervisedExample>;
#endif

%include "LogLoss.h"
%include "HingeLoss.h"
%include "SquaredLoss.h"

%template () emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::LogLoss>;
%template () emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::HingeLoss>;
%template () emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::SquaredLoss>;

typedef emll::predictors::LinearPredictor emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::SquaredLoss>::Predictor;
