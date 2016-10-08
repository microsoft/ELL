////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsingIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AutoDataVector.h"
#include "SupervisedExampleBuilder.h"

namespace emll
{
namespace data
{
    template <typename RowIteratorType, typename VectorElementParserType>
    ParsingIterator<RowIteratorType, VectorElementParserType>::ParsingIterator(RowIteratorType row_iter, VectorElementParserType parser)
        : _rowIterator(std::move(row_iter)), _instanceParser(std::move(parser))
    {
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    AutoSupervisedExample ParsingIterator<RowIteratorType, VectorElementParserType>::Get() const
    {
        SupervisedExampleBuilder<VectorElementParserType, AutoDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.Get());
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType row_iter, VectorElementParserType parser)
    {
        return std::make_unique<ParsingIterator<RowIteratorType, VectorElementParserType>>(std::move(row_iter), std::move(parser));
    }
}
}
