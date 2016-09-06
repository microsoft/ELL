////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%include "stl.i"

%ignore utilities::operator<<;
%ignore utilities::MakeAnyIterator;
%ignore utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include "AnyIterator.h"
#include "RandomEngines.h"
#include "StlIterator.h"
#include "ISerializable.h"
#include "Serializer.h"
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

%include "AnyIterator.h"
%include "RandomEngines.h"
%include "RowDataset.h"

%include "SGDIncrementalTrainer_wrap.h"

%import "Serializer.h"
%include "ISerializable.h"
%include "UniqueId.h"
%include "Variant.h"

WRAP_OSTREAM_OUT_TO_STR(utilities::UniqueId)

// This is necessary for us to avoid leaking memory:
#ifndef SWIGXML
%template () std::vector<dataset::GenericSupervisedExample>;
%template () utilities::StlIterator<typename std::vector<dataset::GenericSupervisedExample>::const_iterator, dataset::GenericSupervisedExample>;
#endif

%include "LogLoss.h"
%include "HingeLoss.h"
%include "SquaredLoss.h"

%template () trainers::SGDIncrementalTrainer<lossFunctions::LogLoss>;
%template () trainers::SGDIncrementalTrainer<lossFunctions::HingeLoss>;
%template () trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>;

typedef predictors::LinearPredictor trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>::Predictor;

// TODO: wrap print