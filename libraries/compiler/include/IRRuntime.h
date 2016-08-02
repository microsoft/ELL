////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRRuntime.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"

namespace emll
{
	namespace compiler
	{
		///<summary>Manages external as well as compiler auto-generated functions</summary>
		class IRRuntime
		{
		public:
			IRRuntime(IRModuleEmitter& module);

			llvm::Function* DotProductF();

			///<summary>Get the sqrt function</summary>
			template<typename T>
			llvm::Function* Sqrt();
		
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

#include "../tcc/IRRuntime.tcc"
