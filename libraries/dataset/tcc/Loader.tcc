// Loader.tcc

#include <memory>
using std::move;

namespace dataset
{
    template<typename RowIteratorType, typename VectorEntryParserType>
    RowDataset Loader::Load(RowIteratorType line_iterator, VectorEntryParserType parser)
    {

        // parse each line according to the parser
        ParsingIterator<RowIteratorType, VectorEntryParserType> iter(line_iterator, parser);

        // create a dataset
        RowDataset dataset;

        // Load row by row
        while (iter.IsValid())
        {
            dataset.PushBackRow(iter.Get());
            iter.Next();
        }

        return move(dataset);
    }
}