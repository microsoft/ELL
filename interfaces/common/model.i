////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     model.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "Node.h"
#include "Model.h"
#include "Port.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "PortElements.h"
#include "InputNode.h"
#include "OutputNode.h"
%}

%nodefaultctor emll::model::NodeIterator;
%nodefaultctor emll::model::Node;
%nodefaultctor emll::model::Port;
%nodefaultctor emll::model::OutputPortBase;
%nodefaultctor emll::model::OutputPort<double>;
%nodefaultctor emll::model::InputPortBase;

%ignore std::hash<emll::model::PortElementBase>;
%ignore std::hash<emll::model::PortRange>;
%ignore emll::model::Model::ComputeOutput;
%ignore emll::model::InputPort::operator[];

%include "Port.h"
%include "OutputPort.h"
%include "PortElements.h"
%include "InputPort.h"
%include "Node.h"
%include "Model.h"
%include "InputNode.h"
%include "OutputNode.h"

%template (DoubleOutputPort) emll::model::OutputPort<double>;
%template (BoolOutputPort) emll::model::OutputPort<bool>;
%template () emll::model::PortElements<double>;
%template () emll::model::PortElements<bool>;

#ifndef SWIGXML
%template (NodeVector) std::vector<emll::model::Node*>;
%template (ConstNodeVector) std::vector<const emll::model::Node*>;
%template (PortVector) std::vector<emll::model::Port*>;
%template (InputPortVector) std::vector<emll::model::InputPortBase*>;
%template (OutputPortVector) std::vector<emll::model::OutputPortBase*>;

%template (DoubleInputNode) emll::model::InputNode<double>;
%template (BoolInputNode) emll::model::InputNode<bool>;
%template (DoubleOutputNode) emll::model::OutputNode<double>;

%template (DoubleInputNodeVector) std::vector<const emll::model::InputNode<double>*>;
%template (DoubleOutputNodeVector) std::vector<const emll::model::OutputNode<double>*>;

#endif

%extend emll::model::Model 
{
    // get input nodes
    std::vector<const emll::model::InputNode<double>*> GetDoubleInputNodes() const
    {
        return $self->GetNodesByType<emll::model::InputNode<double>>();
    }

    // get output nodes
    std::vector<const emll::model::OutputNode<double>*> GetDoubleOutputNodes() const
    {
        return $self->GetNodesByType<emll::model::OutputNode<double>>();
    }

    // compute output
    std::vector<double> ComputeDoubleOutput(const OutputPort<double>& outputPort) const
    {
        return $self->ComputeOutput(outputPort);
    }
}
