////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRSteppableMapCompiler.h (model)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "IRMapCompiler.h"
#include "SteppableMap.h"

// stl
#include <string>

namespace ell
{
namespace model
{
    /// <summary> Compiles Steppable ELL Models to LLVM IR </summary>
    template <typename ClockType>
    class IRSteppableMapCompiler : public IRMapCompiler
    {
    public:
        /// <summary> Create a compiler to produce an LLVM module with the default name </summary>
        IRSteppableMapCompiler();

        /// <summary> Create a compiler to produce an LLVM module with the default name and the given parameters </summary>
        /// <param name="settings"> Compiler settings </param>
        IRSteppableMapCompiler(const MapCompilerParameters& settings);

        /// <summary> Compiles the steppable model with the given function name </summary>
        /// <param name="map"> The map to be compiled </param>
        IRCompiledMap Compile(SteppableMap<ClockType> map);

    protected:
        virtual void EmitModelAPIFunctions(const DynamicMap& map) override;
        virtual std::string GetPredictFunctionName() const override;

    private:
        void EnsureValidMap(SteppableMap<ClockType>& map);
        void EmitStepFunction(SteppableMap<ClockType>& map, const std::string& functionName, emitters::Variable* pLastSampleTicksVar);
        void EmitWaitTimeForNextComputeFunction(SteppableMap<ClockType>& map, const std::string& functionNamePrefix, emitters::Variable* pLastSampleTicksVar);
        void EmitGetIntervalFunction(SteppableMap<ClockType>& map, const std::string& functionNamePrefix);
        llvm::Value* CallClockFunction(emitters::IRFunctionEmitter& function);
    };

    // Time signal contains: sample time, current time
    static constexpr size_t TimeSignalSize = 2;
    static const auto TimeTickVarType = emitters::GetVariableType<TimeTickType>();
}
}

#include "../tcc/IRSteppableMapCompiler.tcc"
