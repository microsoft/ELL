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
			Literal,
			///<summary>Local (stack) variable</summary>
			Local,
			///<summary>Global variable</summary>
			Global,
			///<summary>Heap allocated variable</summary>
			Heap,
			///<summary>Input function argument</summary>
			Input,
			///<summary>Output function argument</summary>
			Output
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
				isComputed		= 0x00000008
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
				return (_scope == VariableScope::Literal);
			}
			///<summary>Is this a global variable?</summary>
			bool IsGlobal() const
			{
				return (_scope == VariableScope::Global);
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
			///<summary>Does the variable need to be initialized?</summary>
			bool HasInitValue() const
			{
				return TestFlags(VariableFlags::hasInitValue);
			}
			bool IsComputed() const
			{
				return TestFlags(VariableFlags::isComputed);
			}
			bool IsNew() const
			{
				return _emittedVar.isNew;
			}
			bool TestFlags(int flags) const
			{
				return ((_flags & flags) != 0);
			}

			///<summary>Bind the logical variable to a physical one</summary>
			void AssignVar(EmittedVar var)
			{
				_emittedVar = var;
			}
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
				return AddLocalScalarVariable(VariableScope::Local, type);
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

