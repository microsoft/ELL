////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
#include "L2NormNode.h"
#include "MovingAverageNode.h"
#include "SumNode.h"
#include "UnaryOperationNode.h"
#include "MovingVarianceNode.h"
%}

%ignore nodes::LinearPredictorSubModelOutputs;
%ignore nodes::BuildSubModel;

%rename (ModelGraph) model::Model;

%include "AccumulatorNode.h"
%include "BinaryOperationNode.h"
%include "ConstantNode.h"
%include "DelayNode.h"
%include "DotProductNode.h"
%include "LinearPredictorNode.h"
%include "L2NormNode.h"
%include "MovingAverageNode.h"
%include "SumNode.h"
%include "UnaryOperationNode.h"
%include "MovingVarianceNode.h"
