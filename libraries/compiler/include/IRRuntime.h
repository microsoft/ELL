#pragma once

#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRRuntime
		{
		public:
			IRRuntime(IRModuleEmitter& module);

			llvm::Function* DotProductF();
		
		private:

			///<summary>Emits a new callable function to compute dot products</summary>
			llvm::Function* EmitDotProductFunctionF();

		private:
			IRModuleEmitter& _module;
			NamedValueTypeList _args;
			llvm::Function* _pfnDotProductF = nullptr;
		};
	}
}
