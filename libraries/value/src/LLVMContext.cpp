////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMContext.h"
#include "Value.h"

// emitters
#include "IRModuleEmitter.h"

namespace ell
{
namespace value
{
    using namespace detail;
    using namespace emitters;
    using namespace utilities;

    namespace
    {

        std::pair<ValueType, int> LLVMTypeToVarType(llvm::Type* type)
        {
            switch (type->getTypeID())
            {
            case llvm::Type::TypeID::FloatTyID:
                return { ValueType::Float, 0 };
            case llvm::Type::TypeID::DoubleTyID:
                return { ValueType::Double, 0 };
            case llvm::Type::TypeID::IntegerTyID:
                switch (type->getIntegerBitWidth())
                {
                case 1:
                    return { ValueType::Boolean, 0 };
                case 8:
                    return { ValueType::Char8, 0 };
                case 16:
                    return { ValueType::Int16, 0 };
                case 32:
                    return { ValueType::Int32, 0 };
                case 64:
                    return { ValueType::Int64, 0 };
                default:
                    break;
                }
            case llvm::Type::TypeID::PointerTyID:
            {
                auto elementType = type->getPointerElementType(); //type->getSequentialElementType();

                auto underlyingType = LLVMTypeToVarType(elementType);
                underlyingType.second += 1;
                return underlyingType;
            }
            case llvm::Type::TypeID::ArrayTyID:
            {
                auto elementType = type->getArrayElementType(); //type->getSequentialElementType();

                auto underlyingType = LLVMTypeToVarType(elementType);
                underlyingType.second += 1;
                return underlyingType;
            }
            default:
                break;
            }
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        VariableType ValueTypeToVariableType(ValueType type)
        {
#define VALUE_TYPE_TO_VARIABLE_TYPE_MAPPING(x, y)                                                                      \
    case ValueType::x:                                                                                                 \
        return VariableType::y

#define VALUE_TYPE_TO_VARIABLE_TYPE_PTR(x)                                                                             \
    case ValueType::x:                                                                                                 \
        return VariableType::x##Pointer

            switch (type)
            {
                VALUE_TYPE_TO_VARIABLE_TYPE_MAPPING(Boolean, Boolean);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Byte);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Char8);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Int16);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Int32);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Int64);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Float);
                VALUE_TYPE_TO_VARIABLE_TYPE_PTR(Double);
                VALUE_TYPE_TO_VARIABLE_TYPE_MAPPING(Void, Void);
            default:
                throw LogicException(LogicExceptionErrors::illegalState);
            }

#undef VALUE_TYPE_TO_VARIABLE_TYPE_PTR

#undef VALUE_TYPE_TO_VARIABLE_TYPE_MAPPING
        }

        // TODO: Make this the basis of an iterator for MemoryLayout
        bool IncrementMemoryCoordinateImpl(int dimension, std::vector<int>& coordinate,
                                           const std::vector<int>& maxCoordinate)
        {
            // base case
            if (dimension < 0)
            {
                return false;
            }

            if (++coordinate[dimension] >= maxCoordinate[dimension])
            {
                coordinate[dimension] = 0;
                return IncrementMemoryCoordinateImpl(dimension - 1, coordinate, maxCoordinate);
            }

            return true;
        }

        bool IncrementMemoryCoordinate(std::vector<int>& coordinate, const std::vector<int>& maxCoordinate)
        {
            assert(coordinate.size() == maxCoordinate.size());
            return IncrementMemoryCoordinateImpl(static_cast<int>(maxCoordinate.size()) - 1, coordinate, maxCoordinate);
        }

    } // namespace

    struct LLVMContext::FunctionScope
    {
        template <typename... Args>
        FunctionScope(LLVMContext& context, Args&&... args) : context(context)
        {
            context._functionStack.push(context._emitter.BeginFunction(std::forward<Args>(args)...));
        }

        ~FunctionScope() { context._functionStack.pop(); }

        LLVMContext& context;
    };

    LLVMContext::LLVMContext(IRModuleEmitter& emitter) : _emitter(emitter), _computeContext(_emitter.GetModuleName()) {}

    Value LLVMContext::AllocateImpl(ValueType type, MemoryLayout layout)
    {
        return { Emittable{
                     _functionStack.top().get().Variable(ValueTypeToVariableType(type), layout.GetMemorySize()) },
                 layout };
    }

    std::pair<ValueType, int> LLVMContext::GetTypeImpl(Emittable emittable)
    {
        auto value = emittable.GetDataAs<LLVMValue>();
        auto type = value->getType();
        return LLVMTypeToVarType(type);
    }

    std::function<void()> LLVMContext::CreateFunctionImpl(std::string fnName, std::function<void()> fn)
    {
        {
            FunctionScope scope(*this, fnName, VariableType::Void);
            fn();
        }

        return [fnName, this] { (void)_emitter.GetCurrentFunction().Call(fnName); };
    }

    std::function<Value()> LLVMContext::CreateFunctionImpl(std::string fnName, Value returnValue,
                                                           std::function<Value()> fn)
    {
        {
            FunctionScope scope(*this, fnName, ValueTypeToVariableType(returnValue.GetBaseType()));
            returnValue = fn();

            _functionStack.top().get().Return(returnValue.Get<Emittable>().GetDataAs<LLVMValue>());
        }

        return [fnName, this] { return Emittable{ _emitter.GetCurrentFunction().Call(fnName) }; };
    }

    std::function<void(std::vector<Value>)> LLVMContext::CreateFunctionImpl(std::string fnName,
                                                                            std::vector<Value> argValues,
                                                                            std::function<void(std::vector<Value>)> fn)
    {
        std::vector<VariableType> variableArgTypes(argValues.size());
        std::transform(argValues.begin(), argValues.end(), variableArgTypes.begin(), [](Value value) {
            return ValueTypeToVariableType(value.GetBaseType());
        });

        {
            FunctionScope scope(*this, fnName, VariableType::Void, variableArgTypes);
            auto functionArgs = _functionStack.top().get().Arguments();

            for (std::pair idx{ 0u, functionArgs.begin() }; idx.first < argValues.size(); ++idx.first, ++idx.second)
            {
                argValues[idx.first].SetData(Emittable{ idx.second });
            }

            fn(argValues);
        }

        return [fnName, this, argValues = std::move(argValues)](std::vector<Value> args) {
            if (!std::equal(args.begin(),
                            args.end(),
                            argValues.begin(),
                            argValues.end(),
                            [](Value suppliedValue, Value fnValue) {
                                return suppliedValue.GetBaseType() == fnValue.GetBaseType();
                            }))
            {
                throw InputException(InputExceptionErrors::invalidArgument);
            }

            std::vector<LLVMValue> llvmArgs(args.size());
            std::transform(args.begin(), args.end(), llvmArgs.begin(), [](Value& arg) {
                return arg.Get<Emittable>().GetDataAs<LLVMValue>();
            });

            _emitter.GetCurrentFunction().Call(fnName, llvmArgs);
        };
    }

    std::function<Value(std::vector<Value>)> LLVMContext::CreateFunctionImpl(
        std::string fnName, Value returnValue, std::vector<Value> argValues,
        std::function<Value(std::vector<Value>)> fn)
    {
        std::vector<VariableType> variableArgTypes(argValues.size());
        std::transform(argValues.begin(), argValues.end(), variableArgTypes.begin(), [](Value value) {
            return ValueTypeToVariableType(value.GetBaseType());
        });

        {
            FunctionScope scope(*this, fnName, ValueTypeToVariableType(returnValue.GetBaseType()), variableArgTypes);
            auto functionArgs = _functionStack.top().get().Arguments();

            for (std::pair idx{ 0u, functionArgs.begin() }; idx.first < argValues.size(); ++idx.first, ++idx.second)
            {
                argValues[idx.first].SetData(Emittable{ idx.second });
            }

            returnValue = fn(argValues);
            _functionStack.top().get().Return(returnValue.Get<Emittable>().GetDataAs<LLVMValue>());
        }

        return [fnName, this, argValues = std::move(argValues)](std::vector<Value> args) {
            if (!std::equal(args.begin(),
                            args.end(),
                            argValues.begin(),
                            argValues.end(),
                            [](Value suppliedValue, Value fnValue) {
                                return suppliedValue.GetBaseType() == fnValue.GetBaseType();
                            }))
            {
                throw InputException(InputExceptionErrors::invalidArgument);
            }

            std::vector<LLVMValue> llvmArgs(args.size());
            std::transform(args.begin(), args.end(), llvmArgs.begin(), [](Value& arg) {
                return arg.Get<Emittable>().GetDataAs<LLVMValue>();
            });

            return Emittable{ _emitter.GetCurrentFunction().Call(fnName, llvmArgs) };
        };
    }

    Value LLVMContext::StoreConstantDataImpl(ConstantData data) { return _computeContext.StoreConstantData(data); }

    void LLVMContext::ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn)
    {
        auto maxCoordinate = layout.GetActiveSize().ToVector();
        decltype(maxCoordinate) coordinate(maxCoordinate.size());

        do
        {
            auto logicalCoordinates = layout.GetLogicalCoordinates(coordinate).ToVector();
            fn(std::vector<Scalar>(logicalCoordinates.begin(), logicalCoordinates.end()));
        } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
    }

    void LLVMContext::MoveDataImpl(Value& source, Value& destination)
    {
        // we treat a move the same as a copy, except we clear out the source
        CopyDataImpl(source, destination);

        // data has been "moved", so clear the source
        source.Reset();
    }

    void LLVMContext::CopyDataImpl(const Value& source, Value& destination)
    {
        if (destination.IsConstant())
        {
            if (source.IsConstant())
            {
                return _computeContext.CopyData(source, destination);
            }
            else
            {
                throw LogicException(LogicExceptionErrors::illegalState);
            }
        }
        else
        {
            //if (source.Type() != destination.Type() || source.PointerLevel() != destination.PointerLevel())
            //{
            //    throw InputException(InputExceptionErrors::typeMismatch);
            //}

            auto& fn = _functionStack.top().get();
            auto destValue = destination.Get<Emittable>().GetDataAs<LLVMValue>();
            if (source.IsConstant())
            {
                // we're only copying active areas below. should we copy padded too?
                auto& layout = source.GetLayout();
                std::visit(VariantVisitor{ [](Undefined) {},
                                           [](Emittable) {},
                                           [destValue, &fn, &layout](auto&& data) {
                                               auto maxCoordinate = layout.GetActiveSize().ToVector();
                                               decltype(maxCoordinate) coordinate(maxCoordinate.size());

                                               do
                                               {
                                                   fn.SetValueAt(destValue,
                                                                 fn.Literal(static_cast<int>(
                                                                     layout.GetEntryOffset(coordinate))),
                                                                 fn.Literal(
                                                                     *(data + layout.GetEntryOffset(coordinate))));
                                               } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                                           } },
                           source.GetUnderlyingData());
            }
            else
            {
                auto srcValue = source.Get<Emittable>().GetDataAs<LLVMValue>();
                if (auto& layout = source.GetLayout(); layout.IsContiguous())
                {
                    _emitter.GetIREmitter().MemoryCopy(srcValue,
                                                       destValue,
                                                       fn.Literal(static_cast<int64_t>(layout.GetMemorySize())));
                }
                else
                {
                    auto maxCoordinate = layout.GetActiveSize().ToVector();
                    decltype(maxCoordinate) coordinate(maxCoordinate.size());

                    do
                    {
                        fn.SetValueAt(destValue,
                                      fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate))),
                                      fn.ValueAt(srcValue,
                                                 fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate)))));
                    } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                }
            }
        }
    }

    Value LLVMContext::OffsetImpl(Value begin, Value index)
    {
        if (begin.IsConstant())
        {
            return _computeContext.Offset(begin, index);
        }
        else
        {
            auto& fn = _functionStack.top().get();

            return std::visit(VariantVisitor{ [](Undefined) -> Value {
                                                 throw LogicException(LogicExceptionErrors::illegalState);
                                             },
                                              [](std::variant<Boolean*, float*, double*>) -> Value {
                                                  throw LogicException(LogicExceptionErrors::illegalState);
                                              },
                                              [&begin, &fn](auto&& data) -> Value {
                                                  using Type = std::decay_t<decltype(data)>;
                                                  auto llvmValue = begin.Get<Emittable>().GetDataAs<LLVMValue>();
                                                  LLVMValue offset = nullptr;

                                                  if constexpr (std::is_same_v<Emittable, Type>)
                                                  {
                                                      offset = data.template GetDataAs<LLVMValue>();
                                                  }
                                                  else
                                                  {
                                                      offset = fn.Literal(*data);
                                                  }
                                                  return Emittable{ fn.PointerOffset(llvmValue, offset) };
                                              } },
                              index.GetUnderlyingData());
        }
    }

    Value LLVMContext::UnaryOperationImpl(ValueUnaryOperation op, Value destination)
    {
        throw LogicException(LogicExceptionErrors::notImplemented);
    }

    Value LLVMContext::BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source)
    {
        if (!source.IsDefined())
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        if (destination.IsDefined())
        {
            if (source.IsConstant() && destination.IsConstant())
            {
                return _computeContext.BinaryOperation(op, destination, source);
            }
        }
        else
        {
            destination = Allocate(source.GetBaseType(), source.GetLayout());
        }

        /*if (!TypeCompatible(destination, source))
    {
        throw InputException(InputExceptionErrors::typeMismatch);
    }*/

        if (destination.GetLayout() != source.GetLayout())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }

        auto& fn = _functionStack.top().get();
        std::visit(
            [&destination, op, &fn](auto&& sourceData) {
                using SourceDataType = std::decay_t<decltype(sourceData)>;
                if constexpr (std::is_same_v<SourceDataType, Undefined>)
                {
                }
                else if constexpr (std::is_same_v<Boolean*, SourceDataType>)
                {
                    throw LogicException(LogicExceptionErrors::notImplemented);
                }
                else
                {
                    auto isFp = destination.IsFloatingPoint();
                    std::function<LLVMValue(LLVMValue, LLVMValue)> opFn;
                    switch (op)
                    {
                    case ValueBinaryOperation::add:
                        opFn = [&fn, isFp](auto dst, auto src) {
                            return fn.Operator(isFp ? TypedOperator::addFloat : TypedOperator::add, dst, src);
                        };
                        break;
                    case ValueBinaryOperation::subtract:
                        opFn = [&fn, isFp](auto dst, auto src) {
                            return fn.Operator(isFp ? TypedOperator::subtractFloat : TypedOperator::subtract, dst, src);
                        };
                        break;
                    case ValueBinaryOperation::multiply:
                        opFn = [&fn, isFp](auto dst, auto src) {
                            return fn.Operator(isFp ? TypedOperator::multiplyFloat : TypedOperator::multiply, dst, src);
                        };
                        break;
                    case ValueBinaryOperation::divide:
                        opFn = [&fn, isFp](auto dst, auto src) {
                            return fn.Operator(isFp ? TypedOperator::divideFloat : TypedOperator::divideSigned,
                                               dst,
                                               src);
                        };
                        break;
                    }

                    auto& layout = destination.GetLayout();
                    auto maxCoordinate = layout.GetActiveSize().ToVector();
                    decltype(maxCoordinate) coordinate(maxCoordinate.size());

                    auto destValue = destination.Get<Emittable>().GetDataAs<LLVMValue>();

                    std::conditional_t<std::is_same_v<Emittable, SourceDataType>, LLVMValue, SourceDataType> srcValue =
                        nullptr;
                    if constexpr (std::is_same_v<Emittable, SourceDataType>)
                    {
                        srcValue = sourceData.template GetDataAs<LLVMValue>();
                    }
                    else
                    {
                        srcValue = sourceData;
                    }

                    do
                    {
                        LLVMValue opResult = nullptr;
                        if constexpr (std::is_same_v<Emittable, SourceDataType>)
                        {
                            opResult =
                                opFn(fn.ValueAt(destValue,
                                                fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate)))),
                                     fn.ValueAt(srcValue,
                                                fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate)))));
                        }
                        else
                        {
                            opResult = opFn(fn.ValueAt(destValue,
                                                       fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate)))),
                                            fn.Literal(*(srcValue + layout.GetEntryOffset(coordinate))));
                        }

                        fn.SetValueAt(destValue,
                                      fn.Literal(static_cast<int>(layout.GetEntryOffset(coordinate))),
                                      opResult);
                    } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                }
            },
            source.GetUnderlyingData());

        return destination;
    }

    bool LLVMContext::TypeCompatible(Value value1, Value value2)
    {
        return value1.GetBaseType() == value2.GetBaseType() && value1.PointerLevel() == value2.PointerLevel();
    }

} // namespace value
} // namespace ell
