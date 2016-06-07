#pragma once

#include <functional>
#include <ostream>

//
// UniqueId: A placeholder for a real GUID-type class
//
class UniqueId
{
public:    
    // Constructor for a unique id
    UniqueId();
    UniqueId(const UniqueId& other) : _id(other._id) {}

    UniqueId& operator=(const UniqueId& other)
    {
        _id = other._id;
        return *this;
    }

    bool operator==(const UniqueId& other) const;
    bool operator!=(const UniqueId& other) const;

    friend std::ostream& operator<<(std::ostream& stream, const UniqueId& id) { stream << id._id; return stream; }
private:
    friend std::hash<UniqueId>;
    size_t _id;
    static size_t _nextId;
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<> class hash<UniqueId>
    {
    public:
        typedef UniqueId argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& id) const;
    };
}
