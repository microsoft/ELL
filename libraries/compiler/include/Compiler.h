////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Compiler.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelEx.h"
#include "Types.h"
#include "SymbolTable.h"
#include "Variable.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#
#include "ConstantNode.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "BinaryOperationNode.h"
#include "DotProductNode.h"
#include "SumNode.h"
#include "AccumulatorNode.h"
#include "DelayNode.h"

#include <functional>

namespace emll
{
	namespace compiler
	{
		///<summary>Standard compiler switches. These will evolve as we do deeper compiler work</summary>
		class CompilerSettings
		{
		public:
			CompilerSettings() = default;
			CompilerSettings(const CompilerSettings& src);

			bool& ShouldUnrollLoops() { return _unrollLoops; }
			bool& ShouldInlineOperators() { return _inlineOperators; }

		private:
			bool _unrollLoops = false;
			bool _inlineOperators = true;
		};

		/// <summary>Abstract base class for EMLL compilers</summary>
		class Compiler
		{
		public:

		public:
			Compiler();
			virtual ~Compiler() = default;

			///<summary>Return the base compiler settings</summary>
			CompilerSettings& Settings() { return _settings; }

			///<summary>Compile the model into a function with the given name</summary>
			void CompileModel(const std::string& functionName, model::Model& model);

			virtual void CompileConstantNode(const model::Node& node) = 0;
			virtual void CompileInputNode(const model::Node& node) = 0;
			virtual void CompileOutputNode(const model::Node& node) = 0;
			virtual void CompileBinaryNode(const model::Node& node) = 0;
			virtual void CompileDotProductNode(const model::Node& node) = 0;
			virtual void CompileSumNode(const model::Node& node) = 0;
			virtual void CompileAccumulatorNode(const model::Node& node) = 0;
			virtual void CompileDelayNode(const model::Node& node) = 0;

			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) = 0;
			virtual void EndFunction() = 0;
			
			///<summary>Variable allocator</summary>
			VariableAllocator& Variables() { return _variables; }
			///<summary>Create a variable to store computed output for the given output port. The variable
			/// will be emitted lazily. </summary>
			Variable* AllocVar(model::OutputPortBase* pPort);
			///<summary>Get the variable for output port</summary>
			Variable* GetVariableFor(const model::OutputPortBase* pPort);
			///<summary>Get the variable for output port element</summary>
			Variable* GetVariableFor(const model::OutputPortElement elt);
			///<summary>Associate the given variable with the output port</summary>
			void SetVariableFor(const model::OutputPortBase* pPort, Variable* pVar);

		protected:
			///<summary>Allocate a *runtime* variable..</summary>
			void AllocVar(Variable& var);
			///<summary>Free a *runtime* variable..</summary>
			void FreeVar(Variable& var);

			bool IsNodeType(const std::string& nodeTypeName , const std::string& typeName);
			///<summary>Convert a port type to a compiler specific value type</summary>
			ValueType ToValueType(model::Port::PortType type);
			///<summary>Models get turned into functions. To declare the function, we need to know inputs and outputs.</summary>
			void CollectInputsAndOutputs(model::Model& model);

		private:
			Variable* AllocArg(const model::OutputPortBase* pPort, bool isInput);
			void Reset();
			
		private:
			EmittedVarAllocator _inputVars;			// Runtime variable table
			EmittedVarAllocator _outputVars;			// Runtime variable table
			EmittedVarAllocator _literalVars;		// Runtime variable table
			EmittedVarAllocator _localVars;			// Runtime variable table
			EmittedVarAllocator _globalVars;			// Runtime variable table

			VariableAllocator _variables;			// variable allocator
			NamedValueTypeList _args;				// function arguments
			// Maps output ports to runtime variables
			std::unordered_map<const model::OutputPortBase*, Variable*> _portToVarMap;

			CompilerSettings _settings;
		};
	}
}
