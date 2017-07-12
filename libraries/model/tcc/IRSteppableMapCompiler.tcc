////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRSteppableMapCompiler.tcc (model)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    // Function name generators
    static std::string MapFunctionName(const std::string& functionNamePrefix)
    {
        return functionNamePrefix + "_Predict";
    }

    static std::string WaitTimeForNextComputeFunctionName(const std::string& functionNamePrefix)
    {
        return functionNamePrefix + "_WaitTimeForNextPredict";
    }

    static std::string GetIntervalFunctionName(const std::string& functionNamePrefix)
    {
        return functionNamePrefix + "_GetInterval";
    }

    template <typename ClockType>
    IRSteppableMapCompiler<ClockType>::IRSteppableMapCompiler()
        : IRSteppableMapCompiler(MapCompilerParameters{})
    {
    }

    template <typename ClockType>
    IRSteppableMapCompiler<ClockType>::IRSteppableMapCompiler(const MapCompilerParameters& settings)
        : IRMapCompiler(settings)
    {
    }

    template <typename ClockType>
    void IRSteppableMapCompiler<ClockType>::EnsureValidMap(SteppableMap<ClockType>& map)
    {
        IRMapCompiler::EnsureValidMap(map);

        auto inputSize = map.GetInput(0)->Size();
        if (inputSize != TimeSignalSize)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Steppable map must have time singla inputs of size = 2");
        }

        auto inputType = map.GetInput(0)->GetOutputPort().GetType();
        if (inputType != Port::PortType::real)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Steppable map must have time signal inputs of type = real");
        }
    }

    template <typename ClockType>
    llvm::Value* IRSteppableMapCompiler<ClockType>::CallClockFunction(emitters::IRFunctionEmitter& function)
    {
        return function.GetClockMilliseconds<ClockType>();
    }

    template <typename ClockType>
    std::string IRSteppableMapCompiler<ClockType>::GetPredictFunctionName() const
    {
        return MapFunctionName(GetMapCompilerParameters().mapFunctionName);
    }

    template <typename ClockType>
    IRCompiledMap IRSteppableMapCompiler<ClockType>::Compile(SteppableMap<ClockType> map)
    {
        EnsureValidMap(map);
        model::TransformContext context{ [](const model::Node& node) { return node.IsCompilable() ? model::NodeAction::compile : model::NodeAction::refine; } };
        map.Refine(context);

        if (GetMapCompilerParameters().profile)
        {
            GetModule().AddPreprocessorDefinition(GetNamespacePrefix() + "_PROFILING", "1");
        }
        _profiler = { GetModule(), map.GetModel(), GetMapCompilerParameters().profile };
        _profiler.EmitInitialization();

        auto predictFunctionName = GetPredictFunctionName();
        CompileMap(map, predictFunctionName);
        assert(GetModule().GetFunction(predictFunctionName) != nullptr);

        // Emit runtime model APIs
        EmitModelAPIFunctions(map);

        // Finish any profiling stuff we need to do and emit functions
        _profiler.EmitModelProfilerFunctions();

        auto module = std::make_unique<emitters::IRModuleEmitter>(std::move(_moduleEmitter));
        module->SetTargetTriple(GetCompilerParameters().targetDevice.triple);
        module->SetTargetDataLayout(GetCompilerParameters().targetDevice.dataLayout);
        return IRCompiledMap(std::move(map), GetMapCompilerParameters().mapFunctionName, std::move(module));
    }

    template <typename ClockType>
    void IRSteppableMapCompiler<ClockType>::EmitModelAPIFunctions(const DynamicMap& map)
    {
        // Emit basic compiled map API functions
        IRMapCompiler::EmitModelAPIFunctions(map);

        auto steppableMap = static_cast<const SteppableMap<ClockType>&>(map);

        // Now emit steppable map-specific functionality
        GetModule().template DeclareGetClockMilliseconds<ClockType>();

        // Globals accessed by the functions
        auto& variables = GetModule().Variables();
        auto pLastSampleTicksVar = variables.template AddVariable<emitters::InitializedScalarVariable<TimeTickType>>(emitters::VariableScope::global, TimeTickType(0));

        // Emit the step function that wraps the map function
        auto baseName = GetMapCompilerParameters().mapFunctionName;
        EmitStepFunction(steppableMap, baseName, pLastSampleTicksVar);

        // Emit the time functions
        EmitWaitTimeForNextComputeFunction(steppableMap, baseName, pLastSampleTicksVar);
        EmitGetIntervalFunction(steppableMap, baseName);
    }

    template <typename ClockType>
    void IRSteppableMapCompiler<ClockType>::EmitStepFunction(SteppableMap<ClockType>& map, const std::string& functionName, emitters::Variable* pLastSampleTicksVar)
    {
        auto args = AllocateNodeFunctionArguments(map, *(GetModuleEmitter()));
        auto function = GetModule().BeginFunction(functionName, emitters::VariableType::Void, args);
        auto arguments = function.Arguments().begin();
        llvm::Argument& output = *(++arguments); // input argument is currently not used because Step generates the time signal

        function.IncludeInHeader();
        function.IncludeInStepInterface(map.GetOutput(0).Size());

        // Constants
        auto intervalTicks = function.template Literal<TimeTickType>(map.GetIntervalTicks());
        auto zeroTicks = function.template Literal<TimeTickType>(TimeTickType(0));

        // Get the current time
        auto nowTicks = CallClockFunction(function);

        // Initialize the lastSampleTicks global
        auto pLastSampleTicks = function.GetModule().EnsureEmitted(*pLastSampleTicksVar);
        auto if1 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::equal), function.Load(pLastSampleTicks), zeroTicks);
        {
            auto lastSampleTicksInit = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::subtract), nowTicks, intervalTicks);
            function.Store(pLastSampleTicks, lastSampleTicksInit);
        }
        if1.End();

        // Local vector for time signal
        auto* pTimeSignal = function.Variable(TimeTickVarType, TimeSignalSize);
        auto lastSampleTicks = function.Load(pLastSampleTicks);
        auto sampleTicks = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::add), lastSampleTicks, intervalTicks);

        // Local pointer to hold the current time as a mutable condition for the for loop
        auto pNowTicks = function.Variable(TimeTickVarType, 1);
        function.Store(function.PointerOffset(pNowTicks, function.Literal(0)), nowTicks);

        // for (sampleTicks = lastSampleTicks + interval; sampleTicks <= nowTicks; sampleTicks += intervalTicks)
        auto forLoop = function.ForLoop();
        forLoop.template Begin<TimeTickType, emitters::BinaryPredicateType::lessOrEqual>(sampleTicks, intervalTicks, pNowTicks);
        {
            sampleTicks = forLoop.LoadIterationVariable();
            DEBUG_EMIT_PRINTF(function, "sampleTicks = %f, increment = %f, end = %f\n", sampleTicks, intervalTicks, nowTicks);

            // Call the map function, with the time signal as input (relative to the last sample ticks)
            auto relativeSampleTicks = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::subtract), sampleTicks, lastSampleTicks);
            function.SetValueAt(pTimeSignal, function.Literal(0), relativeSampleTicks);
            auto relativeNowTicks = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::subtract), nowTicks, lastSampleTicks);
            function.SetValueAt(pTimeSignal, function.Literal(1), relativeNowTicks);

            function.Call(MapFunctionName(functionName), { function.PointerOffset(pTimeSignal, function.Literal(0)), &output });

            // Update state and current time
            function.Store(pLastSampleTicks, sampleTicks);
            nowTicks = CallClockFunction(function);
            function.Store(function.PointerOffset(pNowTicks, function.Literal(0)), nowTicks);
        }
        forLoop.End();

        GetModule().EndFunction();
    }

    template <typename ClockType>
    void IRSteppableMapCompiler<ClockType>::EmitWaitTimeForNextComputeFunction(SteppableMap<ClockType>& map, const std::string& functionNamePrefix, emitters::Variable* pLastSampleTicksVar)
    {
        emitters::NamedVariableTypeList args = {}; // no args
        auto function = GetModule().BeginFunction(WaitTimeForNextComputeFunctionName(functionNamePrefix), TimeTickVarType, args);
        function.IncludeInHeader();
        function.IncludeInStepTimeInterface("WaitTimeForNextStep");

        // Constants
        auto intervalTicks = function.template Literal<TimeTickType>(static_cast<TimeTickType>(map.GetIntervalTicks()));
        auto zeroTicks = function.template Literal<TimeTickType>(TimeTickType(0));

        // Result stack variable
        llvm::Value* pResult = function.Variable(TimeTickVarType);
        function.Store(pResult, zeroTicks); // default to no wait

        // Globals
        auto pLastSampleTicks = function.GetModule().EnsureEmitted(*pLastSampleTicksVar);
        auto lastSampleTicks = function.Load(pLastSampleTicks);

        auto if1 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::equal), lastSampleTicks, zeroTicks);
        {
            // Step has been called at least once, derive the wait time from the interval and last sample time
            auto nextSampleTicks = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::add), lastSampleTicks, intervalTicks);
            auto nowTicks = CallClockFunction(function);

            // If the next sample time is greater than the current time, return the delta
            auto if2 = function.If(emitters::GetComparison<TimeTickType>(emitters::BinaryPredicateType::greater), nextSampleTicks, nowTicks);
            {
                auto waitTicks = function.Operator(emitters::GetOperator<TimeTickType>(emitters::BinaryOperationType::subtract), nextSampleTicks, nowTicks);
                function.Store(pResult, waitTicks);
            }
            if2.End();
        }
        if1.End();

        GetModule().EndFunction(function.Load(pResult));
    }

    template <typename ClockType>
    void IRSteppableMapCompiler<ClockType>::EmitGetIntervalFunction(SteppableMap<ClockType>& map, const std::string& functionNamePrefix)
    {
        emitters::NamedVariableTypeList args = {}; // no args
        auto function = GetModule().BeginFunction(GetIntervalFunctionName(functionNamePrefix), TimeTickVarType, args);
        function.IncludeInHeader();
        function.IncludeInStepTimeInterface("GetInterval");

        auto intervalTicks = function.template Literal<TimeTickType>(static_cast<TimeTickType>(map.GetIntervalTicks()));

        llvm::Value* pResult = function.Variable(TimeTickVarType);
        function.Store(pResult, intervalTicks);

        GetModule().EndFunction(function.Load(pResult));
    }
}
}
