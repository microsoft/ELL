////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variable.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Types.h"
#include "IntegerStack.h"
#include <memory>
#include <string>
#include <vector>

namespace emll
{
	namespace compiler
	{
		///<summary>Metadata about an emitted variable.</summary>
		struct EmittedVar
		{
			///<summary>Has this variable been declared already?</summary>
			bool isNew = false;		
			///<summary>Variable #</summary>
			uint64_t varIndex = 0;

			///<summary>Set fields to default</summary>
			void Clear();
		};

		///<summary>Allocator to alloc, free and reuse emitted (physical) variables</summary>
		class EmittedVarAllocator
		{
		public:
			///<summary>Alloc a variable</summary>
			EmittedVar Alloc();
			///<summary>Free a variable</summary>
			void Free(EmittedVar& var);

		private:
			utilities::IntegerStack _varStack;
		};

		///<summary>Our compilers work with scoped variables</summary>
		enum class VariableScope
		{
			///<summary>Literal variable</summary>
			literal,
			///<summary>Local (stack) variable</summary>
			local,
			///<summary>Global variable</summary>
			global,
			///<summary>Heap allocated variable</summary>
			heap,
			///<summary>Variable returned by a helper function</summary>
			rValue,
			///<summary>Input function argument</summary>
			input,
			///<summary>Output function argument</summary>
			output
		};

		class VariableAllocator;

		///<summary>
		/// Emll Model output ports are bound to logical variables. Variables represent ports and port elements in machine terms: the local, 
		/// global or heap scalars and vectors that they are being emitted as. 
		/// Our compilers can emit code/IR in multiple formats. Hence we need a format independant scheme
		/// </summary>
		class Variable
		{
		public:
			///<summary>Variable options</summary>
			enum VariableFlags
			{
				none = 0,
				///<summary>Mutable or constant</summary>
				isMutable		= 0x00000001,
				///<summary>Initialized or not</summary>
				hasInitValue		= 0x00000002,
				///<summary>Is this a offset into a vector or array</summary>
				isVectorRef		= 0x00000004,
				///<summary>Is this a helper for a tree node</summary>
				isTreeNode		= 0x00000008
			};

		public:
			///<summary>Variable data type</summary>
			ValueType Type() const
			{
				return _type;
			}
			///<summary>Variable scope</summary>
			VariableScope Scope() const
			{
				return _scope;
			}
			///<summary>Variable dimension - scalar or vector</summary>
			virtual size_t Dimension() const
			{
				return 1;
			}
			///<summary>Is this a vector variable?</summary>
			bool IsVector() const
			{
				return !IsScalar();
			}
			///<summary>Is this a scalar variable?</summary>
			virtual bool IsScalar() const
			{
				return (Dimension() == 1);
			}
			///<summary>When a variable is emitted, it is bound to a name</summary>
			const std::string& EmittedName() const
			{
				return _emittedName;
			}
			///<summary>Was the variable emitted?</summary>
			bool HasEmittedName() const
			{
				return (_emittedName.length() > 0);
			}
			///<summary>Set the emitted name for the variable</summary>
			void SetEmittedName(std::string emittedName);


			///<summary>Is this a literal variable?</summary>
			bool IsLiteral() const
			{
				return (_scope == VariableScope::literal);
			}
			///<summary>Is this a global variable?</summary>
			bool IsGlobal() const
			{
				return (_scope == VariableScope::global);
			}
			///<summary>Is this an RValue variable?</summary>
			bool IsRValue() const
			{
				return (_scope == VariableScope::rValue);
			}
			///<summary>Is this a variable mutable?</summary>
			bool IsMutable() const
			{
				return TestFlags(VariableFlags::isMutable);
			}
			///<summary>Is this a variable a constant?</summary>
			bool IsConstant() const
			{
				return !IsMutable();
			}		
			///<summary>Is this variable a reference into a vector?</summary>
			bool IsVectorRef() const
			{
				return TestFlags(VariableFlags::isVectorRef);
			}
			///<summary>Does this variable reference a tree node?</summary>
			bool IsTreeNode() const
			{
				return TestFlags(VariableFlags::isTreeNode);
			}
			///<summary>Does the variable need to be initialized?</summary>
			bool HasInitValue() const
			{
				return TestFlags(VariableFlags::hasInitValue);
			}
			///<summary>True if this a new variable. False it has it already been declared?</summary>
			bool IsNew() const
			{
				return _emittedVar.isNew;
			}
			///<summary>Test if the given set of flags are set</summary>
			bool TestFlags(int flags) const
			{
				return ((_flags & flags) != 0);
			}

			///<summary>Bind the logical variable to a physical one</summary>
			void AssignVar(EmittedVar var)
			{
				_emittedVar = var;
			}
			///<summary>Get the physical variable bound to this logical variable</summary>
			EmittedVar GetAssignedVar()
			{
				return _emittedVar;
			}
		
		protected:

			Variable(const ValueType type, const VariableScope scope, int flags = VariableFlags::none);
			void SetFlags(const VariableFlags flag)
			{
				_flags |= (int)flag;
			}
			void ClearFlags(const VariableFlags flag)
			{
				_flags &= (~((int)flag));
			}

		private:
			std::string _emittedName;
			ValueType _type;
			VariableScope _scope;
			int _flags;
			EmittedVar _emittedVar;
		};

		///<summary>Allocator for logical variables</summary>
		class VariableAllocator
		{
		public:
			///<summary>Add variable or a given type</summary>
			template <typename VarType, typename... Args>
			VarType* AddVariable(Args&&... args);

			///<summary>Add a scalar</summary>
			Variable* AddLocalScalarVariable(ValueType type)
			{
				return AddLocalScalarVariable(VariableScope::local, type);
			}
			///<summary>Add a scalar</summary>
			Variable* AddLocalScalarVariable(VariableScope scope, ValueType type);
			///<summary>Add a vector</summary>
			Variable* AddVectorVariable(VariableScope scope, ValueType type, int size);
			///<summary>Add a reference to vector element</summary>
			Variable* AddVectorElementVariable(ValueType type, Variable& src, int offset);

		private:
			std::vector<std::shared_ptr<Variable>> _variables;
		};
	}
}

#include "../tcc/Variable.tcc"

