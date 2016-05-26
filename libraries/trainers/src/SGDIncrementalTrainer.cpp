////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDIncrementalTrainer.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SGDIncrementalTrainer.h"

namespace trainers
{    
    trainers::SGDIncrementalTrainerParameters::SGDIncrementalTrainerParameters(double regularization) : regularization(regularization)
    {}
}
