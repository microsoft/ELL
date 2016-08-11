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
#include "UnaryOperationNode.h"
#include "ElementSelectorNode.h"
#include "BinaryPredicateNode.h"

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

			///<summary>Edit the prediction function declaration</summary>
			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) = 0;
			///<summary>End the function</summary>
			virtual void EndFunction() = 0;

			///<summary>Variable allocator</summary>
			VariableAllocator& Variables() { return _variables; }

			///<summary>Output the compiled model to the given file</summary>
			virtual void WriteToFile(const std::string& filePath) = 0;
		
		protected:
			///<summary>Compile a ConstantNode</summary>
			virtual void CompileConstantNode(const model::Node& node);
			///<summary>Compile an InputNode</summary>
			virtual void CompileInputNode(const model::Node& node);
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::Node& node);
			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const model::Node& node);
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const model::Node& node);
			///<summary>Compile a BinaryPredicateNode</summary>
			virtual void CompileBinaryPredicateNode(const model::Node& node);

			//---------------------------------------------------
			//
			// These methods may be implemented by specific compilers
			//
			//---------------------------------------------------
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<double>& node) = 0;
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<int>& node) = 0;
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<bool>& node) = 0;

			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<double>& node) = 0;
			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<int>& node) = 0;

			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<double>& node) = 0;
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<int>& node) = 0;

			///<summary>Compile a binary predicate</summary>
			virtual void CompileBinaryPredicateNode(const nodes::BinaryPredicateNode<double>& node) = 0;

			///<summary>Compile a DotProductNode</summary>
			virtual void CompileDotProductNode(const model::Node& node) = 0;
			///<summary>Compile a AccumulatorNode</summary>
			virtual void CompileAccumulatorNode(const model::Node& node) = 0;
			///<summary>Compile a DelayNode</summary>
			virtual void CompileDelayNode(const model::Node& node) = 0;
			///<summary>Compile a UnaryNode</summary>
			virtual void CompileUnaryNode(const model::Node& node) = 0;
			///<summary>Compile an ElementSelectorNode</summary>
			virtual void CompileElementSelectorNode(const model::Node& node) = 0;

			///<summary>Ensure a variable is emitted</summary>
			virtual void EnsureVarEmitted(Variable* pVar) = 0;
			///<summary>The model has unexpected leaf nodes</summary>
			virtual void HandleLeafNode(const model::Node& node);

			virtual void VerifyIsScalar(const model::InputPortBase& port);
			virtual void VerifyIsScalar(const model::OutputPortBase& port);
			virtual void VerifyIsPureBinary(const model::Node& node);

		protected:
			///<summary>Create a variable to store computed output for the given output port. The variable
			/// will be emitted lazily. </summary>
			Variable* AllocVar(model::OutputPortBase* pPort);
			///<summary>Get the variable for output port</summary>
			Variable* GetVariableFor(const model::OutputPortBase* pPort);
			///<summary>Ensure the variable for output port element exists</summary>
			Variable* EnsureVariableFor(const model::OutputPortBase* pPort);
			///<summary>Get the variable for output port element</summary>
			Variable* GetVariableFor(const model::OutputPortElement elt);
			///<summary>Ensure the variable for output port element exists</summary>
			Variable* EnsureVariableFor(const model::OutputPortElement elt);
			///<summary>Associate the given variable with the output port</summary>
			void SetVariableFor(const model::OutputPortBase* pPort, Variable* pVar);

			///<summary>Allocate a *runtime* variable..</summary>
			void AllocVar(Variable& var);
			///<summary>Free a *runtime* variable..</summary>
			void FreeVar(Variable& var);

			bool IsNodeType(const std::string& nodeTypeName , const std::string& typeName);
			///<summary>Convert a port type to a compiler specific value type</summary>
			ValueType ToValueType(model::Port::PortType type);
			///<summary>Models get turned into functions. To declare the function, we need to know inputs and outputs.</summary>
			void CollectInputsAndOutputs(model::Model& model);

			///<summary>Compile a ConstantNode</summary>
			template<typename T>
			void CompileConstant(const nodes::ConstantNode<T>& node);
			///<summary>Compile a boolean ConstantNode, which we have to handle in a special way</summary>
			void CompileConstantBool(const nodes::ConstantNode<bool>& node);

			///<summary>Translate the binary operation operator into a strongly typed operator for LLVM</summary>
			template<typename T>
			OperatorType GetOperator(const nodes::BinaryOperationNode<T>& node) const;

			///<summary>Translate the binary predicate operator into a more strongly typed operator for LLVM</summary>
			template<typename T>
			ComparisonType GetComparison(const nodes::BinaryPredicateNode<T>& node) const;

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

#include "../tcc/Compiler.tcc"
