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

    ClockNode::ClockNode(const model::PortElements<TimeTickType>& input, TimeTickType interval, short lagThreshold, const std::string& functionName, LagNotificationFunction function)
        : CompilableNode({ &_input }, { &_output }),
        _input(this, input, inputPortName),
        _output(this, outputPortName, 2 /*sampleTime, currentTime*/),
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
            if (delta >= _lagThreshold * _interval)
            {
                _lagNotificationFunction(delta);
            }
        }
        _output.SetOutput({ _lastIntervalTime, currentTime });
    }

    void ClockNode::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        auto currentTime = compiler.EnsurePortEmitted(input);

        // Constants
        auto interval = function.template Literal<TimeTickType>(_interval);
        auto uninitializedIntervalTime = function.template Literal<TimeTickType>(UninitializedIntervalTime);
        auto zeroInterval = function.template Literal<TimeTickType>(0);
        auto thresholdTime = function.template Literal<TimeTickType>(_lagThreshold * _interval);

        // Callback
        const emitters::VariableTypeList parameters = { emitters::GetVariableType<TimeTickType>() };
        std::string prefixedName(function.GetModule().GetModuleName() + "_" + _lagNotificationFunctionName);

        function.GetModule().DeclareFunction(prefixedName, emitters::VariableType::Void, parameters);
        function.GetModule().IncludeInHeader(prefixedName);
        function.GetModule().IncludeInCallbackInterface(prefixedName, "ClockNode");

        // State: _lastIntervalTime
        // BUG: InitializedScalarVariable doesn't persist globally for release builds
        auto pLastIntervalTimeVar = function.GetModule().Variables().AddVariable<emitters::InitializedVectorVariable<TimeTickType>>(
            emitters::VariableScope::global,
            std::vector<TimeTickType>({ _lastIntervalTime }));
        auto pLastIntervalTime = function.GetModule().EnsureEmitted(*pLastIntervalTimeVar);
        auto lastIntervalTime = function.ValueAt(pLastIntervalTime, function.Literal(0));

        // No lag when:
        // 1) this is the very first call, or
        // 2) the interval is zero
        auto noLag = function.LogicalOr(
            function.Comparison(equalTime, lastIntervalTime, uninitializedIntervalTime),
            function.Comparison(equalTime, interval, zeroInterval));

        function.GetModule().DeclarePrintf();
        function.Printf({ function.Literal("%d\n"), function.Load(noLag) } );
        function.Printf({ function.Literal("%f\n"), lastIntervalTime } );

        // Use a local scratch value to hold the intermediate computation
        auto scratch = function.Variable(emitters::GetVariableType<TimeTickType>(), "scratch");
        function.Store(scratch, lastIntervalTime);

        auto ifEmitter1 = function.If();
        ifEmitter1.If(function.Load(noLag));
        {
            function.Store(scratch, currentTime);
        }
        ifEmitter1.Else();
        {
            function.Store(scratch, function.Operator(plusTime, lastIntervalTime, interval));
        }
        ifEmitter1.End();

        auto if1 = function.If(greaterThanTime, interval, zeroInterval);
        {
            // Notify if the time lag reaches the threshold
            auto delta = function.Operator(minusTime, currentTime, lastIntervalTime);
            auto if2 = function.If(greaterThanOrEqualTime, delta, thresholdTime);
            {
                auto pLagFunction = function.GetModule().GetFunction(prefixedName);
                DEBUG_EMIT_PRINTF(function, prefixedName + "\n");
                function.Call(pLagFunction, { delta });
            }
            if2.End();
        }
        if1.End();

        // Update _lastInterval state
        function.SetValueAt(pLastIntervalTime, function.Literal(0), function.Load(scratch));

        // Set output
        auto pOutput = compiler.EnsurePortEmitted(output);
        function.SetValueAt(pOutput, function.Literal(0), function.Load(scratch));
        function.SetValueAt(pOutput, function.Literal(1), currentTime);
    }
    
    void ClockNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ClockNode>(newPortElements, _interval, _lagThreshold, _lagNotificationFunctionName);
        newNode->_lagNotificationFunction = _lagNotificationFunction;
        transformer.MapNodeOutput(output, newNode->output);
    }

    void ClockNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver["interval"] << _interval;
        archiver["lagThreshold"] << _lagThreshold;
        archiver["lagNotificationFunctionName"] << _lagNotificationFunctionName;
    }

    void ClockNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        archiver["interval"] >> _interval;
        archiver["lagThreshold"] >> _lagThreshold;
        archiver["lagNotificationFunctionName"] >> _lagNotificationFunctionName;
    }
}
}