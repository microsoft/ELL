////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ComputeContext.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ComputeContext.h"
#include "Value.h"

// utilities
#include "TypeTraits.h"

// stl
#include <string>
#include <cassert>

namespace ell
{
namespace value
{

    using namespace detail;
    using namespace utilities;

    struct ComputeContext::FunctionScope
    {
        FunctionScope(ComputeContext& context) : context(context) { context._stack.push({}); }
        ~FunctionScope() { context._stack.pop(); }

        ComputeContext& context;
    };

    namespace
    {

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

        Value ConstantDataToValue(ConstantData& data, std::optional<MemoryLayout> layout = {})
        {
            return std::visit(
                [&layout](auto&& vector) -> Value {
                    return Value(vector.data(), layout.value_or(MemoryLayout({ (int)vector.size() })));
                },
                data);
        }

    } // namespace

    ComputeContext::ComputeContext(std::string moduleName) : _moduleName(std::move(moduleName))
    {
        // we always have at least one stack space, in case the top level function needs to return something
        _stack.push({});
    }

    Value ComputeContext::AllocateImpl(ValueType type, MemoryLayout layout)
    {
        // special case the scalar case
        auto size = layout.NumDimensions() == 0 ? 1u : layout.GetMemorySize();

        auto createValue = [this, size, &layout](auto type) -> Value {
            using VectorType = std::vector<typename decltype(type)::Type>;
            VectorType v(size);
            Value value = StoreConstantData(std::move(v));
            value.SetLayout(layout);
            return value;
        };

        switch (type)
        {
        case ValueType::Boolean:
            return createValue(IdentityType<Boolean>{});
        case ValueType::Char8:
            return createValue(IdentityType<char>{});
        case ValueType::Byte:
            return createValue(IdentityType<uint8_t>{});
        case ValueType::Int16:
            return createValue(IdentityType<int16_t>{});
        case ValueType::Int32:
            return createValue(IdentityType<int32_t>{});
        case ValueType::Int64:
            return createValue(IdentityType<int64_t>{});
        case ValueType::Float:
            return createValue(IdentityType<float>{});
        case ValueType::Double:
            return createValue(IdentityType<double>{});
        default:
            throw LogicException(LogicExceptionErrors::notImplemented);
        }
    }

    Value ComputeContext::StoreConstantDataImpl(ConstantData data)
    {
        Value value = ConstantDataToValue(data);

        _stack.top().push_front(std::move(data));
        return value;
    }

    Value ComputeContext::OffsetImpl(Value begin, Value index)
    {
        if (!ValidateValue(begin))
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        if (!ValidateValue(index))
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        return std::visit(
            [&begin](auto&& index) -> Value {
                using IndexType = std::remove_pointer_t<std::decay_t<decltype(index)>>;
                if constexpr (std::is_arithmetic_v<IndexType> && !std::is_floating_point_v<IndexType>)
                {
                    auto indexValue = *index;
                    return std::visit(
                        [indexValue](auto&& begin) -> Value {
                            using BeginType = std::decay_t<decltype(begin)>;
                            if constexpr (std::is_same_v<BeginType, Emittable> || std::is_same_v<BeginType, Undefined>)
                            {
                                throw InputException(InputExceptionErrors::invalidArgument);
                            }
                            else
                            {
                                return Value(begin + indexValue);
                            }
                        },
                        begin.GetUnderlyingData());
                }
                else
                {
                    throw InputException(InputExceptionErrors::invalidArgument);
                }
            },
            index.GetUnderlyingData());
    }

    ComputeContext::ConstantData ComputeContext::ExtractConstantData(Value value)
    {
        ConstantData movedOutOfScope;
        const auto& scope = _stack.top();

        std::visit(VariantVisitor{ [](Undefined) {},
                                   [](Emittable) {},
                                   [&movedOutOfScope, &scope](auto&& data) {
                                       using Type = std::decay_t<decltype(data)>;
                                       using RealType = std::remove_pointer_t<Type>;
                                       using VectorType = std::vector<RealType>;

                                       for (auto& constData : scope)
                                       {
                                           auto ptr = std::get_if<VectorType>(&constData);
                                           if (ptr != nullptr)
                                           {
                                               if (ptr->data() == data)
                                               {
                                                   movedOutOfScope = std::move(constData);
                                               }
                                           }
                                       }
                                   } },
                   value.GetUnderlyingData());

        return movedOutOfScope;
    }

    std::pair<ValueType, int> ComputeContext::GetTypeImpl(Emittable)
    {
        throw LogicException(LogicExceptionErrors::notImplemented);
    }

    std::function<void()> ComputeContext::CreateFunctionImpl(std::string fnName, std::function<void()> fn)
    {
        return [fn = std::move(fn), this] {
            FunctionScope scope(*this);
            fn();
        };
    }

    std::function<Value()> ComputeContext::CreateFunctionImpl(std::string fnName, Value expectedReturn,
                                                              std::function<Value()> fn)
    {
        return [fn = std::move(fn), this, expectedReturn]() {
            ConstantData movedOutOfScope;
            {
                FunctionScope scope(*this);

                Value returnValue = expectedReturn;
                returnValue = fn();

                movedOutOfScope = ExtractConstantData(returnValue);
            }
            _stack.top().push_front(std::move(movedOutOfScope));
            return ConstantDataToValue(_stack.top().front(), expectedReturn.GetLayout());
        };
    }

    std::function<void(std::vector<Value>)> ComputeContext::CreateFunctionImpl(
        std::string fnName, std::vector<Value> expectedArgs, std::function<void(std::vector<Value>)> fn)
    {
        return [fn = std::move(fn), this, expectedArgs = std::move(expectedArgs)](std::vector<Value> args) {
            assert(expectedArgs.size() == args.size());
            ConstantData movedOutOfScope;
            {
                FunctionScope scope(*this);
                auto fnArgs = expectedArgs;
                std::copy(args.begin(), args.end(), fnArgs.begin());

                fn(args);
            }
            _stack.top().push_front(std::move(movedOutOfScope));
        };
    }

    std::function<Value(std::vector<Value>)> ComputeContext::CreateFunctionImpl(
        std::string fnName, Value expectedReturn, std::vector<Value> expectedArgs,
        std::function<Value(std::vector<Value>)> fn)
    {
        return [fn = std::move(fn), this, expectedReturn, expectedArgs = std::move(expectedArgs)](
                   std::vector<Value> args) {
            assert(expectedArgs.size() == args.size());
            ConstantData movedOutOfScope;
            {
                FunctionScope scope(*this);
                auto fnArgs = expectedArgs;
                std::copy(args.begin(), args.end(), fnArgs.begin());

                Value returnValue = expectedReturn;
                returnValue = fn(args);

                movedOutOfScope = ExtractConstantData(returnValue);
            }
            _stack.top().push_front(std::move(movedOutOfScope));

            return ConstantDataToValue(_stack.top().front(), expectedReturn.GetLayout());
        };
    }

    void ComputeContext::CopyDataImpl(const Value& source, Value& destination)
    {
        std::visit(VariantVisitor{ [](Undefined) {},
                                   [](Emittable) {},
                                   [&destination, &source](auto&& sourceData) {
                                       using SourceDataType = std::decay_t<decltype(sourceData)>;

                                       auto& destinationData =
                                           std::get<SourceDataType>(destination.GetUnderlyingData());
                                       if (source.GetLayout().IsContiguous() && destination.GetLayout().IsContiguous())
                                       {
                                           auto numElements = destination.GetLayout().NumElements();
                                           std::copy(sourceData, sourceData + numElements, destinationData);
                                       }
                                       else
                                       {
                                           auto& sourceLayout = source.GetLayout();
                                           auto maxCoordinate = sourceLayout.GetActiveSize().ToVector();
                                           decltype(maxCoordinate) coordinate(maxCoordinate.size());

                                           do
                                           {
                                               auto logicalCoordinates = sourceLayout.GetLogicalCoordinates(coordinate);
                                               auto sourceOffset =
                                                   sourceLayout.GetLogicalEntryOffset(logicalCoordinates);
                                               auto destinationOffset =
                                                   destination.GetLayout().GetLogicalEntryOffset(logicalCoordinates);
                                               *(destinationData + destinationOffset) = *(sourceData + sourceOffset);
                                           } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                                       }
                                   } },
                   source.GetUnderlyingData());
    }

    void ComputeContext::MoveDataImpl(Value& source, Value& destination)
    {
        // we treat a move the same as a copy, except we clear out the source
        CopyDataImpl(source, destination);

        // data has been "moved", so clear the source
        source.Reset();
    }

    void ComputeContext::ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn)
    {
        auto maxCoordinate = layout.GetActiveSize().ToVector();
        decltype(maxCoordinate) coordinate(maxCoordinate.size());

        do
        {
            auto logicalCoordinates = layout.GetLogicalCoordinates(coordinate).ToVector();
            fn(std::vector<Scalar>(logicalCoordinates.begin(), logicalCoordinates.end()));
        } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
    }

    Value ComputeContext::UnaryOperationImpl(ValueUnaryOperation op, Value destination)
    {
        throw LogicException(LogicExceptionErrors::notImplemented);
    }

    Value ComputeContext::BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source)
    {
        if (!ValidateValue(source))
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }
        if (!ValidateValue(destination))
        {
            destination = Allocate(source.GetBaseType(), source.GetLayout());
        }

        if (!TypeCompatible(destination, source))
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        if (destination.GetLayout() != source.GetLayout())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }

        std::visit(VariantVisitor{ [](Undefined) {},
                                   [](Emittable) {},
                                   [](Boolean*) {},
                                   [&destination, &source, op](auto&& destinationData) {
                                       using DestinationDataType =
                                           std::remove_pointer_t<std::decay_t<decltype(destinationData)>>;

                                       std::function<DestinationDataType(DestinationDataType, DestinationDataType)>
                                           opFn;
                                       switch (op)
                                       {
                                       case ValueBinaryOperation::add:
                                           opFn = [](auto dst, auto src) { return dst + src; };
                                           break;
                                       case ValueBinaryOperation::subtract:
                                           opFn = [](auto dst, auto src) { return dst - src; };
                                           break;
                                       case ValueBinaryOperation::multiply:
                                           opFn = [](auto dst, auto src) { return dst * src; };
                                           break;
                                       case ValueBinaryOperation::divide:
                                           opFn = [](auto dst, auto src) { return dst / src; };
                                           break;
                                       }

                                       auto& sourceData = std::get<DestinationDataType*>(source.GetUnderlyingData());
                                       if (source.GetLayout().IsContiguous() && destination.GetLayout().IsContiguous())
                                       {
                                           auto numElements = destination.GetLayout().NumElements();
                                           std::transform(destinationData,
                                                          destinationData + numElements,
                                                          sourceData,
                                                          destinationData,
                                                          opFn);
                                       }
                                       else
                                       {
                                           auto& sourceLayout = source.GetLayout();
                                           auto maxCoordinate = sourceLayout.GetActiveSize().ToVector();
                                           decltype(maxCoordinate) coordinate(maxCoordinate.size());

                                           do
                                           {
                                               auto logicalCoordinates = sourceLayout.GetLogicalCoordinates(coordinate);
                                               auto sourceOffset =
                                                   sourceLayout.GetLogicalEntryOffset(logicalCoordinates);
                                               auto destinationOffset =
                                                   destination.GetLayout().GetLogicalEntryOffset(logicalCoordinates);
                                               *(destinationData + destinationOffset) =
                                                   opFn(*(destinationData + destinationOffset),
                                                        *(sourceData + sourceOffset));
                                           } while (IncrementMemoryCoordinate(coordinate, maxCoordinate));
                                       }
                                   } },
                   destination.GetUnderlyingData());

        return destination;
    }

    bool ComputeContext::ValidateValue(Value value)
    {
        return value.IsDefined() && !value.IsEmpty() && value.IsConstant();
    }

    bool ComputeContext::TypeCompatible(Value value1, Value value2)
    {
        return value1.GetBaseType() == value2.GetBaseType() && value1.PointerLevel() == value2.PointerLevel();
    }

} // namespace value
} // namespace ell
