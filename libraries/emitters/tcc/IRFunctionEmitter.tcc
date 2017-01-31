////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Literal(ValueType value)
    {
        return _pEmitter->Literal(value);
    }

    template <typename InputType, typename OutputType>
    llvm::Value* IRFunctionEmitter::CastValue(llvm::Value* pValue)
    {
        return _pEmitter->CastValue<InputType, OutputType>(pValue);
    }

    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Cast(llvm::Value* pValue)
    {
        return Cast(pValue, GetVariableType<ValueType>());
    }

    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Malloc(int64_t size)
    {
        return Malloc(GetVariableType<ValueType>(), size);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryMove(llvm::Value* pPointer, int sourceOffset, int destinationOffset, int size)
    {
        assert(pPointer != nullptr);
        auto pSource = PointerOffset(pPointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pPointer, Literal(destinationOffset));
        int byteCount = size * sizeof(ValueType);
        _pEmitter->MemoryMove(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(llvm::Value* pSourcePointer, int sourceOffset, llvm::Value* pDestinationPointer, int destinationOffset, int size)
    {
        auto pSource = PointerOffset(pSourcePointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pDestinationPointer, Literal(destinationOffset));
        int byteCount = size * sizeof(ValueType);
        _pEmitter->MemoryCopy(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::ShiftAndUpdate(llvm::Value* buffer, int bufferSize, int shiftCount, llvm::Value* pNewData, llvm::Value* pShiftedData)
    {
        assert(buffer != nullptr);
        assert(shiftCount <= bufferSize);

        if (pShiftedData != nullptr)
        {
            MemoryCopy<ValueType>(buffer, 0, pShiftedData, 0, shiftCount);
        }
        if (shiftCount < bufferSize)
        {
            MemoryMove<ValueType>(buffer, shiftCount, 0, (bufferSize - shiftCount));
        }
        MemoryCopy<ValueType>(pNewData, 0, buffer, (bufferSize - shiftCount), shiftCount);
    }
}
}
