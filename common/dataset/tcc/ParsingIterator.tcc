// ParsingIterator.tcc

namespace dataset
{

	template<typename RowIteratorType, typename ParserType>
	ParsingIterator<RowIteratorType, ParserType>::ParsingIterator(RowIteratorType& row_iter, ParserType& parser) :
		_row_iter(row_iter), _parser(parser)
	{}

	template<typename RowIteratorType, typename ParserType>
	bool ParsingIterator<RowIteratorType, ParserType>::IsValid() const
	{
		return _row_iter.IsValid();
	}


	template<typename RowIteratorType, typename ParserType>
	void ParsingIterator<RowIteratorType, ParserType>::Next()
	{
		_row_iter.Next();
	}

	template<typename RowIteratorType, typename ParserType>
	SupervisedExample ParsingIterator<RowIteratorType, ParserType>::GetValue()
	{
		return _parser.parse(_row_iter.GetValue());
	}
}