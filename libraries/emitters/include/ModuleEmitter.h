////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModuleEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "TargetDevice.h"
#include "Variable.h"

// stl
#include <string>

namespace ell
{
namespace emitters
{
    /// <summary> Code output formats </summary>
    enum class ModuleOutputFormat
    {
        ir,
        bitcode,
        assembly,
        objectCode,
        cHeader,
        swigInterface
    };

    /// <summary> Standard compiler switches. </summary>
    struct CompilerParameters
    {
        bool unrollLoops = false;
        bool inlineOperators = true;
        bool useBlas = false;
        bool optimize = true;
        bool includeDiagnosticInfo = false;

        TargetDevice targetDevice;
    };

    /// <summary> Abstract base class for ELL compilers </summary>
    class ModuleEmitter
    {
    public:
        ModuleEmitter();
        virtual ~ModuleEmitter() = default;

        /// <summary> Return the base compiler settings </summary>
        ///
        /// <returns> The settings for the compiler </returns>
        const CompilerParameters& GetCompilerParameters() const { return _parameters; }

        /// <summary> Set the base compiler settings </summary>
        ///
        /// <param name="parameters"> The settings for the compiler to use </param>
        void SetCompilerParameters(const CompilerParameters& parameters);

        // Note, this differs from IRModuleEmitter::BeginFunction only by return type
        /// <summary> Set a function declaration. Note that BeginMapPredictFunction can't be called from within a function - it completes the currently-being-emitted function </summary>
        ///
        /// <param name="functionName"> The name of the function to create </param>
        /// <param name="args"> The names and types of the arguments </param>
        virtual void BeginMapPredictFunction(const std::string& functionName, NamedVariableTypeList& args) = 0;

        /// <summary> End the function </summary>
        virtual void EndMapPredictFunction() = 0;

        /// <summary> Indicates if the given function has any associated comments </summary>
        ///
        /// <param name="functionName"> The name of the function </param>
        ///
        /// <returns> `true` if the function has any comments associated with it </returns>
        virtual bool HasFunctionComments(const std::string& functionName) = 0;

        /// <summary> Get the comments associated with the given function. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        ///
        /// <returns> The comments for the function, as a vector of strings </returns>
        virtual std::vector<std::string> GetFunctionComments(const std::string& functionName) = 0;

        /// <summary> Associates some comment text with the given function. </summary>
        ///
        /// <param name="functionName"> The name of the function. </param>
        /// <param name="comments"> The comments for the function. </param>
        virtual void SetFunctionComments(const std::string& functionName, const std::vector<std::string>& comments) = 0;

        /// <summary> Indicates if the module or given function has the associated metadata. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        ///
        /// <returns> `true` if the function has the metadata associated with it. </returns>
        virtual bool HasMetadata(const std::string& functionName, const std::string& tag) = 0;

        /// <summary> Gets the metadata associated with the module or given function. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        ///
        /// <returns> The metadata values, as a vector of strings. </returns>
        virtual std::vector<std::string> GetMetadata(const std::string& functionName, const std::string& tag) = 0;

        /// <summary> Associates metadata with the module or given function. </summary>
        ///
        /// <param name="functionName"> The name of the function for function-level metadata, or empty string for the module. </param>
        /// <param name="tag"> The metadata tag. </param>
        /// <param name="content"> Optional metadata content. </param>
        virtual void InsertMetadata(const std::string& functionName, const std::string& tag, const std::string& value = "") = 0;

        /// <summary> Variable allocator </summary>
        ///
        /// <returns> The variable allocator </returns>
        VariableAllocator& Variables() { return _variables; }

        // TODO: What does "runtime" variable mean? Stack/heap? This is only called in 1 place, from MapCompiler::AllocateNodeFunctionArgument
        /// <summary> Allocate a *runtime* variable.. </summary>
        void AllocateVariable(Variable& var);

        /// <summary> Output the compiled model to the given file, using file extension to determine output format </summary>
        ///
        /// <param name="filePath"> The path of the file to write to </param>
        virtual void WriteToFile(const std::string& filePath);

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The path of the file to write to </param>
        /// <param name="format"> The format of the output </param>
        virtual void WriteToFile(const std::string& filePath, ModuleOutputFormat format);

        /// <summary> Output the compiled model to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format of the output </param>
        virtual void WriteToStream(std::ostream& stream, ModuleOutputFormat format) = 0;

        /// <summary> Return the appropriate output file type for the given filename extension </summary>
        ///
        /// <param name="extension"> The extension </param>
        /// <returns> The format of the output </returns>
        static ModuleOutputFormat GetFormatFromExtension(const std::string& extension);

    protected:
        void Reset();
        void FreeVariable(Variable& var);

    private:
        CompilerParameters _parameters;

        EmittedVariableAllocator _inputVars; // Runtime input variable table
        EmittedVariableAllocator _outputVars; // Runtime output variable table
        EmittedVariableAllocator _localVars; // Runtime local variable table (don't these belong to a function?)

        EmittedVariableAllocator _globalVars; // Runtime global variable table
        EmittedVariableAllocator _literalVars; // Runtime literal variable table
        EmittedVariableAllocator _rValueVars; // Runtime RValues variable table (what scope?)

        VariableAllocator _variables; // variable object manager
    };
}
}
