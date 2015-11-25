//// Io.cpp
//
//#include "deserializer.h"
//#include "Column.h"
//#include "Io.h"
//
//#include <string>
//using std::string;
//using std::istreambuf_iterator;
//
//#include <memory>
//using std::make_shared;
//using std::dynamic_pointer_cast;
//
//#include <stdexcept>
//using std::runtime_error;
//
//namespace mappings
//{
//    shared_ptr<Mapping> Io::Read(istream& is)
//    {
//        // parse stream contents
//        auto str = string(istreambuf_iterator<char>(is), istreambuf_iterator<char>());
//        JsonSerializer reader;
//        reader.FromString(str);
//        
//        // read Mapping
//        auto map =  shared_ptr<Mapping>(nullptr);
//        reader.Read("Base", map);
//
//        return map;
//    }
//
//    shared_ptr<Column> Io::ReadColumn(istream& is, int numLayers)
//    {
//        auto map = Read(is);
//        auto col = dynamic_pointer_cast<Column>(map);
//        if (col == nullptr)
//        {
//            throw runtime_error("stream does not contain a column Mapping");
//        }
//        col->KeepLayers(numLayers);
//        return col;
//    }
//
//    void Io::Write(ostream& os, shared_ptr<Mapping> map)
//    {    
//        // Serialize Mapping
//        JsonSerializer writer;
//        writer.Write("Base", map);
//        auto str = writer.ToString();
//        
//        // write to stream
//        os << str;
//    }
//}
