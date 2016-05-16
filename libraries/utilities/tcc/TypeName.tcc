////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeName.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template<typename T>
    std::string TypeName<std::unique_ptr<T>>::GetName()
    {
        return std::string("ptr") + typeNameLeftBracket + TypeName<T>::GetName() + typeNameRightBracket;
    }

    template<typename T>
    std::string TypeName<std::vector<T>>::GetName()
    { 
        return std::string("vector") + typeNameLeftBracket + TypeName<T>::GetName() + typeNameRightBracket; 
    }
}