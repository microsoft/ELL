////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMContext.h"
#include "Value.h"
#include "ValueScalar.h"

#include <emitters/include/IRModuleEmitter.h>

#include <utilities/include/StringUtil.h>

using namespace std::string_literals;

namespace ell
{
namespace value
{
    using namespace detail;
    using namespace emitters;
    using namespace utilities;

    namespace
    {

        std::pair<ValueType, int> LLVMTypeToVarType(LLVMType type)
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
                auto elementType = type->getPointerElementType();

                auto underlyingType = LLVMTypeToVarType(elementType);
                underlyingType.second += 1;
                return underlyingType;
            }
            case llvm::Type::TypeID::ArrayTyID:
            {
                auto elementType = type->getArrayElementType();

                auto underlyingType = LLVMTypeToVarType(elementType);
                underlyingType.second += 1;
                return underlyingType;
            }
            default:
                break;
            }
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        LLVMType ValueTypeToLLVMType(IREmitter& emitter, std::pair<ValueType, int> typeDescription)
        {
            auto& builder = emitter.GetIRBuilder();
            LLVMType type = nullptr;
            switch (typeDescription.first)
            {
            case ValueType::Boolean:
                if (typeDescription.second == 0)
                {
                    type = builder.getInt1Ty();
                }
                else
                {
                    type = builder.getInt8Ty();
                }
                break;
            case ValueType::Byte:
                [[fallthrough]];
            case ValueType::Char8:
                type = builder.getInt8Ty();
                break;
            case ValueType::Int16:
                type = builder.getInt16Ty();
                break;
            case ValueType::Int32:
                type = builder.getInt32Ty();
                break;
            case ValueType::Int64:
                type = builder.getInt64Ty();
                break;
            case ValueType::Float:
                type = builder.getFloatTy();
                break;
            case ValueType::Double:
                type = builder.getDoubleTy();
                break;
            case ValueType::Void:
                type = builder.getVoidTy();
                break;
            case ValueType::Undefined:
                [[fallthrough]];
            default:
                throw LogicException(LogicExceptionErrors::illegalState);
            }

            for (int ptrLevel = 0; ptrLevel < typeDescription.second; ++ptrLevel)
            {
                type = type->getPointerTo();
            }

            return type;
        }

        VariableType ValueTypeToVariableType(ValueType type)
        {
            // clang-format off

#define VALUE_TYPE_TO_VARIABLE_TYPE_MAPPING(x, y)  \
    case ValueType::x:                             \
        return VariableType::y

#define VALUE_TYPE_TO_VARIABLE_TYPE_PTR(x)         \
    case ValueType::x:                             \
        return VariableType::x##Pointer

            // clang-format on

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
        bool IncrementMemoryCoordinateImpl(int dimension, std::vector<int>& coordinate, const std::vector<int>& maxCoordinate)
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
        FunctionScope(LLVMContext& context, Args&&... args) :
            context(context)
        {
            context._functionStack.push(context._emitter.BeginFunction(std::forward<Args>(args)...));
            context._promotedConstantStack.push({});
        }

        ~FunctionScope()
        {
            context._functionStack.pop();
            context._promotedConstantStack.pop();
        }

        LLVMContext& context;
    };

    LLVMContext::LLVMContext(IRModuleEmitter& emitter) :
        _emitter(emitter),
        _computeContext(_emitter.GetModuleName())
    {
        _promotedConstantStack.push({});
    }

    Value LLVMContext::AllocateImpl(ValueType type, MemoryLayout layout)
    {
        auto llvmType = ValueTypeToLLVMType(GetFnEmitter().GetEmitter(), { type, 0 });
        return { Emittable{ GetFnEmitter().Variable(llvmType, layout.GetMemorySize()) }, layout };
    }

    std::optional<Value> LLVMContext::GetGlobalValue(GlobalAllocationScope scope, std::string name)
    {
        std::string adjustedName = GetScopeAdjustedName(scope, name);
        if (auto it = _globals.find(adjustedName); it != _globals.end())
        {
            return Value(it->second.first, it->second.second);
        }

        return std::nullopt;
    }

    Value LLVMContext::GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout)
    {
        std::string adjustedName = GetScopeAdjustedName(scope, name);

        if (_globals.find(adjustedName) != _globals.end())
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 "Unexpected collision in global data allocation");
        }

        Emittable emittable = std::visit(
            [this, &adjustedName](auto&& vectorData) {
                using Type = std::decay_t<decltype(vectorData)>;

                if constexpr (std::is_same_v<Type, std::vector<utilities::Boolean>>)
                {
                    // IREmitter stores a vector of bool values as a bitvector, which
                    // breaks the memory model we need for our purposes.
                    // NB: This somewhat screws up our type system because we rely
                    // on LLVM to tell us the type, but here we set a different type
                    // altogether, with no discernable way of retrieving the fact that
                    // originally, this was a vector of bools. This will be rectified
                    // in the near future. (2018-11-08)
                    std::vector<char> transformedData(vectorData.begin(), vectorData.end());
                    return _emitter.GlobalArray(adjustedName, transformedData);
                }
                else
                {
                    return _emitter.GlobalArray(adjustedName, vectorData);
                }
            },
            data);
        _globals[adjustedName] = { emittable, layout };

        return Value(emittable, layout);
    }

    Value LLVMContext::GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout)
    {
        std::string adjustedName = GetScopeAdjustedName(scope, name);

        if (_globals.find(adjustedName) != _globals.end())
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 FormatString("Global variable %s is already defined", adjustedName.c_str()));
        }

        Emittable emittable = _emitter.GlobalArray(adjustedName,
                                                   ValueTypeToLLVMType(_emitter.GetIREmitter(), { type, 0 }),
                                                   layout.GetMemorySize());

        _globals[adjustedName] = { emittable, layout };

        return Value(emittable, layout);
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

    std::function<Value()> LLVMContext::CreateFunctionImpl(std::string fnName, Value returnValue, std::function<Value()> fn)
    {
        {
            FunctionScope scope(*this, fnName, ValueTypeToVariableType(returnValue.GetBaseType()));
            auto returnValueCopy = returnValue;
            returnValueCopy = fn();

            GetFnEmitter().Return(EnsureEmittable(returnValueCopy).Get<Emittable>().GetDataAs<LLVMValue>());
        }

        return [returnValue, fnName, this] {
            auto returnValueCopy = returnValue;
            returnValueCopy.SetData(Emittable{ _emitter.GetCurrentFunction().Call(fnName) });
            return returnValueCopy;
        };
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
            auto functionArgs = GetFnEmitter().Arguments();
            auto argValuesCopy = argValues;

            for (std::pair idx{ 0u, functionArgs.begin() }; idx.first < argValuesCopy.size(); ++idx.first, ++idx.second)
            {
                argValuesCopy[idx.first].SetData(Emittable{ idx.second });
            }

            fn(argValuesCopy);
        }

        return [fnName, this, argValues](std::vector<Value> args) {
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
            std::transform(args.begin(), args.end(), llvmArgs.begin(), [this](Value& arg) {
                return EnsureEmittable(arg).Get<Emittable>().GetDataAs<LLVMValue>();
            });

            _emitter.GetCurrentFunction().Call(fnName, llvmArgs);
        };
    }

    std::function<Value(std::vector<Value>)> LLVMContext::CreateFunctionImpl(
        std::string fnName,
        Value returnValue,
        std::vector<Value> argValues,
        std::function<Value(std::vector<Value>)> fn)
    {
        std::vector<VariableType> variableArgTypes(argValues.size());
        std::transform(argValues.begin(), argValues.end(), variableArgTypes.begin(), [](Value value) {
            return ValueTypeToVariableType(value.GetBaseType());
        });

        {
            FunctionScope scope(*this, fnName, ValueTypeToVariableType(returnValue.GetBaseType()), variableArgTypes);
            auto functionArgs = GetFnEmitter().Arguments();
            auto argValuesCopy = argValues;
            auto returnValueCopy = returnValue;

            for (std::pair idx{ 0u, functionArgs.begin() }; idx.first < argValuesCopy.size(); ++idx.first, ++idx.second)
            {
                argValuesCopy[idx.first].SetData(Emittable{ idx.second });
            }

            returnValueCopy = fn(argValuesCopy);
            GetFnEmitter().Return(EnsureEmittable(returnValueCopy).Get<Emittable>().GetDataAs<LLVMValue>());
        }

        return [fnName, this, argValues, returnValue](std::vector<Value> args) {
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
            std::transform(args.begin(), args.end(), llvmArgs.begin(), [this](Value& arg) {
                return EnsureEmittable(arg).Get<Emittable>().GetDataAs<LLVMValue>();
            });

            auto returnValueCopy = returnValue;
            returnValueCopy.SetData(Emittable{ _emitter.GetCurrentFunction().Call(fnName, llvmArgs) });
            return returnValueCopy;
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
            // if (!TypeCompatible(destination, source))
            // {
            //     throw InputException(InputExceptionErrors::typeMismatch);
            // }

            auto& irEmitter = _emitter.GetIREmitter();
            auto destValue = destination.Get<Emittable>().GetDataAs<LLVMValue>();
            if (source.IsConstant())
            {
                // we're only copying active areas below. should we copy padded too?
                auto& layout = source.GetLayout();
                std::visit(VariantVisitor{ [](Undefined) {},
                                           [](Emittable) {},
                                           [destValue, &irEmitter, &layout](auto&& data) {
                                               auto maxCoordinate = layout.GetActiveSize().ToVector();
                                               decltype(maxCoordinate) coordinate(maxCoordinate.size());

                                               do
                                               {
                                                   auto srcAtOffset =
                                                       irEmitter.Literal(*(data + layout.GetEntryOffset(coordinate)));
                                                   auto destOffset = irEmitter.Literal(
                                                       static_cast<int>(layout.GetEntryOffset(coordinate)));
                                                   auto destAtOffset = irEmitter.PointerOffset(destValue, destOffset);
                                                   (void)irEmitter.Store(destAtOffset, srcAtOffset);
                                               } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                                           } },
                           source.GetUnderlyingData());
            }
            else
            {
                auto srcValue = source.Get<Emittable>().GetDataAs<LLVMValue>();
                if (auto& layout = source.GetLayout(); layout.IsContiguous())
                {
                    irEmitter.MemoryCopy(srcValue,
                                         destValue,
                                         irEmitter.Literal(static_cast<int64_t>(layout.GetMemorySize())));
                }
                else
                {
                    auto maxCoordinate = layout.GetActiveSize().ToVector();
                    decltype(maxCoordinate) coordinate(maxCoordinate.size());

                    do
                    {
                        auto offset = irEmitter.Literal(static_cast<int>(layout.GetEntryOffset(coordinate)));
                        auto srcAtOffset = irEmitter.PointerOffset(srcValue, offset);
                        auto destAtOffset = irEmitter.PointerOffset(destValue, offset);

                        (void)irEmitter.Store(destAtOffset, irEmitter.Load(srcAtOffset));

                    } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                }
            }
        }
    }

    Value LLVMContext::OffsetImpl(Value begin, Value index)
    {
        if (begin.IsConstant() && index.IsConstant())
        {
            return _computeContext.Offset(begin, index);
        }
        else
        {
            auto& fn = GetFnEmitter();
            Value emittableBegin = EnsureEmittable(begin);
            Value emittableIndex = EnsureEmittable(index);

            auto llvmBegin = std::get<Emittable>(emittableBegin.GetUnderlyingData()).GetDataAs<LLVMValue>();
            auto llvmIndex = std::get<Emittable>(emittableIndex.GetUnderlyingData()).GetDataAs<LLVMValue>();

            return Emittable{ fn.PointerOffset(llvmBegin, fn.ValueAt(llvmIndex, 0)) };
            // return std::visit(VariantVisitor{ [](Undefined) -> Value {
            //                                      throw LogicException(LogicExceptionErrors::illegalState);
            //                                  },
            //                                   [](std::variant<Boolean*, float*, double*>) -> Value {
            //                                       throw LogicException(LogicExceptionErrors::illegalState);
            //                                   },
            //                                   [&begin, &fn](auto&& data) -> Value {
            //                                       using Type = std::decay_t<decltype(data)>;
            //                                       auto llvmValue = begin.Get<Emittable>().GetDataAs<LLVMValue>();
            //                                       LLVMValue offset = nullptr;

            //                                       if constexpr (std::is_same_v<Emittable, Type>)
            //                                       {
            //                                           offset = fn.ValueAt(data.template GetDataAs<LLVMValue>());
            //                                       }
            //                                       else
            //                                       {
            //                                           offset = fn.Literal(*data);
            //                                       }
            //                                       return Emittable{ fn.PointerOffset(llvmValue, offset) };
            //                                   } },
            //                   index.GetUnderlyingData());
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

        // if (!TypeCompatible(destination, source))
        // {
        //     throw InputException(InputExceptionErrors::typeMismatch);
        // }

        if (destination.GetLayout() != source.GetLayout())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }

        auto& fn = GetFnEmitter();
        std::visit(
            [destination = EnsureEmittable(destination), op, &fn](auto&& sourceData) {
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
                    case ValueBinaryOperation::modulus:
                        if (isFp)
                        {
                            throw InputException(InputExceptionErrors::invalidArgument);
                        }
                        opFn = [&fn](auto dst, auto src) { return fn.Operator(TypedOperator::moduloSigned, dst, src); };
                        break;
                    default:
                        throw LogicException(LogicExceptionErrors::illegalState);
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

    Value LLVMContext::LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2)
    {
        if (source1.GetLayout() != source2.GetLayout())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }

        if (source1.IsConstant() && source2.IsConstant())
        {
            return _computeContext.LogicalOperation(op, source1, source2);
        }

        auto comparisonOp = TypedComparison::none;
        bool isFp = source1.IsFloatingPoint() || source1.IsFloatingPointPointer();
        switch (op)
        {
        case ValueLogicalOperation::equality:
            comparisonOp = isFp ? TypedComparison::equalsFloat : TypedComparison::equals;
            break;
        case ValueLogicalOperation::inequality:
            comparisonOp = isFp ? TypedComparison::notEqualsFloat : TypedComparison::notEquals;
            break;
        case ValueLogicalOperation::greaterthan:
            comparisonOp = isFp ? TypedComparison::greaterThanFloat : TypedComparison::greaterThan;
            break;
        case ValueLogicalOperation::greaterthanorequal:
            comparisonOp = isFp ? TypedComparison::greaterThanOrEqualsFloat : TypedComparison::greaterThanOrEquals;
            break;
        case ValueLogicalOperation::lessthan:
            comparisonOp = isFp ? TypedComparison::lessThanFloat : TypedComparison::lessThan;
            break;
        case ValueLogicalOperation::lessthanorequal:
            comparisonOp = isFp ? TypedComparison::lessThanOrEqualsFloat : TypedComparison::lessThanOrEquals;
            break;
        }

        Value returnValue = std::visit(
            VariantVisitor{
                [](Undefined) -> Value { throw LogicException(LogicExceptionErrors::illegalState); },
                [this,
                 comparisonOp,
                 &source2UnderlyingData = source2.GetUnderlyingData(),
                 &source1Layout = source1.GetLayout(),
                 &source2Layout = source2.GetLayout()](Emittable source1Data) -> Value {
                    // source1 is an Emittable type, so source2 can be constant or Emittable
                    return std::
                        visit(VariantVisitor{ [](Undefined) -> Value {
                                                 throw LogicException(LogicExceptionErrors::illegalState);
                                             },
                                              [&, this](Emittable source2Data) -> Value {
                                                  auto maxCoordinate = source1Layout.GetActiveSize().ToVector();
                                                  decltype(maxCoordinate) coordinate(maxCoordinate.size());
                                                  auto& fn = this->GetFnEmitter();
                                                  auto result = fn.TrueBit();
                                                  auto llvmOp1 = source1Data.GetDataAs<LLVMValue>();
                                                  auto llvmOp2 = source2Data.GetDataAs<LLVMValue>();
                                                  do
                                                  {
                                                      auto logicalCoordinates =
                                                          source1Layout.GetLogicalCoordinates(coordinate);
                                                      auto source1Offset =
                                                          source1Layout.GetLogicalEntryOffset(logicalCoordinates);
                                                      auto source2Offset =
                                                          source2Layout.GetLogicalEntryOffset(logicalCoordinates);

                                                      result = fn.LogicalAnd(result,
                                                                             fn.Comparison(comparisonOp,
                                                                                           fn.ValueAt(llvmOp1,
                                                                                                      source1Offset),
                                                                                           fn.ValueAt(llvmOp2,
                                                                                                      source2Offset)));

                                                  } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));

                                                  return { Emittable{ result }, ScalarLayout };
                                              },
                                              [&, this](auto&& source2Data) -> Value {
                                                  using Type =
                                                      std::remove_pointer_t<std::decay_t<decltype(source2Data)>>;
                                                  using CastType =
                                                      std::conditional_t<std::is_same_v<Type, Boolean>, bool, Type>;
                                                  auto& fn = this->GetFnEmitter();

                                                  auto result = fn.TrueBit();
                                                  auto llvmOp1 = source1Data.GetDataAs<LLVMValue>();
                                                  auto maxCoordinate = source1Layout.GetActiveSize().ToVector();
                                                  decltype(maxCoordinate) coordinate(maxCoordinate.size());
                                                  do
                                                  {
                                                      auto logicalCoordinates =
                                                          source1Layout.GetLogicalCoordinates(coordinate);
                                                      auto source1Offset =
                                                          source1Layout.GetLogicalEntryOffset(logicalCoordinates);
                                                      auto source2Offset =
                                                          source2Layout.GetLogicalEntryOffset(logicalCoordinates);

                                                      result =
                                                          fn.LogicalAnd(result,
                                                                        fn.Comparison(comparisonOp,
                                                                                      fn.ValueAt(llvmOp1,
                                                                                                 source1Offset),
                                                                                      fn.Literal(static_cast<CastType>(
                                                                                          source2Data
                                                                                              [source2Offset]))));

                                                  } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));

                                                  return { Emittable{ result }, ScalarLayout };
                                              } },
                              source2UnderlyingData);
                },
                [this,
                 comparisonOp,
                 &source2Data = source2.GetUnderlyingData(),
                 &source1Layout = source1.GetLayout(),
                 &source2Layout = source2.GetLayout()](auto&& source1Data) -> Value {
                    // source1 is constant, so source2 has to be an Emittable type
                    using Type = std::remove_pointer_t<std::decay_t<decltype(source1Data)>>;
                    using CastType = std::conditional_t<std::is_same_v<Type, Boolean>, bool, Type>;

                    auto& fn = this->GetFnEmitter();
                    auto result = fn.TrueBit();
                    auto llvmOp2 = std::get<Emittable>(source2Data).GetDataAs<LLVMValue>();

                    auto maxCoordinate = source1Layout.GetActiveSize().ToVector();
                    decltype(maxCoordinate) coordinate(maxCoordinate.size());
                    do
                    {
                        auto logicalCoordinates = source1Layout.GetLogicalCoordinates(coordinate);
                        auto source1Offset = source1Layout.GetLogicalEntryOffset(logicalCoordinates);
                        auto source2Offset = source2Layout.GetLogicalEntryOffset(logicalCoordinates);

                        result =
                            fn.LogicalAnd(result,
                                          fn.Comparison(comparisonOp,
                                                        fn.Literal(static_cast<CastType>(source1Data[source1Offset])),
                                                        fn.ValueAt(llvmOp2, source2Offset)));

                    } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));

                    return { Emittable{ result }, ScalarLayout };
                } },
            source1.GetUnderlyingData());

        return returnValue;
    }

    Value LLVMContext::CastImpl(Value value, ValueType type)
    {
        if (value.IsConstant())
        {
            return _computeContext.Cast(value, type);
        }

        auto data = value.Get<Emittable>().GetDataAs<LLVMValue>();
        auto& fn = GetFnEmitter();

        auto casted = fn.CastPointer(data, ValueTypeToVariableType(type));

        return { casted,
                 value.IsConstrained() ? std::optional<MemoryLayout>(value.GetLayout())
                                       : std::optional<MemoryLayout>(std::nullopt) };
    }

    class LLVMContext::IfContextImpl : public EmitterContext::IfContextImpl
    {
    public:
        IfContextImpl(IRIfEmitter ifEmitter, IRFunctionEmitter& fnEmitter) :
            _ifEmitter(std::move(ifEmitter)),
            _fnEmitter(fnEmitter)
        {}

        void ElseIf(Scalar test, std::function<void()> fn) override
        {
            LLVMValue testValue = nullptr;
            if (auto value = test.GetValue(); value.IsConstant())
            {
                testValue = _fnEmitter.Literal(static_cast<bool>(test.Get<Boolean>()));
            }
            else
            {
                testValue = value.Get<Emittable>().GetDataAs<LLVMValue>();
            }

            _ifEmitter.ElseIf(testValue, [fn = std::move(fn)](auto&) { fn(); });
        }

        void Else(std::function<void()> fn) override
        {
            _ifEmitter.Else([fn = std::move(fn)](auto&) { fn(); });
        }

    private:
        IRIfEmitter _ifEmitter;
        IRFunctionEmitter& _fnEmitter;
    };

    EmitterContext::IfContext LLVMContext::IfImpl(Scalar test, std::function<void()> fn)
    {
        auto& fnEmitter = GetFnEmitter();
        LLVMValue testValue = nullptr;
        if (auto value = test.GetValue(); value.IsConstant())
        {
            testValue = fnEmitter.Literal(static_cast<bool>(test.Get<Boolean>()));
        }
        else
        {
            testValue = value.Get<Emittable>().GetDataAs<LLVMValue>();
        }

        auto ifEmitter = fnEmitter.If(testValue, [fn = std::move(fn)](auto&) { fn(); });

        return { std::make_unique<LLVMContext::IfContextImpl>(std::move(ifEmitter), fnEmitter) };
    }

    Value LLVMContext::CallImpl(std::string fnName, Value retValue, std::vector<Value> args)
    {
        throw LogicException(LogicExceptionErrors::notImplemented);
    }

    bool LLVMContext::TypeCompatible(Value value1, Value value2)
    {
        return value1.GetBaseType() == value2.GetBaseType() && value1.PointerLevel() == value2.PointerLevel();
    }

    std::string LLVMContext::GetScopeAdjustedName(GlobalAllocationScope scope, std::string name) const
    {
        switch (scope)
        {
        case GlobalAllocationScope::Global:
            return GetGlobalScopedName(name);
        case GlobalAllocationScope::Function:
            return GetCurrentFunctionScopedName(name);
        }

        throw LogicException(LogicExceptionErrors::illegalState);
    }

    std::string LLVMContext::GetGlobalScopedName(std::string name) const
    {
        return _emitter.GetModuleName() + "_" + name;
    }

    std::string LLVMContext::GetCurrentFunctionScopedName(std::string name) const
    {
        if (_functionStack.empty())
        {
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        return GetGlobalScopedName(GetFnEmitter().GetFunctionName() + "_" + name);
    }

    IRFunctionEmitter& LLVMContext::GetFnEmitter() const { return _functionStack.top().get(); }

    Value LLVMContext::PromoteConstantData(Value value)
    {
        assert(value.IsConstant() && value.IsDefined() && !value.IsEmpty());

        const auto& constantData = _computeContext.GetConstantData(value);

        ptrdiff_t offset = 0;
        LLVMValue llvmValue = std::visit(
            [this, &value, &offset](auto&& data) -> LLVMValue {
                using Type = std::decay_t<decltype(data)>;
                using DataType = typename Type::value_type;

                auto ptrData = std::get<DataType*>(value.GetUnderlyingData());
                offset = ptrData - data.data();

                if (_functionStack.empty())
                {
                    std::string globalName =
                        GetGlobalScopedName("_"s + std::to_string(_promotedConstantStack.top().size()));

                    llvm::GlobalVariable* globalVariable = nullptr;
                    if constexpr (std::is_same_v<DataType, Boolean>)
                    {
                        globalVariable =
                            _emitter.GlobalArray(globalName, std::vector<uint8_t>(data.begin(), data.end()));
                    }
                    else
                    {
                        globalVariable = _emitter.GlobalArray(globalName, data);
                    }

                    return globalVariable;
                }
                else
                {
                    auto& fn = GetFnEmitter();
                    llvm::Constant* constant = nullptr;
                    if constexpr (std::is_same_v<DataType, Boolean>)
                    {
                        constant = fn.GetEmitter().Literal(std::vector<uint8_t>(data.begin(), data.end()));
                    }
                    else
                    {
                        constant = fn.GetEmitter().Literal(data);
                    }

                    auto varType =
                        GetVariableType<std::conditional_t<std::is_same_v<DataType, Boolean>, bool, DataType>>();

                    LLVMValue newValue = fn.Variable(varType, data.size());
                    fn.MemoryCopy<DataType>(constant, newValue, static_cast<int>(data.size()));

                    return newValue;
                }
            },
            constantData);

        _promotedConstantStack.top().push_back({ &constantData, llvmValue });

        auto& irEmitter = _emitter.GetIREmitter();
        auto llvmOffset = irEmitter.Literal(static_cast<int>(offset));
        if (auto globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(llvmValue))
        {
            llvmValue = irEmitter.PointerOffset(globalVariable, llvmOffset);
        }
        else
        {
            llvmValue = irEmitter.PointerOffset(llvmValue, llvmOffset);
        }

        Value newValue = value;
        newValue.SetData(Emittable{ llvmValue });

        return newValue;
    }

    std::optional<LLVMContext::PromotedConstantDataDescription> LLVMContext::HasBeenPromoted(Value value)
    {
        if (!value.IsDefined() || value.IsEmpty() || !value.IsConstant())
        {
            return std::nullopt;
        }

        const auto& constantData = _computeContext.GetConstantData(value);
        const auto& promotedStack = _promotedConstantStack.top();

        if (auto it = std::find_if(promotedStack.begin(),
                                   promotedStack.end(),
                                   [&constantData](const auto& desc) { return desc.data == &constantData; });
            it != promotedStack.end())
        {
            return *it;
        }
        else
        {
            return std::nullopt;
        }
    }

    Value LLVMContext::Realize(Value value)
    {
        if (auto desc = HasBeenPromoted(value); !desc)
        {
            return value;
        }
        else
        {
            const auto& promotionalDesc = *desc;
            auto offset = std::visit(
                [&value](auto&& data) -> ptrdiff_t {
                    using Type = std::decay_t<decltype(data)>;
                    using DataType = typename Type::value_type;

                    auto ptrData = std::get<DataType*>(value.GetUnderlyingData());

                    return ptrData - data.data();
                },
                *promotionalDesc.data);

            auto& fn = GetFnEmitter();
            auto emittable = promotionalDesc.realValue;

            Value newValue = value;
            newValue.SetData(Emittable{ fn.PointerOffset(emittable.GetDataAs<LLVMValue>(), static_cast<int>(offset)) });

            return newValue;
        }
    }

    Value LLVMContext::EnsureEmittable(Value value)
    {
        if (!value.IsConstant())
        {
            return value;
        }
        else if (Value newValue = Realize(value); !newValue.IsConstant())
        {
            return newValue;
        }
        else
        {
            return PromoteConstantData(newValue);
        }
    }

} // namespace value
} // namespace ell
