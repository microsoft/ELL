////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Needed in the tcc because IRLocalValueHelpers.h needs IRFunctionEmitter to be defined
#include "IRLocalValueOperations.h"

namespace ell
{
namespace emitters
{
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    IRLocalScalar IRFunctionEmitter::LocalScalar(ValueType value)
    {
        return IRLocalScalar(*this, Literal(value));
    }

    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Literal(ValueType value)
    {
        return _pEmitter->Literal(value);
    }

    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Pointer(ValueType* value)
    {
        return _pEmitter->Pointer(value);
    }

    template <typename InputType, typename OutputType>
    llvm::Value* IRFunctionEmitter::CastValue(llvm::Value* pValue)
    {
        return _pEmitter->CastValue<InputType, OutputType>(pValue);
    }

    template <typename ValueType>
    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, ValueType leftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
    {
        assert(pRightValue != nullptr);

        auto forLoop = ForLoop();
        llvm::Value* pLeftItem = Literal(leftValue);
        forLoop.Begin(size);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pRightItem = ValueAt(pRightValue, i);
            llvm::Value* pTemp = Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        }
        forLoop.End();
    }

    template <typename ValueType>
    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, ValueType rightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator)
    {
        assert(pLeftValue != nullptr);

        auto forLoop = ForLoop();
        llvm::Value* pRightItem = Literal(rightValue);
        forLoop.Begin(size);
        {
            auto i = forLoop.LoadIterationVariable();
            llvm::Value* pLeftItem = ValueAt(pLeftValue, i);
            llvm::Value* pTemp = Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        }
        forLoop.End();
    }

    template <typename ValueType>
    llvm::Value* IRFunctionEmitter::Malloc(int64_t size)
    {
        return Malloc(GetVariableType<ValueType>(), size);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryMove(llvm::Value* pPointer, int sourceOffset, int destinationOffset, int count)
    {
        assert(pPointer != nullptr);
        auto pSource = PointerOffset(pPointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pPointer, Literal(destinationOffset));
        int byteCount = count * sizeof(ValueType);
        _pEmitter->MemoryMove(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* pDestinationPointer, int count)
    {
        auto pSource = PointerOffset(pSourcePointer, 0);
        auto pDestination = PointerOffset(pDestinationPointer, 0);
        auto byteCount = count * sizeof(ValueType);
        _pEmitter->MemoryCopy(pSource, pDestination, Literal<int>(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* pDestinationPointer, llvm::Value* count)
    {
        auto pSource = PointerOffset(pSourcePointer, 0);
        auto pDestination = PointerOffset(pDestinationPointer, 0);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        _pEmitter->MemoryCopy(pSource, pDestination, byteCount);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(llvm::Value* pSourcePointer, int sourceOffset, llvm::Value* pDestinationPointer, int destinationOffset, int count)
    {
        auto pSource = PointerOffset(pSourcePointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pDestinationPointer, Literal(destinationOffset));
        int byteCount = count * sizeof(ValueType);
        _pEmitter->MemoryCopy(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* sourceOffset, llvm::Value* pDestinationPointer, llvm::Value* destinationOffset, llvm::Value* count)
    {
        auto pSource = PointerOffset(pSourcePointer, sourceOffset);
        auto pDestination = PointerOffset(pDestinationPointer, destinationOffset);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        _pEmitter->MemoryCopy(pSource, pDestination, byteCount);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(llvm::Value* pDestinationPointer, int destinationOffset, llvm::Value* value, int count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, Literal(destinationOffset));
        int byteCount = count * sizeof(ValueType);
        _pEmitter->MemorySet(pDestination, value, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(llvm::Value* pDestinationPointer, llvm::Value* pDestinationOffset, llvm::Value* value, int count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, pDestinationOffset);
        int byteCount = count * sizeof(ValueType);
        _pEmitter->MemorySet(pDestination, value, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(llvm::Value* pDestinationPointer, llvm::Value* pDestinationOffset, llvm::Value* value, llvm::Value* count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, pDestinationOffset);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        _pEmitter->MemorySet(pDestination, value, byteCount);
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

    template <typename ArgsListType>
    void IRFunctionEmitter::RegisterFunctionArgs(const ArgsListType& args)
    {
        auto argumentsIterator = Arguments().begin();
        for (size_t i = 0; i < args.size(); ++i)
        {
            auto arg = &(*argumentsIterator);
            _locals.Add(args[i].first, arg);
            ++argumentsIterator;
        }
    }
}
}
