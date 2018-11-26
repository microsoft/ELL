////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DebugSinkNode.h (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>

#include <emitters/include/IRMetadata.h>

#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A function that the DebugSinkNode calls to deliver data to user code. </summary>
    ///
    /// In device-side compiled code, the function signature should be:
    /// ```
    /// void DebugSinkFunction(char* label, ValueType* input, char* userData);
    ///
    /// Note: the userData argument in the compiled code is char* instead of void* because LLVM doesn't consider pointer-to-void to be a valid type
    ///   (for some versions of LLVM, at least)
    /// ```
    template <typename ValueType>
    using DebugSinkFunction = std::function<void(const std::string&, const std::vector<ValueType>&, void* userData)>;

    template <typename ValueType>
    class DebugSinkNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor </summary>
        DebugSinkNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> Port elements for input values </param>
        /// <param name="sink"> The sink function that will receive output values </param>
        /// <param name="label"> The optional label to be passed to the DebugSinkFunction </param>
        /// <param name="sinkFunctionName"> The optional sink function name to be emitted </param>
        DebugSinkNode(const model::OutputPort<ValueType>& input, DebugSinkFunction<ValueType> sink, const std::string& label, void* userData, const std::string& sinkFunctionName);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DebugSinkNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Get the label of this node </summary>
        ///
        /// <returns> The node label. </returns>
        virtual std::string GetLabel() const { return _label; }

    protected:
        bool ShouldCompileInline() const override;
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Evaluates whether the input meets the filter criteria,
        // and should be forwarded to the sink function.
        virtual bool EvaluateInput() const;

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        model::InputPort<ValueType> _input;
        model::OutputPort<ValueType> _output;
        std::string _label;
        void* _userData;
        std::string _sinkFunctionName;
        DebugSinkFunction<ValueType> _sink;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    DebugSinkNode<ValueType>::DebugSinkNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _userData(nullptr)
    {
    }

    template <typename ValueType>
    DebugSinkNode<ValueType>::DebugSinkNode(const model::OutputPort<ValueType>& input, DebugSinkFunction<ValueType> sink, const std::string& label, void* userData, const std::string& sinkFunctionName) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, _input.Size()),
        _label(label),
        _userData(userData),
        _sinkFunctionName(sinkFunctionName),
        _sink(std::move(sink))
    {
    }

    template <typename ValueType>
    bool DebugSinkNode<ValueType>::ShouldCompileInline() const
    {
        return true;
    }

    template <typename ValueType>
    void DebugSinkNode<ValueType>::Compute() const
    {
        DEBUG_THROW(_sink == nullptr, utilities::InputException(utilities::InputExceptionErrors::nullReference, "Sink function is not set"));

        auto result = EvaluateInput();
        if (result && _sink != nullptr)
        {
            _sink(_label, _input.GetValue(), _userData);
        }
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void DebugSinkNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(input);
        auto userData = function.Pointer((char*)_userData);

        // EvaluateInput defaults to 'pass through' in base implementation, which means
        // we always call the sink function
        const emitters::NamedVariableTypeList parameters = { { "label", emitters::GetVariableType<char*>() },
                                                             { "output", emitters::GetPointerType(emitters::GetVariableType<ValueType>()) },
                                                             { "userData", emitters::GetVariableType<char*>() } };

        // Callback signature: void DebugSinkNode(char* label, ValueType* array, char* userData)
        function.GetModule().DeclareFunction(_sinkFunctionName, emitters::VariableType::Void, parameters);
        emitters::LLVMFunction pSinkFunction = function.GetModule().GetFunction(_sinkFunctionName);
        function.Call(pSinkFunction, { function.Literal(_label), function.PointerOffset(pInput, function.Literal(0)), userData });

        // Tag the sink function as a callback that is emitted in headers
        function.IncludeInHeader();
    }

    template <typename ValueType>
    void DebugSinkNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<DebugSinkNode<ValueType>>(newPortElements, _sink, _label, _userData, _sinkFunctionName);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void DebugSinkNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["sinkFunctionName"] << _sinkFunctionName;
    }

    template <typename ValueType>
    void DebugSinkNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["sinkFunctionName"] >> _sinkFunctionName;
        // _sink needs to be set separately
    }

    template <typename ValueType>
    bool DebugSinkNode<ValueType>::EvaluateInput() const
    {
        // Default pass through (derived classes will override).
        return true;
    }
}; // namespace nodes
} // namespace ell

#pragma endregion implementation
