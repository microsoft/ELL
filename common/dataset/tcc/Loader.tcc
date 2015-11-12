// Loader.tcc

#include <memory>
using std::move;

namespace dataset
{
    template<typename RowIteratorType, typename ParserType>
    RowDataset Loader::Load(RowIteratorType& line_iterator, ParserType& parser)
    {

        // parse each line according to the parser
        ParsingIterator<RowIteratorType, ParserType> iter(line_iterator, parser);

        // create a dataset
        RowDataset dataset;

        // Load row by row
        while (iter.IsValid())
        {
            dataset.PushBackRow(iter.GetValue());
            iter.Next();
        }

        return move(dataset);
    }
}