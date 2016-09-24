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
    template <typename RowIteratorType, typename VectorElementParserType>
    ParsingIterator<RowIteratorType, VectorElementParserType>::ParsingIterator(RowIteratorType row_iter, VectorElementParserType parser)
        : _rowIterator(std::move(row_iter)), _instanceParser(std::move(parser))
    {
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    GenericSupervisedExample ParsingIterator<RowIteratorType, VectorElementParserType>::Get() const
    {
        SupervisedExampleBuilder<VectorElementParserType, FloatDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.Get());
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType row_iter, VectorElementParserType parser)
    {
        return std::make_unique<ParsingIterator<RowIteratorType, VectorElementParserType>>(std::move(row_iter), std::move(parser));
    }
}
}
