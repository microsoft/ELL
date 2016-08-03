////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRLoopEmitter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IREmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRFunctionEmitter;

		///<summary>Class that simplifies for loop creation</summary>
		class IRForLoopEmitter
		{
		public:
			///Construct a for loop emitter to emit code into the given function
			IRForLoopEmitter(IRFunctionEmitter& fn)
				: _fn(fn) {}

			///<summary>The block containing the body of the for loop</summary>
			llvm::BasicBlock* Body()
			{
				return _pBodyBlock;
			}
			///<summary>The typical for loop has an iteration count -- the "i". LoopEmitter manages that for you.
			/// The IterationVar() returns the llvm::Value that will contain "i"</summary>
			llvm::Value* IterationVar()
			{
				return _pIterationVar;
			}
			llvm::Value* LoadIterationVar();

			///<summary>Begin a for loop that repeats the given number of times. The </summary>
			llvm::BasicBlock* Begin(const int repeatCount)
			{
				return Begin(0, repeatCount, 1);
			}
			///<summary>Begin a for loop from iStartAt to iMaxValue, with the given step size</summary>
			llvm::BasicBlock* Begin(const int iStartAt, const int iMaxValue, const int stepSize);

			///<summary>Begin a for loop that repeats the given number of times</summary>
			llvm::BasicBlock* Begin(llvm::Value* pRepeatCount);

			///<summary>End this for loop</summary>
			void End();
			///<summary>reset..to start a new for loop</summary>
			void Clear();

		private:
			///<summary>Create the standard blocks used by a typical for loop... see member variables for details></summary>
			void CreateBlocks();
			void EmitIterationVar(ValueType type, llvm::Value* pStartValue);
			void EmitCondition(ComparisonType type, llvm::Value* pTestValue);
			void EmitIncrement(ValueType type, llvm::Value* pIncValue);
			llvm::BasicBlock* PrepareBody();

		private:
			IRFunctionEmitter& _fn;							// Loop written into this function
			llvm::BasicBlock* _pInitBlock = nullptr;			// The for loop is set up in this block - such as initializing iteration variables
			llvm::BasicBlock* _pConditionBlock = nullptr;	// Here we do the loop termination check
			llvm::BasicBlock* _pBodyBlock = nullptr;			// The body of the for loop
			llvm::BasicBlock* _pIncBlock = nullptr;			// Here we increment the iteration variable
			llvm::BasicBlock* _pAfterBlock = nullptr;		// When the loop is done, we branch to this block 
			llvm::Value* _pIterationVar = nullptr;
		};
	}
}
