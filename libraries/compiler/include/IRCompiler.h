#pragma once
#include "Compiler.h"
#include "IRInclude.h"

namespace emll
{
	namespace compiler
	{
		class IRCompiler : public Compiler
		{
		public:
			IRCompiler();

		private:
			IREmitter _emitter;
			IRModuleEmitter _module;
		};
	}
}