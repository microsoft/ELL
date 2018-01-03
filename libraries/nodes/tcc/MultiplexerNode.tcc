////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultiplexerNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode()
        : CompilableNode({ &_elements, &_selector }, { &_output }), _elements(this, {}, elementsPortName), _selector(this, {}, selectorPortName), _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector)
        : CompilableNode({ &_elements, &_selector }, { &_output }), _elements(this, input, elementsPortName), _selector(this, selector, selectorPortName), _output(this, defaultOutputPortName, 1)
    {
        if (selector.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Condition must be 1-D signal");
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
    void MultiplexerNode<ValueType, SelectorType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        if (std::is_same<SelectorType, bool>())
        {
            CompileMultiplexerBinary(compiler, function);
        }
        else if (std::is_same<SelectorType, int>())
        {
            CompileUnrolled(compiler, function);
        }
        else
        {
            throw emitters::EmitterException(emitters::EmitterError::valueTypeNotSupported, "Multiplexer node selectors must be bool or int");
        }
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::CompileMultiplexerBinary(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        VerifyIsScalar(selector);
        VerifyIsScalar(output);

        llvm::Value* pSelectorVal = compiler.EnsurePortEmitted(selector);
        llvm::Value* pResult = compiler.EnsurePortEmitted(output);
        auto lVal = elements.GetInputElement(0); // lval is selected if the result of the "if" comparison is NON-zero
        auto rVal = elements.GetInputElement(1);
        auto pLMergeableSrc = compiler.GetMergeableNodeRegion(lVal);
        auto pRMergeableSrc = compiler.GetMergeableNodeRegion(rVal);

        emitters::IRIfEmitter ife = function.If();
        ife.If(emitters::TypedComparison::equals, pSelectorVal, function.Literal<SelectorType>(0));
        {
            if (pLMergeableSrc != nullptr)
            {
                function.MergeRegion(pLMergeableSrc);
            }
            function.Store(pResult, compiler.LoadPortElementVariable(elements.GetInputElement(0)));
        }
        ife.Else();
        {
            if (pRMergeableSrc != nullptr)
            {
                function.MergeRegion(pRMergeableSrc);
            }
            function.Store(pResult, compiler.LoadPortElementVariable(elements.GetInputElement(1)));
        }
        ife.End();

        auto pSelectorNode = selector.GetParentNodes()[0];
        if (HasSingleDescendant(*pSelectorNode))
        {
            compiler.TryMergeNodeRegions(*pSelectorNode, *this);
        }
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::CompileUnrolled(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        VerifyIsScalar(selector);
        VerifyIsScalar(output);
        auto numElements = elements.Size();

        llvm::Value* pSelectorVal = compiler.LoadPortVariable(selector); // TODO: change this to EnsurePortEmitted
        llvm::Value* result = compiler.EnsurePortEmitted(output);
        for (size_t index = 0; index < numElements; ++index)
        {
            emitters::IRIfEmitter if1 = function.If(emitters::TypedComparison::equals, function.Literal((int)index), pSelectorVal);
            {
                llvm::Value* val = compiler.LoadPortElementVariable(elements.GetInputElement(index));
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
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver["elements"] >> _elements;
        archiver["selector"] >> _selector;
    }
}
}
