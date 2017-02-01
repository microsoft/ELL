////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterTypes.tcc (emitter)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    template <typename T>
    std::vector<typename VariableValueType<T>::DestType> VariableValueType<T>::ToVariableVector(const std::vector<T>& src)
    {
        return src;
    }

    template <typename T>
    std::vector<T> VariableValueType<T>::FromVariableVector(const std::vector<typename VariableValueType<T>::DestType>& src)
    {
        return src;
    }

    // bool specialization
    inline std::vector<typename VariableValueType<bool>::DestType> VariableValueType<bool>::ToVariableVector(const std::vector<bool>& src)
    {
        std::vector<VariableValueType<bool>::DestType> result(src.begin(), src.end());
        return result;
    }

    inline std::vector<bool> VariableValueType<bool>::FromVariableVector(const std::vector<typename VariableValueType<bool>::DestType>& src)
    {
        std::vector<bool> result(src.begin(), src.end());
        return result;
    }
}
}
