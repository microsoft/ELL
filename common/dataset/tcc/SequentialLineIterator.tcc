// SequentialLineIterator.tcc

namespace dataset
{
	template<char Delimiter>
	istream& operator>> (istream& is, DelimitedText<Delimiter>& ln)
	{
		getline(is, ln, Delimiter);
		return is;
	}
}