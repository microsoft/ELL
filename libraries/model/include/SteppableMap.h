////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SteppableMap.h (model)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DynamicMap.h"

// stl
#include <chrono>
#include <vector>

namespace ell
{
namespace model
{
    using DurationType = std::chrono::milliseconds;
    using TimeTickType = double;

    /// <summary> Class that wraps a model and its designated outputs, and performs interval-based computes (steps) on the model </summary>
    template <typename ClockType = std::chrono::steady_clock>
    class SteppableMap : public DynamicMap
    {
    public:
        using StepTimepointType = std::chrono::time_point<ClockType>;

        SteppableMap() = default;

        /// <summary> Copy constructor </summary>
        SteppableMap(const SteppableMap& other);

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap. </param>
        /// <param name="inputs"> A vector of name/value pairs for the inputs this map uses. </param>
        /// <param name="outputs"> A vector of name/value pairs for the outputs this map generates. </param>
        /// <param name="interval"> The interval used for running (stepping) model computes. </param>
        SteppableMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs, DurationType interval);

        virtual ~SteppableMap() = default;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ClockType>("SteppableMap"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the interval used for running model computes. </summary>
        ///
        /// <returns> The interval (in ticks). </returns>
        auto GetIntervalTicks() const { return _interval.count(); }

        /// <summary> Gets the duration to wait before calling Compute(). </summary>
        ///
        /// <returns> The duration. </returns>
        DurationType GetWaitTimeForNextCompute() const;

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        virtual std::vector<bool> ComputeBoolOutput(const PortElementsBase& outputs) const override;
        virtual std::vector<int> ComputeIntOutput(const PortElementsBase& outputs) const override;
        virtual std::vector<double> ComputeDoubleOutput(const PortElementsBase& outputs) const override;

    private:
        template <typename OutputType, typename ComputeFunction>
        std::vector<OutputType> Step(ComputeFunction&& compute) const;

        template <typename InputType>
        void SetInputValue(size_t index, StepTimepointType sampleTime, StepTimepointType currentTime) const;

        TimeTickType ToTicks(StepTimepointType timepoint) const;

        DurationType _interval;
        mutable StepTimepointType _lastSampleTime;
        size_t _numInputs;
    };
}
}

#include "../tcc/SteppableMap.tcc"