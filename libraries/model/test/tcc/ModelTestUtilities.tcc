////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.tcc (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ell;

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
    out << "[";
    for (size_t index = 0; index < v.size(); ++index)
    {
        if (index != 0)
            out << ", ";
        out << v[index];
    }
    out << "]";
    return out;
}

template <typename InputType, typename OutputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    if (!IsVerbose())
    {
        return;
    }

    // compare output
    for (const auto& input : signal)
    {
        auto computedResult = map.Compute<OutputType>(input);
        auto compiledResult = compiledMap.Compute<OutputType>(input);
        std::cout << computedResult << " \t" << compiledResult << std::endl;
    }
}

template <typename InputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    switch (map.GetOutput(0).GetPortType())
    {
        case model::Port::PortType::boolean:
            PrintCompiledOutput<InputType, bool>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::integer:
            PrintCompiledOutput<InputType, int>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::bigInt:
            PrintCompiledOutput<InputType, int64_t>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::smallReal:
            PrintCompiledOutput<InputType, float>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::real:
            PrintCompiledOutput<InputType, double>(map, compiledMap, signal, name);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}

template <typename InputType, typename OutputType>
void VerifyMapOutput(const model::DynamicMap& map, std::vector<std::vector<InputType>>& signal, std::vector<std::vector<OutputType>>& expectedOutput, const std::string& name)
{
    bool ok = true;
    // compare output
    for(int index = 0; index < signal.size(); ++index)
    {
        auto&& input = signal[index];
        auto&& output = expectedOutput[index];
        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<OutputType>(0);

        ok = ok && testing::IsEqual(output, computedResult);

        if (IsVerbose())
        {
            std::cout << computedResult << " \t" << output << std::endl;
        }
    }
    testing::ProcessTest(std::string("Testing map " + name + " compute"), ok);
}

template <typename InputType, typename OutputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    bool ok = true;
    // compare output
    for (const auto& input : signal)
    {
        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<OutputType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<OutputType>(0);
        ok = ok && testing::IsEqual(computedResult, compiledResult);

        if (IsVerbose())
        {
            std::cout << computedResult << " \t" << compiledResult << std::endl;
        }
    }
    testing::ProcessTest(std::string("Testing compiled " + name + " compute"), ok);
}

template <typename InputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name)
{
    switch (map.GetOutput(0).GetPortType())
    {
        case model::Port::PortType::boolean:
            VerifyCompiledOutput<InputType, bool>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::integer:
            VerifyCompiledOutput<InputType, int>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::bigInt:
            VerifyCompiledOutput<InputType, int64_t>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::smallReal:
            VerifyCompiledOutput<InputType, float>(map, compiledMap, signal, name);
            break;
        case model::Port::PortType::real:
            VerifyCompiledOutput<InputType, double>(map, compiledMap, signal, name);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}

template <typename InputType>
void InputCallbackTester<InputType>::Initialize(const std::vector<std::vector<InputType>>& input)
{
    begin = input.begin();
    end = input.end();
    assert(begin != end);

    cur = begin;
}

template <typename InputType>
bool InputCallbackTester<InputType>::InputCallback(std::vector<InputType>& input)
{
    input = *cur;
    if (IsVerbose())
    {
        std::cout << "    InputCallback(vector), input[0]: " << input[0] << "\n";
    }

    if (++cur == end)
    {
        cur = begin; // simulate infinite series
    }
    return true;
}

template <typename InputType>
bool InputCallbackTester<InputType>::InputCallback(InputType* input)
{
    std::copy((*cur).begin(), (*cur).end(), input);
    if (IsVerbose())
    {
        std::cout << "    InputCallback(C array), input[0]: " << input[0] << "\n";
    }

    if (++cur == end)
    {
        cur = begin; // simulate infinite series
    }
    return true;
}
