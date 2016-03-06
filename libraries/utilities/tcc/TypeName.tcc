////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TypeName.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename T>
    std::string TypeName<T>::GetSerializationName()
    {
        return std::string(T::GetSerializationName());
    }

    template<typename T>
    std::string TypeName<std::shared_ptr<T>>::GetSerializationName()
    {
        return "ptr(" + TypeName<T>::GetSerializationName() + ")";
    }

    template<typename T>
    std::string TypeName<std::vector<T>>::GetSerializationName()
    { 
        return "vector(" + TypeName<T>::GetSerializationName() + ")"; 
    }
}