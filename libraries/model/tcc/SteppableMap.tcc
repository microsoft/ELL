////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SteppableMap.tcc (model)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename ClockType>
    SteppableMap<ClockType>::SteppableMap(const Model& model, const std::vector<std::pair<std::string, InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, PortElementsBase>>& outputs, DurationType interval)
        : DynamicMap(model, inputs, outputs), _interval(interval), _lastSampleTime(StepTimepointType::min()), _numInputs(inputs.size())
    {
    }

    template <typename ClockType>
    SteppableMap<ClockType>::SteppableMap(const SteppableMap& other)
        : DynamicMap(other), _lastSampleTime(other._lastSampleTime), _interval(other._interval), _numInputs(other._numInputs)
    {
    }

    template <typename ClockType>
    DurationType SteppableMap<ClockType>::GetWaitTimeForNextCompute() const
    {
        DurationType result = DurationType(0); // default to no waiting
        const auto now = ClockType::now();

        if (_lastSampleTime != StepTimepointType::min())
        {
            // Compute has been called at least once
            const auto nextTime = _lastSampleTime + _interval;
            if (nextTime > now)
            {
                result = std::chrono::duration_cast<DurationType>(nextTime - now);
            }
        }
        return result;
    }

    template <typename ClockType>
    void SteppableMap<ClockType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        DynamicMap::WriteToArchive(archiver);

        archiver["interval"] << static_cast<int>(_interval.count());
        archiver["lastSampleTime"] << ToTicks(_lastSampleTime);
    }

    template <typename ClockType>
    void SteppableMap<ClockType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        DynamicMap::ReadFromArchive(archiver);

        int intervalTicks;
        archiver["interval"] >> intervalTicks;
        _interval = DurationType(intervalTicks);

        double lastSampleTimeTicks;
        archiver["lastSampleTime"] >> lastSampleTimeTicks;
        _lastSampleTime = StepTimepointType(DurationType(static_cast<long long int>(lastSampleTimeTicks))); // valid cast ???

        _numInputs = GetInputs().size();
    }

    template <typename ClockType>
    std::vector<bool> SteppableMap<ClockType>::ComputeBoolOutput(const PortElementsBase& outputs) const
    {
        return Step<bool>([this, outputs]() { return DynamicMap::ComputeBoolOutput(outputs); });
    }

    template <typename ClockType>
    std::vector<int> SteppableMap<ClockType>::ComputeIntOutput(const PortElementsBase& outputs) const
    {
        return Step<int>([this, outputs]() { return DynamicMap::ComputeIntOutput(outputs); });
    }

    template <typename ClockType>
    std::vector<double> SteppableMap<ClockType>::ComputeDoubleOutput(const PortElementsBase& outputs) const
    {
        return Step<double>([this, outputs]() { return DynamicMap::ComputeDoubleOutput(outputs); });
    }

    template <typename ClockType>
    template <typename OutputType, typename ComputeFunction>
    std::vector<OutputType> SteppableMap<ClockType>::Step(ComputeFunction&& compute) const
    {
        std::vector<OutputType> resultValues;

        if (_lastSampleTime == StepTimepointType::min())
        {
            _lastSampleTime = ClockType::now() - _interval;
        }

        auto sampleTime = _lastSampleTime + _interval;
        auto now = ClockType::now();
        while (sampleTime <= now)
        {
            // Feed the time signal into all inputs
            // Here we assume that the model InputNodes are setup correctly to receive the time signal
            for (size_t i = 0; i < _numInputs; i++)
            {
                if (GetInput(i)->GetOutputPort().GetType() == Port::PortType::real)
                {
                    SetInputValue<TimeTickType>(i, sampleTime, now);
                }
                else
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                }
            }

            // Now compute the model
            resultValues = compute();

            // Sleep between iterations is not necessary while we are playing catch-up
            _lastSampleTime = sampleTime;
            sampleTime += _interval;

            // Compute could have taken a long time, update the "now" timestamp
            // There is a risk that Step will never return, so we may need to set a time limit
            now = ClockType::now();
        }

        // Only the latest result will be available in the output when Step returns
        // Meanwhile, the eventing node will callback with results at each iteration
        return resultValues;
    }

    template <typename ClockType>
    template <typename InputType>
    void SteppableMap<ClockType>::SetInputValue(size_t index, StepTimepointType sampleTime, StepTimepointType currentTime) const
    {
        // Time signal is represented as ticks, relative to the last sample. This keeps the numbers
        // small and the model only cares about a time window starting from the last sample
        auto lastSampleTicks = ToTicks(_lastSampleTime);
        auto sampleTimeTicks = static_cast<InputType>(ToTicks(sampleTime) - lastSampleTicks);
        auto currentTimeTicks = static_cast<InputType>(ToTicks(currentTime) - lastSampleTicks);

        DynamicMap::SetInputValue<InputType>(index, std::vector<InputType>{ sampleTimeTicks, currentTimeTicks });
    }

    template <typename ClockType>
    TimeTickType SteppableMap<ClockType>::ToTicks(StepTimepointType timepoint) const
    {
        return std::chrono::duration_cast<DurationType>(timepoint.time_since_epoch()).count();
    }
}
}