////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRCompiledMap.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    template <typename InputType>
    void IRCompiledMap::SetComputeFunctionForInputType() const
    {
        if (!_computeFunctionDefined)
        {
            _computeFunctionDefined = true;
            auto outputSize = GetOutput(0).Size();
            auto functionPointer = _executionEngine->ResolveFunctionAddress(_functionName);
            ComputeFunction<InputType> computeFunction;
            switch (GetOutput(0).GetPortType()) // Switch on output type
            {
            case model::Port::PortType::boolean:
            {
                std::get<utilities::ConformingVector<bool>>(_cachedOutput).resize(outputSize);
                if (GetInput(0)->Size() == 1)
                {
                    // scalar input
                    auto fn = reinterpret_cast<void(*)(const InputType, bool*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(*input, (bool*)std::get<utilities::ConformingVector<bool>>(_cachedOutput).data());
                    };
                }
                else
                {
                    // vector input
                    auto fn = reinterpret_cast<void(*)(const InputType*, bool*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(input, (bool*)std::get<utilities::ConformingVector<bool>>(_cachedOutput).data());
                    };
                }
            }
            break;

            case model::Port::PortType::integer:
            {
                std::get<utilities::ConformingVector<int>>(_cachedOutput).resize(outputSize);
                if (GetInput(0)->Size() == 1)
                {
                    // scalar input
                    auto fn = reinterpret_cast<void(*)(const InputType, int*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(*input, std::get<utilities::ConformingVector<int>>(_cachedOutput).data());
                    };
                }
                else
                {
                    // vector input
                    auto fn = reinterpret_cast<void(*)(const InputType*, int*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(input, std::get<utilities::ConformingVector<int>>(_cachedOutput).data());
                    };
                }
            }
            break;

            case model::Port::PortType::bigInt:
            {
                std::get<utilities::ConformingVector<int64_t>>(_cachedOutput).resize(outputSize);
                if (GetInput(0)->Size() == 1)
                {
                    // scalar input
                    auto fn = reinterpret_cast<void(*)(const InputType, int64_t*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(*input, std::get<utilities::ConformingVector<int64_t>>(_cachedOutput).data());
                    };
                }
                else
                {
                    // vector input
                    auto fn = reinterpret_cast<void(*)(const InputType*, int64_t*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(input, std::get<utilities::ConformingVector<int64_t>>(_cachedOutput).data());
                    };
                }
            }
            break;

            case model::Port::PortType::smallReal:
            {
                std::get<utilities::ConformingVector<float>>(_cachedOutput).resize(outputSize);
                if (GetInput(0)->Size() == 1)
                {
                    // scalar input
                    auto fn = reinterpret_cast<void(*)(const InputType, float*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(*input, std::get<utilities::ConformingVector<float>>(_cachedOutput).data());
                    };
                }
                else
                {
                    // vector input
                    auto fn = reinterpret_cast<void(*)(const InputType*, float*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(input, std::get<utilities::ConformingVector<float>>(_cachedOutput).data());
                    };
                }
            }
            break;

            case model::Port::PortType::real:
            {
                std::get<utilities::ConformingVector<double>>(_cachedOutput).resize(outputSize);
                if (GetInput(0)->Size() == 1)
                {
                    // scalar input
                    auto fn = reinterpret_cast<void(*)(const InputType, double*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(*input, std::get<utilities::ConformingVector<double>>(_cachedOutput).data());
                    };
                }
                else
                {
                    // vector input
                    auto fn = reinterpret_cast<void(*)(const InputType*, double*)>(functionPointer);
                    computeFunction = [this, fn](const InputType* input) {
                        fn(input, std::get<utilities::ConformingVector<double>>(_cachedOutput).data());
                    };
                }
            }
            break;

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }

            std::get<ComputeFunction<InputType>>(_computeInputFunction) = computeFunction;
        }
    }
}
}
