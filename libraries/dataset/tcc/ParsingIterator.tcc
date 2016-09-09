////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsingIterator.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"
#include "SupervisedExampleBuilder.h"

namespace emll
{
namespace dataset
{
    template <typename RowIteratorType, typename VectorEntryParserType>
    ParsingIterator<RowIteratorType, VectorEntryParserType>::ParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser)
        : _rowIterator(std::move(row_iter)), _instanceParser(std::move(parser))
    {
    }

    template <typename RowIteratorType, typename VectorEntryParserType>
    GenericSupervisedExample ParsingIterator<RowIteratorType, VectorEntryParserType>::Get() const
    {
        SupervisedExampleBuilder<VectorEntryParserType, FloatDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.Get());
    }

    template <typename RowIteratorType, typename VectorEntryParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser)
    {
        return std::make_unique<ParsingIterator<RowIteratorType, VectorEntryParserType>>(std::move(row_iter), std::move(parser));
    }
}
}
