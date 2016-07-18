namespace emll
{
	namespace compiler
	{
		template<typename T>
		void TypeList<T>::init(std::initializer_list<T> args)
		{
			clear();
			for (auto v = args.begin(); v != args.end(); ++v)
			{
				push_back(*v);
			}
		}
	}
}