////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TypeInfo.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename T>
    std::string TypeInfo<T>::GetSerializationName()
    {
        return std::string(T::GetSerializationName());
    }

    template<typename T>
    std::string TypeInfo<std::shared_ptr<T>>::GetSerializationName()
    {
        return "ptr(" + TypeInfo<T>::GetSerializationName() + ")";
    }

    template<typename T>
    std::string TypeInfo<std::vector<T>>::GetSerializationName()
    { 
        return "vector(" + TypeInfo<T>::GetSerializationName() + ")"; 
    }
}