////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.tcc (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// data
#include "SingleLineParsingExampleIterator.h"

// Model
#include "IRMapCompiler.h"
#include "IRCompiledMap.h" 

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

    template <typename ExampleType, typename MapType>
    auto TransformDatasetWithCompiledMap(data::Dataset<ExampleType>& input, const MapType& map)
    {
        model::IRMapCompiler compiler;
        auto compiledMap = compiler.Compile(map);

        return input.template Transform<ExampleType>([&compiledMap](const ExampleType& example)
        {
            compiledMap.SetInputValue(0, example.GetDataVector());
            auto transformedDataVector = compiledMap.template ComputeOutput<typename ExampleType::DataVectorType>(0);

            return ExampleType(std::move(transformedDataVector), example.GetMetadata());
        });
    }


}
}
