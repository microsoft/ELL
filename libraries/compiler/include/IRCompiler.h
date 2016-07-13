#pragma once
#include "Compiler.h"
#include "IRInclude.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		class IRCompiler : public Compiler
		{
		public:
			IRCompiler(const std::string& moduleName, std::ostream& os);

			void DebugDump();

		protected:
			virtual void Begin() override;
			virtual void End() override;
			virtual void BeginMain(const std::string& functionName) override;
			virtual void EndMain() override;

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