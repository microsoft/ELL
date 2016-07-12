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
			virtual void BeginFunction(const std::string& functionName, const VariableDecl& input, const VariableDecl& output) override;
			virtual void EndFunction() override;

		private:

		private:
			std::ostream& _os;
			IREmitter _emitter;
			IRModuleEmitter _module;
			IRFunctionEmitter _fn;
		};
	}
}