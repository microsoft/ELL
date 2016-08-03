////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRFunctionEmitter.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Value* IRFunctionEmitter::Literal(T value)
		{
			return _pEmitter->Literal(value);
		}

		template<typename T>
		llvm::Value* IRFunctionEmitter::Cast(llvm::Value* pValue)
		{
			return Cast(pValue, GetValueType<T>());
		}

		template<typename T>
		llvm::Value* IRFunctionEmitter::Malloc(int64_t count)
		{
			return Malloc(GetValueType<T>(), count);
		}

		template<typename T>
		void IRFunctionEmitter::MemMove(llvm::Value* pPtr, int fromOffset, int destOffset, int count)
		{
			assert(pPtr != nullptr);
			auto pSrc = PtrOffset(pPtr, Literal(fromOffset));
			auto pDest = PtrOffset(pPtr, Literal(destOffset));
			int byteCount = count * sizeof(T);
			_pEmitter->MemMove(pSrc, pDest, Literal(byteCount));
		}

		template<typename T>
		void IRFunctionEmitter::MemCopy(llvm::Value* pPtrSrc, int srcOffset, llvm::Value* pPtrDest, int destOffset, int count)
		{
			auto pSrc = PtrOffset(pPtrSrc, Literal(srcOffset));
			auto pDest = PtrOffset(pPtrDest, Literal(destOffset));
			int byteCount = count * sizeof(T);
			_pEmitter->MemCopy(pSrc, pDest, Literal(byteCount));
		}

		template<typename T>
		void IRFunctionEmitter::ShiftAndUpdate(llvm::Value* pBuffer, int bufferCount, int shiftCount, llvm::Value* pNewData, llvm::Value* pShiftedData)
		{
			assert(pBuffer != nullptr);
			assert(shiftCount <= bufferCount);

			if (pShiftedData != nullptr)
			{
				MemCopy<T>(pBuffer, 0, pShiftedData, 0, shiftCount);
			}
			if (shiftCount < bufferCount)
			{
				MemMove<T>(pBuffer, shiftCount, 0, (bufferCount - shiftCount));
			}
			MemCopy<T>(pNewData, 0, pBuffer, (bufferCount - shiftCount), shiftCount);
		}
	}
}
