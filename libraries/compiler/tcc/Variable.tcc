namespace emll
{
	namespace compiler
	{
		template<ValueType DataType, typename T>
		ScalarVar<DataType, T>::ScalarVar(T data)
			: Variable(DataType), _data(data)
		{
		}

		template<ValueType DataType, typename T>
		VectorVar<DataType, T>::VectorVar()
			: Variable(DataType)
		{
		}
	}
}
