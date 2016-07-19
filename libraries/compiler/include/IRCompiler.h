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
			virtual void BeginMain(const std::string& functionName) override;
			virtual void EndMain() override;
			virtual void Compile(LiteralNode& node) override;

			llvm::Value* EmitScalar(Variable& var);

			llvm::Value* EmitLocalScalar(Variable& var);
			template<typename T>
			llvm::Value* EmitLocalScalar(LocalScalarVar<T>& var);
			llvm::Value* EmitGlobalScalar(Variable& var);
			template<typename T>
			llvm::Value* EmitGlobalScalar(GlobalScalarVar<T>& var);

		private:

			void AddArgs(NamedValueTypeList& args, const std::string& namePrefix, const std::vector<const model::Node*>& nodes);
			void AddArgs(NamedValueTypeList& args, const std::string& name, const model::OutputPortBase* pOutput);

			std::string MakeVarName(const std::string& namePrefix, size_t i);

		private:
			std::ostream& _os;
			IREmitter _emitter;
			IRModuleEmitter _module;
			IRFunctionEmitter _fn;
			IRVariableTable _vars;
		};
	}
}

#include "../tcc/IRCompiler.tcc"