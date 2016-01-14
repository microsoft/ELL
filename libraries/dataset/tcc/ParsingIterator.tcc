// ParsingIterator.tcc

#include "SupervisedExampleBuilder.h"
#include "DenseDataVector.h"

//stl
#include <memory>
using std::move;

namespace dataset
{
    template<typename RowIteratorType, typename VectorEntryParserType>
    ParsingIterator<RowIteratorType, VectorEntryParserType>::ParsingIterator(RowIteratorType&& row_iter, const VectorEntryParserType& parser) :
        _rowIterator(move(row_iter)), _instanceParser(parser)
    {}

    template<typename RowIteratorType, typename VectorEntryParserType>
    bool ParsingIterator<RowIteratorType, VectorEntryParserType>::IsValid() const
    {
        return _rowIterator.IsValid();
    }

    template<typename RowIteratorType, typename VectorEntryParserType>
    void ParsingIterator<RowIteratorType, VectorEntryParserType>::Next()
    {
        _rowIterator.Next();
    }

    template<typename RowIteratorType, typename VectorEntryParserType>
    SupervisedExample ParsingIterator<RowIteratorType, VectorEntryParserType>::Get()
    {
        // TODO - currently, FloatDataVector is hardcoded. Instead, use a MRU strategy
        SupervisedExampleBuilder<VectorEntryParserType, FloatDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.Get());
    }

    template<typename RowIteratorType, typename VectorEntryParserType>
    unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType&& row_iter, const VectorEntryParserType& parser)
    {
        return make_unique<ParsingIterator<RowIteratorType, VectorEntryParserType>>(move(row_iter), parser);
    }

}