////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ClockNode.cpp (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// model
#include "IRMapCompiler.h"

#include "ClockNode.h"

namespace ell
{
namespace nodes
{
    constexpr TimeTickType UninitializedIntervalTime = -1;

    // Useful aliases for operators
    const auto plusTime = emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::add);
    const auto minusTime = emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::subtract);

    // comparisons
    const auto equalTime = emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::equal);
    const auto greaterThanTime = emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::greater);
    const auto greaterThanOrEqualTime = emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::greaterOrEqual);

    ClockNode::ClockNode()
        : ClockNode({}, 0, 0, "", nullptr)
    {
    }

    ClockNode::ClockNode(const model::PortElements<TimeTickType>& input, TimeTickType interval, TimeTickType lagThreshold, const std::string& functionName, LagNotificationFunction function)
        : CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 2 /*sampleTime, currentTime*/),
        _interval(interval),
        _lastIntervalTime(UninitializedIntervalTime),
        _lagThreshold(lagThreshold),
        _lagNotificationFunction(function == nullptr ? [](auto){} : function),
        _lagNotificationFunctionName(functionName)
    {
        if (interval < 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "interval must be >= 0");
        }
        if (lagThreshold < 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "lagThreshold must be >= 0");
        }
    }

    void ClockNode::Compute() const
    {
        auto currentTime = _input.GetValue(0);

        // No lag when:
        // 1) this is the very first Compute() call, or
        // 2) the interval is zero
        if (_lastIntervalTime == UninitializedIntervalTime || _interval == 0)
        {
            _lastIntervalTime = currentTime;
        }
        else
        {
            _lastIntervalTime += _interval;
        }

        if (_lagNotificationFunction && _interval > 0)
        {
            // Notify if the time lag reaches the threshold
            auto delta = currentTime - _lastIntervalTime;
            if (delta >= _lagThreshold)
            {
                _lagNotificationFunction(delta);
            }
        }
        _output.SetOutput({ _lastIntervalTime, currentTime });
    }

    void ClockNode::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto now = compiler.EnsurePortEmitted(input);
        auto& module = function.GetModule();

        // Constants
        auto interval = function.template Literal<TimeTickType>(_interval);
        auto uninitializedIntervalTime = function.template Literal<TimeTickType>(UninitializedIntervalTime);
        auto zeroInterval = function.template Literal<TimeTickType>(0);
        auto thresholdTime = function.template Literal<TimeTickType>(_lagThreshold);

        // Callback
        const emitters::NamedVariableTypeList parameters = { { "currentTime", emitters::GetVariableType<TimeTickType>() } };
        std::string prefixedName(compiler.GetNamespacePrefix() + "_" + _lagNotificationFunctionName);
        module.DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);
        module.IncludeInCallbackInterface(prefixedName, "ClockNode");

        // State: _lastIntervalTime
        auto pLastIntervalTime = module.Global(compiler.GetNamespacePrefix() + "_lastIntervalTime", UninitializedIntervalTime);
        auto lastIntervalTime = function.Load(pLastIntervalTime);

        // No lag when:
        // 1) this is the very first call, or
        // 2) the interval is zero
        auto noLag = function.LogicalOr(
            function.Comparison(equalTime, lastIntervalTime, uninitializedIntervalTime),
            function.Comparison(equalTime, interval, zeroInterval));

        auto newLastInterval = function.Variable(emitters::GetVariableType<TimeTickType>(), "newLastInterval");
        function.Store(newLastInterval, lastIntervalTime);

        auto ifEmitter1 = function.If();
        ifEmitter1.If(noLag);
        {
            function.Store(newLastInterval, now);
        }
        ifEmitter1.Else();
        {
            function.Store(newLastInterval, function.Operator(plusTime, lastIntervalTime, interval));
        }
        ifEmitter1.End();

        auto if1 = function.If(greaterThanTime, interval, zeroInterval);
        {
            // Notify if the time lag reaches the threshold
            auto delta = function.Operator(minusTime, now, function.Load(newLastInterval));
            auto if2 = function.If(greaterThanOrEqualTime, delta, thresholdTime);
            {
                auto pLagFunction = module.GetFunction(prefixedName);
                function.Call(pLagFunction, { delta });
            }
            if2.End();
        }
        if1.End();

        // Update _lastInterval state
        function.Store(pLastIntervalTime, function.Load(newLastInterval));

        // Set output
        auto pOutput = compiler.EnsurePortEmitted(output);
        function.SetValueAt(pOutput, function.Literal(0), function.Load(newLastInterval));
        function.SetValueAt(pOutput, function.Literal(1), now);

        EmitGetTicksUntilNextIntervalFunction(compiler, module, pLastIntervalTime);
        EmitGetLagThresholdFunction(compiler, module);
        EmitGetStepIntervalFunction(compiler, module);
    }

    void ClockNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ClockNode>(newPortElements, _interval, _lagThreshold, _lagNotificationFunctionName, _lagNotificationFunction);
        transformer.MapNodeOutput(output, newNode->output);
    }

    void ClockNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;

        archiver["interval"] << _interval;
        archiver["lagThreshold"] << _lagThreshold;
        archiver["lagNotificationFunctionName"] << _lagNotificationFunctionName;
    }

    void ClockNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;

        archiver["interval"] >> _interval;
        archiver["lagThreshold"] >> _lagThreshold;
        archiver["lagNotificationFunctionName"] >> _lagNotificationFunctionName;
    }

    TimeTickType ClockNode::GetTicksUntilNextInterval(TimeTickType now) const
    {
        TimeTickType result;
        if (_lastIntervalTime == UninitializedIntervalTime || _interval == 0)
        {
            result = TimeTickType(0);
        }
        else
        {
            result = _lastIntervalTime + _interval - now;
        }
        return result;
    }

    void ClockNode::EmitGetTicksUntilNextIntervalFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter, llvm::GlobalVariable* pLastIntervalTime)
    {
        std::string functionName = compiler.GetNamespacePrefix() + "_GetTicksUntilNextInterval";
        const auto timeTickType = emitters::GetVariableType<TimeTickType>();
        const emitters::NamedVariableTypeList parameters = { { "currentTime", timeTickType } };

        emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, timeTickType, parameters);
        moduleEmitter.DeclareFunction(functionName, timeTickType, parameters);
        function.IncludeInHeader();

        auto arguments = function.Arguments().begin();
        auto now = &(*arguments++);

        auto interval = function.template Literal<TimeTickType>(_interval);
        auto uninitializedIntervalTime = function.template Literal<TimeTickType>(UninitializedIntervalTime);
        auto zeroInterval = function.template Literal<TimeTickType>(0);
        auto lastIntervalTime = function.Load(pLastIntervalTime);

        auto result = function.Variable(timeTickType, "result");

        auto noLag = function.LogicalOr(
            function.Comparison(equalTime, lastIntervalTime, uninitializedIntervalTime),
            function.Comparison(equalTime, interval, zeroInterval));

        auto ifEmitter1 = function.If();
        ifEmitter1.If(noLag);
        {
            function.Store(result, zeroInterval);
        }
        ifEmitter1.Else();
        {
            function.Store(result, function.Operator(minusTime, function.Operator(plusTime, lastIntervalTime, interval), now));
        }
        ifEmitter1.End();

        function.Return(function.Load(result));
        moduleEmitter.EndFunction();
    }

    void ClockNode::EmitGetLagThresholdFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter)
    {
        std::string functionName = compiler.GetNamespacePrefix() + "_GetLagThreshold";
        const auto timeTickType = emitters::GetVariableType<TimeTickType>();

        emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, timeTickType, emitters::NamedVariableTypeList{});
        moduleEmitter.DeclareFunction(functionName, timeTickType, emitters::NamedVariableTypeList{});
        function.IncludeInHeader();

        function.Return(function.template Literal<TimeTickType>(_lagThreshold));
        moduleEmitter.EndFunction();
    }

    void ClockNode::EmitGetStepIntervalFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter)
    {
        std::string functionName = compiler.GetNamespacePrefix() + "_GetStepInterval";
        const auto timeTickType = emitters::GetVariableType<TimeTickType>();

        emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, timeTickType, emitters::NamedVariableTypeList{});
        moduleEmitter.DeclareFunction(functionName, timeTickType, emitters::NamedVariableTypeList{});
        function.IncludeInHeader();

        function.Return(function.template Literal<TimeTickType>(_interval));
        moduleEmitter.EndFunction();
    }
}
}
