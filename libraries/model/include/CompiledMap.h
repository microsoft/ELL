////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledMap.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// emitters
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
    /// <summary> Abstract base class for a map that has been compiled </summary>
    class CompiledMap : public model::DynamicMap
    {
    public:
        /// <summary> Default constructor </summary>
        CompiledMap();

        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model to wrap </param>
        /// <param name="inputs"> A vector of name/value pairs for the inputs this map uses </param>
        /// <param name="outputs"> A vector of name/value pairs for the outputs this map generates </param>
        CompiledMap(const model::Model& model, const std::vector<std::pair<std::string, model::InputNodeBase*>>& inputs, const std::vector<std::pair<std::string, model::PortElementsBase>>& outputs);

        /// <summary> Constructor </summary>
        ///
        /// <param name="map"> The input map to compile </param>
        /// <param name="functionName"> The name of the function to compile the map to </param>
        /// <param name="optimize"> Flag indicating if the output should be optimized </param>
        CompiledMap(const model::DynamicMap& other, const std::string& functionName = "predict", bool optimize = true);

        CompiledMap(const CompiledMap& other) = delete;

        CompiledMap(CompiledMap&& other) = default;

        virtual ~CompiledMap() = default;

        /// <summary> Get the name of function this map compiles to </summary>
        ///
        /// <returns> The name of function this map compiles to </returns>
        std::string GetFunctionName() { return _functionName; }

        /// <summary> Output the compiled model to the given file </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        virtual void WriteCode(const std::string& filePath) = 0;

        /// <summary> Output the compiled model to the given file with the given format </summary>
        ///
        /// <param name="filePath"> The file to write to </param>
        /// <param name="format"> The format to write out ("asm" or "bc") </param>
        virtual void WriteCode(const std::string& filePath, emitters::ModuleOutputFormat format) = 0;

        /// <summary> Output the compiled model to an output stream with the given format </summary>
        ///
        /// <param name="stream"> The stream to write to </param>
        /// <param name="format"> The format to write out ("asm" or "bc") </param>
        virtual void WriteCode(std::ostream& stream, emitters::ModuleOutputFormat format) = 0;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="filePath"> The path to the file to write </param>
        virtual void WriteCodeHeader(const std::string& filePath) const = 0;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <param name="streawm"> The stream to write the prototype to </param>
        virtual void WriteCodeHeader(std::ostream& stream) const = 0;

        /// <summary> Output a 'C'-style function prototype for the compiled function </summary>
        ///
        /// <returns> A string with the function prototype </returns>
        virtual std::string GetCodeHeaderString() const = 0;

        /// <summary> Can this compiled map be used? </summary>
        ///
        /// <returns> true if active, false if not. </returns>
        virtual bool IsValid() const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "CompiledMap"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;
        virtual void Compile() = 0;

        std::string _functionName;
        bool _optimize = false;
    };
}
}
