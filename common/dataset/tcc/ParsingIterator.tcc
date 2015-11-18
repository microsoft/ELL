// ParsingIterator.tcc

#include "SupervisedExampleBuilder.h"

#include "DenseDataVector.h"
using linear::FloatDataVector;

namespace dataset
{
    template<typename RowIteratorType, typename VectorEntryParserType>
    ParsingIterator<RowIteratorType, VectorEntryParserType>::ParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser) :
        _rowIterator(row_iter), _instanceParser(parser)
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
    SupervisedExample ParsingIterator<RowIteratorType, VectorEntryParserType>::GetValue()
    {
        SupervisedExampleBuilder<VectorEntryParserType, FloatDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.GetValue());
    }
}