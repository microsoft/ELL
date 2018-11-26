////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     nodes.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include <nodes/include/AccumulatorNode.h>
#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/DelayNode.h>
#include <nodes/include/DotProductNode.h>
#include <nodes/include/LinearPredictorNode.h>
#include <nodes/include/L2NormSquaredNode.h>
#include <nodes/include/MovingAverageNode.h>
#include <nodes/include/SumNode.h>
#include <nodes/include/UnaryOperationNode.h>
#include <nodes/include/MovingVarianceNode.h>
%}

%ignore ell::nodes::LinearPredictorSubModelOutputs;
%ignore ell::nodes::BuildSubModel;

%include <nodes/include/AccumulatorNode.h>
%include <nodes/include/BinaryOperationNode.h>
%include <nodes/include/ConstantNode.h>
%include <nodes/include/DelayNode.h>
%include <nodes/include/DotProductNode.h>
%include <nodes/include/LinearPredictorNode.h>
%include <nodes/include/L2NormSquaredNode.h>
%include <nodes/include/MovingAverageNode.h>
%include <nodes/include/SumNode.h>
%include <nodes/include/UnaryOperationNode.h>
%include <nodes/include/MovingVarianceNode.h>
