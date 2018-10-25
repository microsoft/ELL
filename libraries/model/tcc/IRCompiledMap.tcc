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
                std::get<Vector<bool>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void(*)(void*, const InputType*, bool*)>(functionPointer);
                computeFunction = [this, fn](void* context, const InputType* input) {
                    fn(context, input, (bool*)std::get<Vector<bool>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::integer:
            {
                std::get<Vector<int>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void(*)(void*, const InputType*, int*)>(functionPointer);
                computeFunction = [this, fn](void* context, const InputType* input) {
                    fn(context, input, std::get<Vector<int>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::bigInt:
            {
                std::get<Vector<int64_t>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void(*)(void*, const InputType*, int64_t*)>(functionPointer);
                computeFunction = [this, fn](void* context, const InputType* input) {
                    fn(context, input, std::get<Vector<int64_t>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::smallReal:
            {
                std::get<Vector<float>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void(*)(void*, const InputType*, float*)>(functionPointer);
                computeFunction = [this, fn](void* context, const InputType* input) {
                    fn(context, input, std::get<Vector<float>>(_cachedOutput).data());
                };
            }
            break;

            case model::Port::PortType::real:
            {
                std::get<Vector<double>>(_cachedOutput).resize(outputSize);
                auto fn = reinterpret_cast<void(*)(void*, const InputType*, double*)>(functionPointer);
                computeFunction = [this, fn](void* context, const InputType* input) {
                    fn(context, input, std::get<Vector<double>>(_cachedOutput).data());
                };
            }
            break;

            default:
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }

            std::get<ComputeFunction<InputType>>(_computeInputFunction) = computeFunction;
        }
    }

    template<typename ElementType>
    ElementType* IRCompiledMap::GetGlobalValuePointer(const std::string& name)
    {
        auto& jitter = GetJitter();
        auto address = jitter.GetGlobalValueAddress(name);
        return reinterpret_cast<ElementType*>(address);
    }

}
}
