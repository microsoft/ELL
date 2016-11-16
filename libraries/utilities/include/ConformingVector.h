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
    /// <summary> 
    /// ConformingVector is a type alias used to to work around the nonconformant 
    /// std::vector<bool> specialization. ConformingVector<T> can be used as a drop-in
    /// replacement to std::vector<> in generic code, and evaluates to std::vector<T> for non-bool T
    /// and evaluates to std::vector<BoolProxy> when T == bool.
    /// </summary>

    /// <summary> 
    /// BoolProxy is a simple wrapper around a bool type, which is used internally
    /// by the ConformingVector type alias. 
    /// </summary>
    class BoolProxy
    {
    public:
        BoolProxy() = default;
        
        /// <summary> Constructor </summary>
        ///
        /// <param name="value"> The value to take </param>
        BoolProxy(bool value);

        /// <summary> Constructor </summary>
        ///
        /// <param name="value"> The value to take </param>
        BoolProxy(std::vector<bool>::reference value);

        /// <summary> Cast operator to bool </summary>
        ///
        /// <returns> The boolean value wrapped by this object </returns>
        operator bool() const { return _value; }

    private:
        bool _value = false;
    };

    /// <summary> A trivial type-wrapping template for std::vectors </summary> 
    template<typename ValueType>
    struct VectorType
    {
        using type = std::vector<ValueType>;
    };

    /// <summary> Specialization for bool case </summary>
    template<>
    struct VectorType<bool>
    {
        using type = std::vector<BoolProxy>;
    };

    /// <summary> 
    /// A type alias that returns a std::vector of the requested type, or 
    /// a vector of BoolProxy, if the template parameter is bool.
    /// </summary>
    template <typename ValueType>
    using ConformingVector = typename VectorType<ValueType>::type;
}
}
