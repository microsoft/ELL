////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ClockNode.h (nodes)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "ModelTransformer.h"

namespace ell
{
namespace nodes
{
    using TimeTickType = double;

    /// <summary> A function that the node calls if the timestamp lags too far behind an interval. </summary>
    using LagNotificationFunction = std::function<void(TimeTickType)>;

    /// <summary> A node that verifies if input timestamps are within a specified time interval. </summary>
    class ClockNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<TimeTickType>& input = _input;
        const model::OutputPort<TimeTickType>& output = _output;
        /// @}

        ClockNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> Port elements for input value (current time). </param>
        /// <param name="interval"> The time interval. </param>
        /// <param name="lagThreshold">The time lag before lagFunction is called. </param>
        /// <param name="functionName">The lag notification name to be emitted. </param>
        /// <param name="function">The optional lag notification function used in Compute(). </param>
        ClockNode(const model::PortElements<TimeTickType>& input, TimeTickType interval, TimeTickType lagThreshold, const std::string& functionName, LagNotificationFunction function = nullptr);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ClockNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` receiving the copy. </param>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Sets the interval for this node. </summary>
        ///
        /// <param name="interval"> The interval to set. </param>
        void SetInterval(TimeTickType interval) { _interval = interval; }

        /// <summary> Sets the lag exceeded function for this node for use in Compute(). </summary>
        ///
        /// <param name="function"> The lag exceeded function to set. </param>
        void SetLagNotificationFunction(LagNotificationFunction function) { _lagNotificationFunction = function; }

        /// <summary> Sets the lag threshold for this node. </summary>
        ///
        /// <param name="threshold"> The threshold to set. </param>
        void SetLagTheshold(TimeTickType threshold) { _lagThreshold = threshold; }

        /// <summary> Gets the ticks until the next interval, relative to the current time. </summary>
        ///
        /// <param name="now"> The current time. </param>
        ///
        /// <returns> Ticks until the next interval. </param>
        TimeTickType GetTicksUntilNextInterval(TimeTickType now) const;

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        bool HasState() const override { return true; } // stored state: interval, lag threshold, lag function name

    private:
        void EmitGetTicksUntilNextIntervalFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter, llvm::GlobalVariable* pLastIntervalTime);
        void EmitGetLagThresholdFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter);
        void EmitGetStepIntervalFunction(model::IRMapCompiler& compiler, emitters::IRModuleEmitter& moduleEmitter);

        model::InputPort<TimeTickType> _input;
        model::OutputPort<TimeTickType> _output;

        TimeTickType _interval;
        mutable TimeTickType _lastIntervalTime;
        TimeTickType _lagThreshold;
        LagNotificationFunction _lagNotificationFunction;
        std::string _lagNotificationFunctionName;
    };
}
}