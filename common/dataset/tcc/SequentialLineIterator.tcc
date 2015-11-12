// SequentialLineIterator.tcc

namespace dataset
{
	template<char Delimiter>
	istream& operator>> (istream& is, delimited_text<Delimiter>& ln)
	{
		getline(is, ln, Delimiter);
		return is;
	}
}