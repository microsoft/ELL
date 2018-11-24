////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstantNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    // superclass (Node) constructor takes two array arguments: inputs and outputs. These are pointers to our local InputPort and OutputPort storage.

    // Default constructor
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode() :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, 0){};

    // Constructor for a scalar constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(ValueType value) :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, 1),
        _values({ value }){};

    // Constructor for a vector constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(const std::vector<ValueType>& values) :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, values.size()),
        _values(values){};

    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(const std::vector<ValueType>& values, const model::MemoryShape& shape) :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, shape),
        _values(values){};

    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(const std::vector<ValueType>& values, const model::PortMemoryLayout& layout) :
        CompilableNode({}, { &_output }),
        _output(this, defaultOutputPortName, layout),
        _values(values){};

    template <typename ValueType>
    void ConstantNode<ValueType>::Compute() const
    {
        _output.SetOutput(_values);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<ConstantNode<ValueType>>(_values, _output.GetMemoryLayout().GetActiveSize());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto values = this->GetValues();
        emitters::Variable* pVar = nullptr;
        pVar = function.GetModule().Variables().AddVariable<emitters::LiteralVectorVariable<ValueType>>(values);
        compiler.SetVariableForPort(output, pVar); // Just set the variable corresponding to the output port to be the global variable we created
    }

    template <typename ValueType>
    utilities::ArchiveVersion ConstantNode<ValueType>::GetArchiveVersion() const
    {
        return utilities::ArchiveVersionNumbers::v8_port_memory_layout;
    }

    template <typename ValueType>
    bool ConstantNode<ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version <= utilities::ArchiveVersionNumbers::v8_port_memory_layout;
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver["values"] << _values;
        archiver["layout"] << _output.GetMemoryLayout();
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver["values"] >> _values;
        if (archiver.HasNextPropertyName("layout"))
        {
            model::PortMemoryLayout layout;
            archiver["layout"] >> layout;
            _output.SetMemoryLayout(layout);
        }
        else
        {
            _output.SetSize(_values.size());
        }
    }
} // namespace nodes
} // namespace ell
