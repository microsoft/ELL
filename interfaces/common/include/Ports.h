////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Ports.h (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

#include "MathInterface.h"
#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/OutputPort.h>
#include <model/include/Port.h>
#include <model/include/PortElements.h>
#include <model/include/PortMemoryLayout.h>

#endif

namespace ELL_API
{

class InputPort;
class OutputPort;
class Node;
class NodeIterator;
class PortMemoryLayout;

//
// PortType
//

enum class PortType : int
{
    none = (int)ell::model::Port::PortType::none,
    smallReal = (int)ell::model::Port::PortType::smallReal, // == float
    real = (int)ell::model::Port::PortType::real, // == double
    integer = (int)ell::model::Port::PortType::integer, // == int32
    bigInt = (int)ell::model::Port::PortType::bigInt, // == int64
    categorical = (int)ell::model::Port::PortType::categorical,
    boolean = (int)ell::model::Port::PortType::boolean
};

//
// Port
//

class Port
{
public:
    Port() = default;
    Node GetNode();
    std::string GetName();
    std::string GetRuntimeTypeName();
    PortType GetOutputType();
    int Size();
    PortMemoryLayout GetMemoryLayout();

#ifndef SWIG
    Port(const ell::model::Port* other, std::shared_ptr<ell::model::Model> model);
    const ell::model::Port& GetPort() const { return *_port; }
    std::shared_ptr<ell::model::Model> GetModel() const { return _model; }
#endif
private:
    const ell::model::Port* _port = nullptr;

protected:
    std::shared_ptr<ell::model::Model> _model;
};

//
// InputPortIterator
//

class InputPortIterator
{
public:
    InputPortIterator() = default;
    bool IsValid();
    void Next();
    InputPort Get();
#ifndef SWIG
    InputPortIterator(const std::vector<const ell::model::InputPortBase*>& ports, std::shared_ptr<ell::model::Model> model);
#endif
private:
    size_t _i = 0;
    std::vector<const ell::model::InputPortBase*> _ports;
    std::shared_ptr<ell::model::Model> _model;
};

//
// OutputPortIterator
//

class OutputPortIterator
{
public:
    OutputPortIterator() = default;
    bool IsValid();
    void Next();
    OutputPort Get();
#ifndef SWIG
    OutputPortIterator(const std::vector<const ell::model::OutputPortBase*>& ports, std::shared_ptr<ell::model::Model> model);
#endif
private:
    size_t _i = 0;
    std::vector<const ell::model::OutputPortBase*> _ports;

protected:
    std::shared_ptr<ell::model::Model> _model;
};

//
// Port Memory Layout
//

class PortMemoryLayout
{
public:
    const std::vector<int> size;
    const std::vector<int> extent;
    const std::vector<int> offset;
    const std::vector<int> order;

    PortMemoryLayout(const std::vector<int>& size,
                     const std::vector<int>& extent = {},
                     const std::vector<int>& offset = {},
                     const std::vector<int>& order = {});

    PortMemoryLayout(const ell::api::math::TensorShape& size);

    bool IsEqual(const PortMemoryLayout& other);

#ifndef SWIG
    const ell::model::PortMemoryLayout& Get() const
    {
        return _layout;
    }

    PortMemoryLayout(const ell::model::PortMemoryLayout& layout);

#endif
private:
    ell::model::PortMemoryLayout _layout;
};

//
// PortElement
//

class PortElement
{
public:
    PortElement() = default;
    int GetIndex();
    PortType GetType();
    OutputPort ReferencedPort();

#ifndef SWIG
    PortElement(const ell::model::PortElementBase& other, std::shared_ptr<ell::model::Model> model);
    std::shared_ptr<ell::model::Model> GetModel() const { return _model; }
#endif
private:
    ell::model::PortElementBase _port;
    std::shared_ptr<ell::model::Model> _model;
};

//
// PortElements
//

class PortElements
{
public:
    PortElements() = default;
    PortElements(const OutputPort& port);
    int Size() const;
    PortMemoryLayout GetMemoryLayout() const;
    PortType GetType() const;
    PortElement GetElement(int index) const;

#ifndef SWIG
    PortElements(const ell::model::PortElementsBase& other, std::shared_ptr<ell::model::Model> model);
    PortElements(const ell::model::OutputPortBase& port, std::shared_ptr<ell::model::Model> model);
    const ell::model::PortElementsBase& GetPortElements() const { return _elements; }
    std::shared_ptr<ell::model::Model> GetModel() const { return _model; }
#endif
private:
    ell::model::PortElementsBase _elements;
    std::shared_ptr<ell::model::Model> _model;
};

//
// InputPort
//

class InputPort : public Port
{
public:
    InputPort() = default;
    NodeIterator GetParentNodes();
    OutputPort GetReferencedPort();
#ifndef SWIG
    InputPort(const ell::model::InputPortBase* other, std::shared_ptr<ell::model::Model> model);
    const ell::model::InputPortBase& GetInputPort() const { return *_inputPort; }
#endif
private:
    const ell::model::InputPortBase* _inputPort = nullptr;
};

//
// OutputPort
//

class OutputPort : public Port
{
public:
    OutputPort() = default;
    bool IsReferenced() const;
    InputPortIterator GetReferences();
    std::vector<double> GetDoubleOutput();
    double GetDoubleOutput(int index);
#ifndef SWIG
    OutputPort(const ell::model::OutputPortBase* other, std::shared_ptr<ell::model::Model> model);
    const ell::model::OutputPortBase& GetOutputPort() const { return *_outputPort; }
#endif
private:
    const ell::model::OutputPortBase* _outputPort = nullptr;
};

} // namespace ELL_API