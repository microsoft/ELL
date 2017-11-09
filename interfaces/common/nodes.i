////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     nodes.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "LinearPredictorNode.h"
#include "L2NormSquaredNode.h"
#include "MovingAverageNode.h"
#include "SumNode.h"
#include "UnaryOperationNode.h"
#include "MovingVarianceNode.h"
%}

%ignore ell::nodes::LinearPredictorSubModelOutputs;
%ignore ell::nodes::BuildSubModel;

%include "AccumulatorNode.h"
%include "BinaryOperationNode.h"
%include "ConstantNode.h"
%include "DelayNode.h"
%include "DotProductNode.h"
%include "LinearPredictorNode.h"
%include "L2NormSquaredNode.h"
%include "MovingAverageNode.h"
%include "SumNode.h"
%include "UnaryOperationNode.h"
%include "MovingVarianceNode.h"
