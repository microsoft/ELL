////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConformingVector.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <vector>

namespace emll
{
namespace utilities
{

    /// <summary> A simple wrapper around bool type. Used to avoid nonconformant std::vector<bool> specialization </summary>
    class BoolProxy
    {
    public:
        BoolProxy() = default;
        BoolProxy(bool value);
        BoolProxy(std::vector<bool>::reference value);
        operator bool() const { return _value; }

    private:
        bool _value = false;
    };

    template<typename ValueType>
    struct VectorType
    {
        using type = std::vector<ValueType>;
    };

    template<>
    struct VectorType<bool>
    {
        using type = std::vector<BoolProxy>;
    };

    template <typename ValueType>
    using ConformingVector = typename VectorType<ValueType>::type;
}
}
