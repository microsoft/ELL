////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model.i (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{

#ifdef SWIGPYTHON
#ifndef SWIG
#include <model/include/Map.h>
std::vector<void*> GetInputBuffersFromList(std::shared_ptr<ell::model::Map> map, PyObject *list);
std::vector<void*> GetOutputBuffersFromList(std::shared_ptr<ell::model::Map> map, PyObject *list);
#endif // SWIG
#endif // SWIGPYTHON

#include "Ports.h"
#include "ModelInterface.h"
#include "ModelBuilderInterface.h"

#include <utilities/include/Variant.h>
#include <utilities/include/StringUtil.h>
#include <vector>
%}

// naturalvar declarations for members that are object types (see ..\Readme.md)
%naturalvar ELL_API::PortMemoryLayout::size;
%naturalvar ELL_API::PortMemoryLayout::extent;
%naturalvar ELL_API::PortMemoryLayout::offset;
%naturalvar ELL_API::PortMemoryLayout::order;

// Include the C++ code to be wrapped
%include "Ports.h"
%include "ModelInterface.h"
%include "ModelBuilderInterface.h"
%include "macros.i"

// Template instantiations
%template(StepDouble) ELL_API::CompiledMap::Step<double>;
%template(StepFloat) ELL_API::CompiledMap::Step<float>;

%template(RegisterCallbackDouble) ELL_API::SinkNode::RegisterCallback<double>;
%template(RegisterCallbackFloat) ELL_API::SinkNode::RegisterCallback<float>;
%template(RegisterCallbackInt) ELL_API::SinkNode::RegisterCallback<int>;
%template(RegisterCallbackInt64) ELL_API::SinkNode::RegisterCallback<int64_t>;
%template(RegisterCallbackBoolean) ELL_API::SinkNode::RegisterCallback<bool>;

%template(RegisterCallbackDouble) ELL_API::SourceNode::RegisterCallback<double>;
%template(RegisterCallbackFloat) ELL_API::SourceNode::RegisterCallback<float>;
%template(RegisterCallbackInt) ELL_API::SourceNode::RegisterCallback<int>;
%template(RegisterCallbackInt64) ELL_API::SourceNode::RegisterCallback<int64_t>;
%template(RegisterCallbackBoolean) ELL_API::SourceNode::RegisterCallback<bool>;

%template(StepDouble) ELL_API::Map::Step<double>;
%template(StepFloat) ELL_API::Map::Step<float>;

#ifndef SWIGXML
%include "std_vector.i"

%template(PortElementsList) std::vector<ELL_API::PortElements*>;
%template(InputNodeList) std::vector<ELL_API::InputNode*>;
%template(OutputNodeList) std::vector<ELL_API::OutputNode*>;
%template(CallbackInfoList) std::vector<ELL_API::CallbackInfo>;

#endif // SWIGXML

#ifdef SWIGPYTHON
    %include "model_python_post.i"
#elif SWIGJAVASCRIPT
    %include "model_javascript_post.i"
#endif
