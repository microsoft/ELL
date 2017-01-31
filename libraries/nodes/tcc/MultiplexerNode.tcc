////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultiplexerNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode()
        : CompilableNode({ &_elements, &_selector }, { &_output }), _elements(this, {}, elementsPortName), _selector(this, {}, selectorPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector)
        : CompilableNode({ &_elements, &_selector }, { &_output }), _elements(this, input, elementsPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, 1)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
    };

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Compute() const
    {
        int index = static_cast<int>(_selector[0]);
        _output.SetOutput({ _elements[index] });
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newElements = transformer.TransformPortElements(_elements.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<MultiplexerNode<ValueType, SelectorType>>(newElements, newSelector);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Compile(model::IRMapCompiler& compiler)
    {
        compiler.NewBlockRegion(*this);

        if (std::is_same<SelectorType, bool>())
        {
            CompileMultiplexerBinary(compiler);
        }
        else if (std::is_same<SelectorType, int>())
        {
            CompileUnrolled(compiler);
        }
        else
        {
            throw emitters::EmitterException(emitters::EmitterError::valueTypeNotSupported, "Multiplexer node selectors must be bool or int");
        }

        compiler.TryMergeRegion(*this);
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::CompileMultiplexerBinary(model::IRMapCompiler& compiler)
    {
        auto& function = compiler.GetCurrentFunction();

        auto pElements = GetInputPorts()[0];
        auto pSelector = GetInputPorts()[1];
        VerifyIsScalar(*pSelector);

        auto pOutput = GetOutputPorts()[0];
        VerifyIsScalar(*pOutput);

        llvm::Value* pSelectorVal = compiler.LoadVariable(pSelector);
        llvm::Value* pResult = compiler.EnsureEmitted(pOutput);
        auto lVal = pElements->GetInputElement(0); // lval is selected if the result of the "if" comparison is NON-zero
        auto rVal = pElements->GetInputElement(1);
        auto pLMergeableSrc = compiler.GetMergeableRegion(lVal);
        auto pRMergeableSrc = compiler.GetMergeableRegion(rVal);

        emitters::IRIfEmitter ife = function.If();
        ife.If(emitters::TypedComparison::equals, pSelectorVal, function.Literal(0));
        {
            if (pLMergeableSrc != nullptr)
            {
                function.MergeRegion(pLMergeableSrc);
            }
            function.Store(pResult, compiler.LoadVariable(pElements->GetInputElement(0)));
        }
        ife.Else();
        {
            if (pRMergeableSrc != nullptr)
            {
                function.MergeRegion(pRMergeableSrc);
            }
            function.Store(pResult, compiler.LoadVariable(pElements->GetInputElement(1)));
        }
        ife.End();

        auto pSelectorNode = pSelector->GetParentNodes()[0];
        if (HasSingleDescendant(*pSelectorNode))
        {
            compiler.TryMergeRegions(*pSelectorNode, *this);
        }
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::CompileUnrolled(model::IRMapCompiler& compiler)
    {
        auto& function = compiler.GetCurrentFunction();

        auto elementsPort = GetInputPorts()[0];
        auto selectorPort = GetInputPorts()[1];
        VerifyIsScalar(*selectorPort);
        auto numElements = elementsPort->Size();

        auto outputPort = GetOutputPorts()[0];
        VerifyIsScalar(*outputPort);

        llvm::Value* selector = compiler.LoadVariable(selectorPort);
        llvm::Value* result = compiler.EnsureEmitted(outputPort);
        for (size_t index = 0; index < numElements; ++index)
        {
            emitters::IRIfEmitter if1 = function.If(emitters::TypedComparison::equals, function.Literal((int)index), selector);
            {
                llvm::Value* val = compiler.LoadVariable(elementsPort->GetInputElement(index));
                function.Store(result, val);
            }
            if1.End();
        }
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver["elements"] << _elements;
        archiver["selector"] << _selector;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver["elements"] >> _elements;
        archiver["selector"] >> _selector;
        archiver[outputPortName] >> _output;
    }
}
}
