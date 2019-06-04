////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Ports.cpp (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Ports.h"
#include "ModelInterface.h"

#include <utilities/include/Exception.h>

namespace ELL_API
{

using namespace ell::utilities;

//
// Port
//
PortType Port::GetOutputType()
{
    return static_cast<PortType>(_port->GetType());
}

Port::Port(const ell::model::Port* other, std::shared_ptr<ell::model::Model> model) :
    _port(other),
    _model(model)
{
}

Node Port::GetNode()
{
    return Node(_port->GetNode(), _model);
}

std::string Port::GetName()
{
    return _port->GetName();
}

std::string Port::GetRuntimeTypeName()
{
    return _port->GetRuntimeTypeName();
}

int Port::Size()
{
    return static_cast<int>(_port->Size());
}

PortMemoryLayout Port::GetMemoryLayout()
{
    return PortMemoryLayout(_port->GetMemoryLayout());
}

//
// InputPortIterator
//
bool InputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void InputPortIterator::Next()
{
    _i++;
}

InputPort InputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return InputPort(_ports[_i], _model);
}

InputPortIterator::InputPortIterator(const std::vector<const ell::model::InputPortBase*>& ports, std::shared_ptr<ell::model::Model> model) :
    _i(0),
    _ports(ports),
    _model(model)
{
}

//
// OutputPortIterator
//
bool OutputPortIterator::IsValid()
{
    return _i < _ports.size();
}

void OutputPortIterator::Next()
{
    _i++;
}

OutputPort OutputPortIterator::Get()
{
    if (!IsValid())
    {
        throw std::out_of_range("invalid iterator");
    }
    return OutputPort(_ports[_i], _model);
}

OutputPortIterator::OutputPortIterator(const std::vector<const ell::model::OutputPortBase*>& ports, std::shared_ptr<ell::model::Model> model) :
    _i(0),
    _ports(ports),
    _model(model)
{
}

//
// PortElement
//
PortElement::PortElement(const ell::model::PortElementBase& other, std::shared_ptr<ell::model::Model> model) :
    _port(other),
    _model(model)
{
}

int PortElement::GetIndex()
{
    return static_cast<int>(_port.GetIndex());
}

PortType PortElement::GetType()
{
    return static_cast<PortType>(_port.GetPortType());
}

OutputPort PortElement::ReferencedPort()
{
    auto port = _port.ReferencedPort();
    if (port == nullptr)
    {
        throw Exception("no referenced port");
    }
    return OutputPort(port, _model);
}

//
// PortElements
//
PortElements::PortElements(const ell::model::PortElementsBase& other, std::shared_ptr<ell::model::Model> model) :
    _elements(other),
    _model(model)
{
}

PortElements::PortElements(const OutputPort& port) :
    _elements(port.GetOutputPort()),
    _model(port.GetModel())
{
}

int PortElements::Size() const
{
    return _elements.Size();
}

PortMemoryLayout PortElements::GetMemoryLayout() const
{
    return _elements.GetMemoryLayout();
}

PortType PortElements::GetType() const
{
    return static_cast<PortType>(_elements.GetPortType());
}

PortElement PortElements::GetElement(int index) const
{
    if (index < 0 || index >= Size())
    {
        throw std::invalid_argument("index out of range");
    }
    return PortElement(_elements.GetElement(index), _model);
}

//
// InputPort
//

InputPort::InputPort(const ell::model::InputPortBase* other, std::shared_ptr<ell::model::Model> model) :
    Port(other, model),
    _inputPort(other)
{
}

NodeIterator InputPort::GetParentNodes()
{
    return NodeIterator(_inputPort->GetParentNodes(), _model);
}

OutputPort InputPort::GetReferencedPort()
{
    return OutputPort(&_inputPort->GetReferencedPort(), _model);
}

//
// OutputPort
//
OutputPort::OutputPort(const ell::model::OutputPortBase* other, std::shared_ptr<ell::model::Model> model) :
    Port(other, model),
    _outputPort(other)
{
}

bool OutputPort::IsReferenced() const
{
    return _outputPort->IsReferenced();
}

InputPortIterator OutputPort::GetReferences()
{
    return InputPortIterator(_outputPort->GetReferences(), _model);
}

std::vector<double> OutputPort::GetDoubleOutput()
{
    return _outputPort->GetDoubleOutput();
}

double OutputPort::GetDoubleOutput(int index)
{
    return _outputPort->GetDoubleOutput((size_t)index);
}

//
// PortMemoryLayout
//

namespace
{
    bool IsAllZeros(const std::vector<int>& p)
    {
        if (p.empty())
        {
            return true;
        }
        return std::all_of(p.begin(), p.end(), [](const int& value) { return value == 0; });
    }
} // namespace

ell::model::PortMemoryLayout GetPortMemoryLayout(const std::vector<int>& size, const std::vector<int>& extent, const std::vector<int>& offset, const std::vector<int>& order)
{
    ell::model::MemoryShape sizeMemoryShape = { size };
    ell::model::MemoryShape extentMemoryShape;

    if (IsAllZeros(extent))
    {
        extentMemoryShape = ell::model::MemoryShape{ size };
    }
    else
    {
        extentMemoryShape = ell::model::MemoryShape{ extent };
    }

    if (IsAllZeros(offset) && IsAllZeros(order))
    {
        return ell::model::PortMemoryLayout(sizeMemoryShape);
    }
    else if (IsAllZeros(order))
    {
        return ell::model::PortMemoryLayout(sizeMemoryShape, extentMemoryShape, ell::model::MemoryShape{ offset });
    }
    else if (IsAllZeros(offset))
    {
        return ell::model::PortMemoryLayout(sizeMemoryShape, ell::model::DimensionOrder{ order });
    }
    else
    {
        return ell::model::PortMemoryLayout(sizeMemoryShape, extentMemoryShape, ell::model::MemoryShape{ offset }, ell::model::DimensionOrder{ order });
    }
}

PortMemoryLayout::PortMemoryLayout(const std::vector<int>& size, const std::vector<int>& extent, const std::vector<int>& offset, const std::vector<int>& order) :
    PortMemoryLayout(GetPortMemoryLayout(size, extent, offset, order))
{
}

PortMemoryLayout::PortMemoryLayout(const ell::api::math::TensorShape& size) :
    PortMemoryLayout(ell::model::PortMemoryLayout(size.ToMemoryShape()))
{
}

PortMemoryLayout::PortMemoryLayout(const ell::model::PortMemoryLayout& layout) :
    size(layout.GetActiveSize().ToVector()),
    extent(layout.GetExtent().ToVector()),
    offset(layout.GetOffset().ToVector()),
    order(layout.GetLogicalDimensionOrder().ToVector()),
    _layout(layout)
{
}

bool PortMemoryLayout::IsEqual(const PortMemoryLayout& other)
{
    return _layout == other._layout;
}

} // namespace ELL_API