// StringFormat.tcc

namespace utilities
{
    template<typename ... Args>
    string StringFormat(const string& input, Args ...args)
    {
        // get c-string
        const char* cstr = input.c_str();
        
        // allocate buffer of correct size
        auto size = snprintf(nullptr, 0, cstr, args ...);
        auto buf = new char[size + 1];      // leave room for trailing \0
        
        // format
        snprintf(buf, size + 1, cstr, args ...);
        
        // cast back to stl::string
        auto result = string(buf, buf + size);

        // cleanup and return
        delete[] buf;
        return result;
    }
}