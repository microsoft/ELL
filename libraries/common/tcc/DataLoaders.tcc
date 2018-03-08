////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.tcc (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// data
#include "SingleLineParsingExampleIterator.h"

// model
#include "IRMapCompiler.h"
#include "IRCompiledMap.h" 

// nodes
#include "ClockNode.h" // for nodes::TimeTickType

namespace ell
{
namespace common
{
    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    auto GetExampleIterator(std::istream& stream)
    {
        TextLineIteratorType textLineIterator(stream);

        MetadataParserType metadataParser;

        DataVectorParserType dataVectorParser;

        return data::MakeSingleLineParsingExampleIterator(std::move(textLineIterator), std::move(metadataParser), std::move(dataVectorParser));
    }


    template <typename ExampleType, typename MapType>
    auto TransformDataset(data::Dataset<ExampleType>& input, const MapType& map)
    {
        return input.template Transform<ExampleType>([map](const ExampleType& example)
        {
            auto transformedDataVector = map.template Compute<data::DoubleDataVector>(example.GetDataVector());
            return ExampleType(std::move(transformedDataVector), example.GetMetadata());
        });
    }

    namespace detail
    {
        // this ugly global goes away once callbacks support passing around a context
        static std::vector<double> g_inputValues;
    }

    // C functions called by compiled maps
    extern "C"
    {
    inline bool InputCallback_Double(double* input)
    {
        std::copy(detail::g_inputValues.begin(), detail::g_inputValues.end(), input);
        return true;
    }

    inline bool InputCallback_Float(float* input)
    {
        std::transform(detail::g_inputValues.begin(), detail::g_inputValues.end(), input, [](double val){ return static_cast<float>(val); });
        return true;
    }
    }

    namespace detail
    {
        // Sets up the function address that the LLVM jit will call for the source function callback
        // Note that this only supports a single source node, but can be extended in the future
        // to support multiple source nodes (e.g. by switching the function on node id).
        template <typename MapType>
        void ResolveInputCallback(const MapType& map, llvm::Module* module, ell::emitters::IRExecutionEngine& jitter)
        {
            const std::string defaultCallbackName("ELL_InputCallback");
            auto callback = module->getFunction(defaultCallbackName);

            ptrdiff_t callbackAddress = 0;
            switch (map.GetInputType())
            {
            case model::Port::PortType::smallReal:
            {
                callbackAddress = reinterpret_cast<ptrdiff_t>(&InputCallback_Float);
                break;
            }
            case model::Port::PortType::real:
            {
                callbackAddress = reinterpret_cast<ptrdiff_t>(&InputCallback_Double);
                break;
            }
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unexpected source input type for model. Should be double or float.");
            }

            jitter.DefineFunction(callback, callbackAddress);
        }

        template <typename DataVectorType>
        void SetCallbackInputValue(const DataVectorType& values)
        {
            g_inputValues = values.ToArray();
        }
    }

    template <typename ExampleType, typename MapType>
    auto TransformDatasetWithCompiledMap(data::Dataset<ExampleType>& input, const MapType& map, bool useBlas)
    {
        ell::model::MapCompilerOptions settings;
        settings.compilerSettings.useBlas = useBlas;

        model::IRMapCompiler compiler(settings);

        auto module = compiler.GetModule().GetLLVMModule();
        auto compiledMap = compiler.Compile(map);

        // Unlike reference maps, compiled maps receive the current time as the parameter input and
        // values through the input callback.
        detail::ResolveInputCallback(map, module, compiledMap.GetJitter());

        return input.template Transform<ExampleType>([&compiledMap](const ExampleType& example)
        {
            compiledMap.SetInputValue(0, std::vector<nodes::TimeTickType>({ 0 /*currentTime*/ }));
            detail::SetCallbackInputValue(example.GetDataVector());
            auto transformedDataVector = compiledMap.template ComputeOutput<typename ExampleType::DataVectorType>(0);
            return ExampleType(std::move(transformedDataVector), example.GetMetadata());
        });
    }

}
}
