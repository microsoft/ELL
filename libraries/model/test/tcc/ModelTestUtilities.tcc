////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.tcc (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace ell;

template<typename ValueType>
ValueType LargestDifference(const std::vector<ValueType>& a, const std::vector<ValueType>& b)
{
    ValueType largestDifference = 0;
    auto size = a.size();
    for (size_t index = 0; index < size; ++index)
    {
        auto difference = a[index] - b[index];
        if (std::fabs(difference) > std::fabs(largestDifference))
        {
            largestDifference = difference;
        }
    }
    return largestDifference;
}

template<typename ValueType>
bool IsEqual(const ValueType& a, const ValueType& b, double epsilon = 1e-6)
{
    return testing::IsEqual(a, b);
}

template<>
inline bool IsEqual(const float& a, const float& b, double epsilon)
{
    return testing::IsEqual(a, b, static_cast<float>(epsilon));
}

template<>
inline bool IsEqual(const double& a, const double& b, double epsilon)
{
    return testing::IsEqual(a, b, epsilon);
}

template<>
inline bool IsEqual(const std::vector<float>& a, const std::vector<float>& b, double epsilon)
{
    return testing::IsEqual(a, b, static_cast<float>(epsilon));
}

template<>
inline bool IsEqual(const std::vector<double>& a, const std::vector<double>& b, double epsilon)
{
    return testing::IsEqual(a, b, epsilon);
}

template<typename T>
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

template<typename InputType, typename OutputType>
void PrintCompiledOutput(const model::Map& map, const model::IRCompiledMap& compiledMap, const std::vector<std::vector<InputType>>& signal, const std::string& name)
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

template<typename InputType>
void PrintCompiledOutput(const model::Map& map, const model::IRCompiledMap& compiledMap, const std::vector<std::vector<InputType>>& signal, const std::string& name)
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

template<typename InputType, typename OutputType>
void VerifyMapOutput(const model::Map& map, std::vector<std::vector<InputType>>& signal, std::vector<std::vector<OutputType>>& expectedOutput, const std::string& name)
{
    bool ok = true;
    // compare output
    for (size_t index = 0; index < signal.size(); ++index)
    {
        auto&& input = signal[index];
        auto&& output = expectedOutput[index];
        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<OutputType>(0);

        ok = ok && IsEqual(output, computedResult);

        if (IsVerbose())
        {
            std::cout << computedResult << " \t" << output << std::endl;
        }
    }
    testing::ProcessTest(std::string("Testing map " + name + " compute"), ok);
}

template<typename InputType, typename OutputType>
void VerifyCompiledOutput(const model::Map& map, const model::IRCompiledMap& compiledMap, const std::vector<std::vector<InputType>>& signal, const std::string& name, double epsilon)
{
    bool ok = true;
    // compare output
    for (const auto& input : signal)
    {
        map.SetInputValue(0, input);
        auto computedResult = map.ComputeOutput<OutputType>(0);

        compiledMap.SetInputValue(0, input);
        auto compiledResult = compiledMap.ComputeOutput<OutputType>(0);
        ok = ok && IsEqual(computedResult, compiledResult, static_cast<OutputType>(epsilon));

        if (IsVerbose())
        {
            std::cout << "input: " << input << std::endl;
            std::cout << "computed: " << computedResult << " \n\ncompiled: " << compiledResult << std::endl;
            if (!ok)
            {
                std::cout << "Largest difference: " << LargestDifference(computedResult, compiledResult) << ", epsilon: " << epsilon << std::endl;
            }
        }
    }
    testing::ProcessTest(std::string("Testing compiled " + name + " compute"), ok);
}

template<typename InputType>
void VerifyCompiledOutput(const model::Map& map, const model::IRCompiledMap& compiledMap, const std::vector<std::vector<InputType>>& signal, const std::string& name, double epsilon)
{
    switch (map.GetOutput(0).GetPortType())
    {
        case model::Port::PortType::boolean:
            VerifyCompiledOutput<InputType, bool>(map, compiledMap, signal, name, epsilon);
            break;
        case model::Port::PortType::integer:
            VerifyCompiledOutput<InputType, int>(map, compiledMap, signal, name, epsilon);
            break;
        case model::Port::PortType::bigInt:
            VerifyCompiledOutput<InputType, int64_t>(map, compiledMap, signal, name, epsilon);
            break;
        case model::Port::PortType::smallReal:
            VerifyCompiledOutput<InputType, float>(map, compiledMap, signal, name, epsilon);
            break;
        case model::Port::PortType::real:
            VerifyCompiledOutput<InputType, double>(map, compiledMap, signal, name, epsilon);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
    }
}
