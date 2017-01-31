////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRCompiledMap.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompiledMap.h"

// emitters
#include "IRExecutionEngine.h"
#include "IRModuleEmitter.h"
#include "ModuleEmitter.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"
#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

// utilities
#include "ConformingVector.h"
#include "TypeName.h"

// stl
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A map that can be compiled </summary>
    class IRCompiledMap : public CompiledMap
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="map"> The input map to compile </param>
        /// <param name="functionName"> The name of the function to compile the map to </param>
        /// <param name="optimize"> Flag indicating if the output should be optimized </param>
        IRCompiledMap(const model::DynamicMap& other, const std::string& functionName = "predict", bool optimize = true);

        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> The compiled map being moved. </param>
        IRCompiledMap(IRCompiledMap&& other);

        virtual ~IRCompiledMap() = default;

        /// <summary> Output the compiled model to the given file </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        virtual void WriteCode(const std::string& filePath) override;

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        /// <param name="format"> The format to write out </param>
        virtual void WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format) override;

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        /// <param name="format"> The format to write out </param>
        /// <param name="options"> The options to pass to the code generator </param>
        void WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options);

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="filePath"> The path to the file to write </param>
        virtual void WriteCodeHeader(const std::string& filePath) const override;

        /// <summary> Output the compiled model to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format to write out </param>
        virtual void WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format) override;

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        /// <param name="format"> The format to write out </param>
        /// <param name="options"> The options to pass to the code generator </param>
        void WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format, emitters::MachineCodeOutputOptions options);

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="streawm"> The stream to write the prototype to </param>
        virtual void WriteCodeHeader(std::ostream& stream) const override;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <returns> A string with the function prototype </returns>
        virtual std::string GetCodeHeaderString() const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IRCompiledMap"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Can this compiled map be used? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        virtual bool IsValid() const override;

    protected:
        virtual void Compile() override;

        virtual void SetNodeInput(model::InputNode<bool>* node, const std::vector<bool>& inputValues) const override;
        virtual void SetNodeInput(model::InputNode<int>* node, const std::vector<int>& inputValues) const override;
        virtual void SetNodeInput(model::InputNode<double>* node, const std::vector<double>& inputValues) const override;

        virtual std::vector<bool> ComputeBoolOutput(const model::PortElementsBase& outputs) const override;
        virtual std::vector<int> ComputeIntOutput(const model::PortElementsBase& outputs) const override;
        virtual std::vector<double> ComputeDoubleOutput(const model::PortElementsBase& outputs) const override;

        template <typename InputType, typename OutputType>
        void SetComputeFunction();

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        template <typename InputType>
        using ComputeFunction = std::function<void(const InputType*)>;

        std::string _moduleName = "ELL";

        std::unique_ptr<emitters::IRModuleEmitter> _module;
        std::unique_ptr<emitters::IRExecutionEngine> _executionEngine;

        // Only one of the entries in the tuple is active, depending on the input and output types of the map
        std::tuple<ComputeFunction<bool>, ComputeFunction<int>, ComputeFunction<double>> _computeInputFunction;
        std::tuple<utilities::ConformingVector<bool>, utilities::ConformingVector<int>, utilities::ConformingVector<double>> _cachedOutput;

        void EnsureValidMap(); // fixes up model if necessary and checks inputs/outputs are compilable
        void SetComputeFunction();

        template <typename InputType>
        void SetComputeFunctionForInputType();
    };
}
}

#include "../tcc/IRCompiledMap.tcc"
