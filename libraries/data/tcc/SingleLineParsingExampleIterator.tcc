////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SingleLineParsingExampleIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::SingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser)
        : _textLineIterator(std::move(textLineIterator)), _metadataParser(std::move(metadataParser)), _dataVectorParser(std::move(dataVectorParser))
    {
        if (_textLineIterator.IsValid())
        {
            ReadExample();
        }
    }

    template<typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    void SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::Next()
    {
        _textLineIterator.Next();
        ReadExample();
    }

    template<typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    void SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>::ReadExample()
    {
        // get a line - skip lines that contain just whitespace or just a comment
        TextLine line = _textLineIterator.GetTextLine();
        line.TrimLeadingWhitespace();

        while (line.IsEndOfContent())
        {
            _textLineIterator.Next();
            if (!_textLineIterator.IsValid())
            {
                return;
            }

            line = _textLineIterator.GetTextLine();
            line.TrimLeadingWhitespace();
        }

        // parse metadata
        auto metaData = _metadataParser.Parse(line);
        
        // parse datavector
        auto dataVector = _dataVectorParser.Parse(line);
        
        // cache the parsed example
        _currentExample = ExampleType(std::move(dataVector), std::move(metaData));
    }

    template <typename TextLineIteratorType, typename MetadataParserType, typename DataVectorParserType>
    auto MakeSingleLineParsingExampleIterator(TextLineIteratorType textLineIterator, MetadataParserType metadataParser, DataVectorParserType dataVectorParser)
    {
        using ExampleType = ParserExample<DataVectorParserType, MetadataParserType>;
        using IteratorType = SingleLineParsingExampleIterator<TextLineIteratorType, MetadataParserType, DataVectorParserType>;
        auto iterator = std::make_unique<IteratorType>(std::move(textLineIterator), std::move(metadataParser), std::move(dataVectorParser));
        return ExampleIterator<ExampleType>(std::move(iterator));
    }
}
}