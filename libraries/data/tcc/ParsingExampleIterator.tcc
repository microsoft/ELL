////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParsingExampleIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AutoDataVector.h"
#include "SupervisedExampleBuilder.h"

namespace ell
{
namespace data
{
    template <typename RowIteratorType, typename VectorElementParserType>
    ParsingExampleIterator<RowIteratorType, VectorElementParserType>::ParsingExampleIterator(RowIteratorType row_iter, VectorElementParserType parser)
        : _rowIterator(std::move(row_iter)), _instanceParser(std::move(parser))
    {
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    AutoSupervisedExample ParsingExampleIterator<RowIteratorType, VectorElementParserType>::Get() const
    {
        SupervisedExampleBuilder<VectorElementParserType, AutoDataVector> exampleBuilder(_instanceParser);
        return exampleBuilder.Build(_rowIterator.Get());
    }

    template <typename RowIteratorType, typename VectorElementParserType>
    ExampleIterator<AutoSupervisedExample> GetParsingExampleIterator(RowIteratorType row_iter, VectorElementParserType parser)
    {
        auto pIterator = std::make_unique<ParsingExampleIterator<RowIteratorType, VectorElementParserType>>(std::move(row_iter), std::move(parser));
        return ExampleIterator<AutoSupervisedExample>(std::move(pIterator));
    }
}
}
