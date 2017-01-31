////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExtremalValueNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode()
        : CompilableNode({ &_input }, { &_val, &_argVal }), _input(this, {}, inputPortName), _val(this, valPortName, 1), _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode(const model::PortElements<ValueType>& input)
        : CompilableNode({ &_input }, { &_val, &_argVal }), _input(this, input, inputPortName), _val(this, valPortName, 1), _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    std::string ExtremalValueNode<ValueType, max>::GetTypeName()
    {
        if (max)
        {
            return utilities::GetCompositeTypeName<ValueType, std::true_type>("ExtremalValueNode");
        }
        else
        {
            return utilities::GetCompositeTypeName<ValueType, std::false_type>("ExtremalValueNode");
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compute() const
    {
        auto inputValues = _input.GetValue();
        decltype(std::max_element(inputValues.begin(), inputValues.end())) result;
        if (max)
        {
            result = std::max_element(inputValues.begin(), inputValues.end());
        }
        else
        {
            result = std::min_element(inputValues.begin(), inputValues.end());
        }
        auto val = *result;
        auto index = result - inputValues.begin();
        _val.SetOutput({ val });
        _argVal.SetOutput({ (int)index });
    };

    template <typename ValueType, bool max>
    emitters::TypedComparison ExtremalValueNode<ValueType, max>::GetComparison() const
    {
        if (IsMaxNode())
        {
            return emitters::GetComparison<ValueType>(emitters::BinaryPredicateType::greater);
        }
        else
        {
            return emitters::GetComparison<ValueType>(emitters::BinaryPredicateType::less);
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compile(model::IRMapCompiler& compiler)
    {
        compiler.NewBlockRegion(*this);
        auto inputPort = GetInputPorts()[0];
        if (IsPureVector(*inputPort) && !compiler.GetCompilerParameters().unrollLoops)
        {
            CompileLoop(compiler);
        }
        else
        {
            CompileExpanded(compiler);
        }
        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::CompileLoop(model::IRMapCompiler& compiler)
    {
        auto inputPort = GetInputPorts()[0];
        auto valPort = GetOutputPorts()[0];
        auto argValPort = GetOutputPorts()[1];
        VerifyIsScalar(*valPort);
        VerifyIsScalar(*argValPort);
        auto inputType = GetPortVariableType(*inputPort);

        llvm::Value* input = compiler.EnsureEmitted(inputPort);
        llvm::Value* outVal = compiler.EnsureEmitted(valPort);
        llvm::Value* outArgVal = compiler.EnsureEmitted(argValPort);

        auto numInputs = inputPort->Size();
        auto& function = compiler.GetCurrentFunction();

        llvm::Value* bestVal = function.Variable(inputType, "bestVal");
        llvm::Value* bestIndex = function.Variable(ell::emitters::VariableType::Int32, "bestArgVal");

        auto val0 = function.ValueAt(input, function.Literal(0));
        function.Store(bestVal, val0);
        function.Store(bestIndex, function.Literal(0));

        auto forLoop = function.ForLoop();
        forLoop.Begin(1, numInputs, 1);
        {
            auto i = forLoop.LoadIterationVariable();
            auto val = function.ValueAt(input, i);
            emitters::IRIfEmitter if1 = function.If(GetComparison(), val, function.Load(bestVal));
            {
                function.Store(bestVal, val);
                function.Store(bestIndex, i);
            }
            if1.End();
        }
        forLoop.End();
        function.Store(outVal, function.Load(bestVal));
        function.Store(outArgVal, function.Load(bestIndex));
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::CompileExpanded(model::IRMapCompiler& compiler)
    {
        auto inputPort = GetInputPorts()[0];
        auto valPort = GetOutputPorts()[0];
        auto argValPort = GetOutputPorts()[1];
        VerifyIsScalar(*valPort);
        VerifyIsScalar(*argValPort);

        llvm::Value* outVal = compiler.EnsureEmitted(valPort);
        llvm::Value* outArgVal = compiler.EnsureEmitted(argValPort);
        auto inputType = GetPortVariableType(*inputPort);

        auto numInputs = inputPort->Size();
        auto& function = compiler.GetCurrentFunction();

        llvm::Value* bestVal = function.Variable(inputType, "bestVal");
        llvm::Value* bestIndex = function.Variable(ell::emitters::VariableType::Int32, "bestArgVal");

        llvm::Value* val0 = compiler.LoadVariable(inputPort->GetInputElement(0));
        function.Store(bestVal, val0);
        function.Store(bestIndex, function.Literal(0));

        for (int i = 1; i < numInputs; ++i)
        {
            llvm::Value* val = compiler.LoadVariable(inputPort->GetInputElement(i));
            emitters::IRIfEmitter if1 = function.If(GetComparison(), val, function.Load(bestVal));
            {
                function.Store(bestVal, val);
                function.Store(bestIndex, function.Literal(i));
            }
            if1.End();
        }

        function.Store(outVal, function.Load(bestVal));
        function.Store(outArgVal, function.Load(bestIndex));
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[valPortName] << _val;
        archiver[argValPortName] << _argVal;
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[valPortName] >> _val;
        archiver[argValPortName] >> _argVal;
    }

    //
    // Copy definitions for subclasses
    //
    template <typename ValueType>
    void ArgMinNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<ArgMinNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }

    template <typename ValueType>
    void ArgMaxNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<ArgMaxNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }
}
}
