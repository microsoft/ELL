// Io.cpp

#include "deserializer.h"
#include "Column.h"
#include "Io.h"
#include <string>
#include <memory>
#include <stdexcept>

using std::istreambuf_iterator;
using std::string;
using std::make_shared;
using std::dynamic_pointer_cast;

namespace mappings
{
	shared_ptr<Mapping> Io::read(istream& is)
	{
		// parse stream contents
		auto str = string(istreambuf_iterator<char>(is), istreambuf_iterator<char>());
		JsonSerializer reader;
		reader.FromString(str);
		
		// read Mapping
		auto map =  shared_ptr<Mapping>(nullptr);
		reader.read("Base", map);

		return map;
	}

	shared_ptr<Column> Io::ReadColumn(istream& is, int num_layers)
	{
		auto map = read(is);
		auto col = dynamic_pointer_cast<Column>(map);
		if (col == nullptr)
		{
			throw runtime_error("stream does not contain a Column Mapping");
		}
		col->KeepLayers(num_layers);
		return col;
	}

	void Io::write(ostream& os, shared_ptr<Mapping> map)
	{	
		// Serialize Mapping
		JsonSerializer writer;
		writer.write("Base", map);
		auto str = writer.to_string();
		
		// write to stream
		os << str;
	}
}
