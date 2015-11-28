// Io.cpp

#include "Io.h"

#include "deserializer.h"

#include <string>
using std::string;
using std::istreambuf_iterator;

#include <memory>
using std::make_shared;

namespace layers
{
    shared_ptr<Map> Io::Read(istream& is)
    {
        // parse stream contents
        auto str = string(istreambuf_iterator<char>(is), istreambuf_iterator<char>());
        JsonSerializer reader;
        reader.FromString(str);
        
        // read Map
        auto map =  shared_ptr<Map>(nullptr);
        reader.Read("Base", map);

        return map;
    }

    void Io::Write(ostream& os, shared_ptr<Map> map)
    {    
        // Serialize Mapping
        JsonSerializer writer;
        writer.Write("Base", map);
        auto str = writer.ToString();
        
        // write to stream
        os << str;
    }
}
