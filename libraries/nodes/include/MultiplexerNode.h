////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MultiplexerNode.h (node)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/MapCompiler.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>
#include <model/include/PortElements.h>

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>

#include <exception>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace nodes
{
    /// <summary> A node that outputs a dynamically specified element from an input array. </summary>
    template <typename ValueType, typename SelectorType>
    class MultiplexerNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* elementsPortName = "elements";
        static constexpr const char* selectorPortName = "selector";
        const model::InputPort<ValueType>& elements = _elements;
        const model::InputPort<SelectorType>& selector = _selector;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MultiplexerNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="elements"> The input aray of values. </param>
        /// <param name="selector"> The index of the chosen element </param>
        MultiplexerNode(const model::OutputPort<ValueType>& elements, const model::OutputPort<SelectorType>& selector);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, SelectorType>("MultiplexerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        void CompileMultiplexerBinary(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);
        void CompileUnrolled(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Inputs
        model::InputPort<ValueType> _elements;
        model::InputPort<SelectorType> _selector;

        // Output
        model::OutputPort<ValueType> _output;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="elements"> The input aray of values. </param>
    /// <param name="selector"> The index of the chosen element </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType, typename SelectorType>
    const model::OutputPort<ValueType>& Multiplexer(const model::OutputPort<ValueType>& elements, const model::OutputPort<SelectorType>& selector);

} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode() :
        CompilableNode({ &_elements, &_selector }, { &_output }),
        _elements(this, {}, elementsPortName),
        _selector(this, {}, selectorPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode(const model::OutputPort<ValueType>& input, const model::OutputPort<SelectorType>& selector) :
        CompilableNode({ &_elements, &_selector }, { &_output }),
        _elements(this, input, elementsPortName),
        _selector(this, selector, selectorPortName),
        _output(this, defaultOutputPortName, 1)
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
        const auto& newElements = transformer.GetCorrespondingInputs(_elements);
        const auto& newSelector = transformer.GetCorrespondingInputs(_selector);
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

        emitters::LLVMValue pSelector = compiler.EnsurePortEmitted(selector);
        emitters::LLVMValue pSelectorVal = function.Load(pSelector);
        emitters::LLVMValue pResult = compiler.EnsurePortEmitted(output);
        auto lVal = elements.GetInputElement(0); // lval is selected if the result of the "if" comparison is NON-zero
        auto rVal = elements.GetInputElement(1);
        auto pLMergeableSrc = compiler.GetMergeableNodeRegion(lVal);
        auto pRMergeableSrc = compiler.GetMergeableNodeRegion(rVal);

        function.If(emitters::TypedComparison::equals, pSelectorVal, function.Literal<SelectorType>(0), [pLMergeableSrc, pResult, &compiler, this](emitters::IRFunctionEmitter& function) {
                    if (pLMergeableSrc != nullptr)
                    {
                        function.MergeRegion(pLMergeableSrc);
                    }
                    function.Store(pResult, compiler.LoadPortElementVariable(elements.GetInputElement(0)));
                })
            .Else([pRMergeableSrc, pResult, &compiler, this](emitters::IRFunctionEmitter& function) {
                if (pRMergeableSrc != nullptr)
                {
                    function.MergeRegion(pRMergeableSrc);
                }
                function.Store(pResult, compiler.LoadPortElementVariable(elements.GetInputElement(1)));
            });

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

        emitters::LLVMValue pSelector = compiler.EnsurePortEmitted(selector);
        auto pSelectorVal = function.Load(pSelector);
        emitters::LLVMValue result = compiler.EnsurePortEmitted(output);
        for (size_t index = 0; index < numElements; ++index)
        {
            function.If(emitters::TypedComparison::equals, function.Literal((int)index), pSelectorVal, [index, result, &compiler, this](emitters::IRFunctionEmitter& function) {
                emitters::LLVMValue val = compiler.LoadPortElementVariable(elements.GetInputElement(index));
                function.Store(result, val);
            });
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

    template <typename ValueType, typename SelectorType>
    const model::OutputPort<ValueType>& Multiplexer(const model::OutputPort<ValueType>& elements, const model::OutputPort<SelectorType>& selector)
    {
        model::Model* model = elements.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        if (*model != *(selector.GetNode()->GetModel()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Inputs not part of the same model");
        }

        auto node = model->AddNode<MultiplexerNode<ValueType, SelectorType>>(elements, selector);
        return node->output;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
