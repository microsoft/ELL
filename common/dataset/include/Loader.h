// Loader.h

#pragma once

#include "RowMatrix.h"
using namespace linear;

#include "SupervisedExample.h"

#include<string>
using std::string;

namespace dataset
{
	typedef RowMatrix<SupervisedExample> RowDataset;

	namespace Loader
	{

		template<typename RowIteratorType, typename ParserType>
		RowDataset Load(RowIteratorType& line_iterator, ParserType& parser);

	}
}

#include "../tcc/Loader.tcc"

