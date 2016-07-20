#pragma once
#include "Compiler.h"
#include "IRInclude.h"
#include "ScalarVar.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		class IRCompiler : public Compiler
		{
		public:
			IRCompiler(const std::string& moduleName, std::ostream& os);

			virtual void Begin() override;
			virtual void End() override;

			llvm::Value* GetVariable(const std::string& name);
			llvm::Value* EnsureVariable(Variable& var);

			void DebugDump();

		protected:
			virtual void BeginMain(const std::string& functionName, NamedValueTypeList& args) override;
			virtual void EndMain() override;
			virtual void Compile(LiteralNode& node) override;

			llvm::Value* EmitScalar(Variable& var);

			llvm::Value* EmitLocalScalar(Variable& var);
			llvm::Value* EmitGlobalScalar(Variable& var);
			llvm::Value* EmitVectorRef(Variable& var);

			template<typename T>
			llvm::Value* EmitLocalScalar(LocalScalarVar<T>& var);
			template<typename T>
			llvm::Value* EmitGlobalScalar(GlobalScalarVar<T>& var);

		private:

		private:
			std::ostream& _os;
			IREmitter _emitter;
			IRModuleEmitter _module;
			IRFunctionEmitter _fn;
			IRVariableTable _vars;
			IRVariableTable _globals;
		};
	}
}

#include "../tcc/IRCompiler.tcc"