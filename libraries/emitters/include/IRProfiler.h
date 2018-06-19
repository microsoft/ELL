////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRProfiler.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
#include "EmitterTypes.h"
#include "IRLocalScalar.h"

// llvm
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

// stl
#include <string>
#include <unordered_set>

// External API for profiling functions
extern "C" {

/// <summary> A struct that holds information about a profile region. </summary>
struct ProfileRegionInfo
{
    int64_t count;
    double totalTime;
    const char* name;
};
}

namespace ell
{
namespace emitters
{
    // import ProfileRegionInfo into this namespace
    using ::ProfileRegionInfo;

    class IRFunctionEmitter;
    class IRModuleEmitter;
    class IRProfiler;

    /// <summary> 
    /// A class representing a function-scoped region to profile. 
    /// Emitted code within this region will have its total runtime measured, and the total number of times run tallied.
    /// </summary>
    class IRProfileRegion
    {
    public:
        /// <summary> Constructor. </summary>
        IRProfileRegion(IRFunctionEmitter& function, const std::string& name);

        /// <summary> Enter the profiling region: increment the visit count and begin timing. </summary>
        void Enter();

        /// <summary> Exit the profiling region: accumulate the time spent since calling `Enter()`. </summary>
        void Exit();

    private:
        friend IRProfiler;
        IRProfileRegion(IRFunctionEmitter& function, IRProfiler& profiler, const std::string& name, IRLocalScalar index);
        
        IRFunctionEmitter& GetFunction() { return _function; }
        IRLocalScalar GetIndex() const { return _index; }
        IRLocalScalar GetStartTime() const { return _startTime; }
        void SetStartTime(const IRLocalScalar& time) { _startTime = time; }
        
        IRFunctionEmitter& _function;
        IRProfiler& _profiler;
        IRLocalScalar _index;
        IRLocalScalar _startTime;
    };

    /// <summary> 
    /// An RAII class to make it easier to create function-local profile regions. Any code between
    /// this object's construction and destruction will be profilied as a region.
    /// </summary>
    class IRProfileRegionBlock
    {
    public:
        /// <summary> Constructor. Creates an IRProfileRegion. Any emitted code after this will be included in that region. </summary>
        ///
        /// <param name="function"> The function containing the code to be profiled. </param>
        /// <param name="name"> The name of the region to create. </param>
        IRProfileRegionBlock(IRFunctionEmitter& function, const std::string& name);

        IRProfileRegionBlock(const IRProfileRegionBlock&) = delete;
        IRProfileRegionBlock(IRProfileRegionBlock&&) = default;

        /// <summary> Destructor. Exits the profile region. </summary>
        ~IRProfileRegionBlock();

    private:
        IRProfileRegion _region;
    };

    /// <summary> A class that manages profile code generation. </summary>
    class IRProfiler
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="module"> The `IRModuleEmitter` to compile the profiling information into. </param>
        /// <param name="enableProfiling"> Indicates whether profiling should be enabled. </param>
        IRProfiler(emitters::IRModuleEmitter& module, bool enableProfiling);

        /// <summary> 
        /// Emit static initialization code to allocate and initialize info and perf counter data. 
        /// Called by the IRModuleEmitter that owns this profiler. 
        /// </summary>
        void Init();

        /// <summary> Get the name of the emitted "GetNumRegions" function. </summary>
        ///
        /// <returns> The name of the emitted "GetNumRegions" function. </returns>
        std::string GetGetNumRegionsFunctionName() const;

        /// <summary> Get the name of the emitted "GetRegionProfilingInfo" function. </summary>
        ///
        /// <returns> The name of the emitted "GetRegionProfilingInfo" function. </returns>
        std::string GetGetRegionProfilingInfoFunctionName() const;

        /// <summary> Get the name of the emitted "ResetRegionProfilingInfo" function. </summary>
        ///
        /// <returns> The name of the emitted "ResetRegionProfilingInfo" function. </returns>
        std::string GetResetRegionProfilingInfoFunctionName() const;

    private:
        friend IRProfileRegion;

        std::string GetNamespacePrefix() const;
        llvm::StructType* GetRegionType() const;
        IRLocalScalar GetCurrentTime(IRFunctionEmitter& function);

        // Actual implementations of the functions in IRProfileRegion
        void InitRegion(IRProfileRegion& region, const std::string& name);
        void EnterRegion(IRProfileRegion& region);
        void ExitRegion(IRProfileRegion& region);
        void ResetRegionCounts(IRFunctionEmitter& function, const IRLocalScalar& regionIndex);

        // Generating functions
        void EmitProfilerFunctions();
        void EmitGetRegionBufferFunction();
        void EmitGetNumRegionsFunction();
        void EmitGetRegionProfilingInfoFunction();
        void EmitResetRegionProfilingInfoFunction();
        
        // Lower-level codegen
        // CreateRegion returns the index of the new region
        IRLocalScalar CreateRegion(IRFunctionEmitter& function);
        void CreateStructTypes();
        void CreateRegionData();
        void ReallocateRegionData();
        void FixUpGetNumRegionsFunction();
        void FixUpGetRegionBufferFunction();

        llvm::Value* GetRegionBuffer(emitters::IRFunctionEmitter& function);
        llvm::Value* GetNumRegions(emitters::IRFunctionEmitter& function);
        llvm::Value* GetRegionPointer(emitters::IRFunctionEmitter& function, llvm::Value* index);

        emitters::IRModuleEmitter* _module = nullptr;
        bool _profilingEnabled = false;

        std::unordered_set<std::string> _regionNames;

        // Cache these often-used functions so we don't have to keep looking them up by name
        llvm::Function* _getNumRegionsFunction = nullptr;
        llvm::Function* _getRegionBufferFunction = nullptr;

        llvm::StructType* _profileRegionType = nullptr;
        llvm::GlobalVariable* _profileRegionsArray = nullptr;
        int _regionCount = 0;
    };
}
}
