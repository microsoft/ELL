////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Archiver.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    //
    // PropertyArchiver class
    //

    template <typename ValueType>
    void Archiver::PropertyArchiver::operator<<(ValueType&& value)
    {
        _archiver.Archive(_propertyName.c_str(), value);
    }

    //
    // Archiver class
    //

    template <typename ValueType>
    void Archiver::Archive(ValueType&& value)
    {
        Archive("", std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Archiver::operator<<(ValueType&& value)
    {
        Archive(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Archiver::Archive(const char* name, ValueType&& value)
    {
        ArchiveItem(name, value);
    }

    //
    // Implementations
    //

    // Non-vectors
    template <typename ValueType, IsNotVector<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, ValueType&& value)
    {
        ArchiveValue(name, value);
    }

    // Pointers
    template <typename ValueType>
    void Archiver::ArchiveItem(const char* name, ValueType* value)
    {
        Archive(name, *value);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, const std::vector<ValueType>& array)
    {
        ArchiveArray(name, array);
    }

    // Vector of strings
    inline void Archiver::ArchiveItem(const char* name, const std::vector<std::string>& array)
    {
        ArchiveArray(name, array);
    }

    // Vector of serializable objects
    template <typename ValueType, IsIArchivable<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, const std::vector<ValueType>& array)
    {
        auto baseTypeName = GetArchivedTypeName<ValueType>();
        std::vector<const utilities::IArchivable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(&item);
        }
        ArchiveArray(name, baseTypeName, tmpArray);
    }

    // Vector of serializable pointers
    template <typename ValueType, IsIArchivable<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, const std::vector<const ValueType*>& array)
    {
        auto baseTypeName = GetArchivedTypeName<ValueType>();
        std::vector<const utilities::IArchivable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(item);
        }
        ArchiveArray(name, baseTypeName, tmpArray);
    }

    //
    // PropertyUnarchiver class
    //
    template <typename ValueType>
    void Unarchiver::PropertyUnarchiver::operator>>(ValueType&& value)
    {
        _unarchiver.Unarchive(_propertyName.c_str(), value);
    }

    //
    // OptionalPropertyUnarchiver class
    //
    template <typename DefaultValueType>
    Unarchiver::OptionalPropertyUnarchiver<DefaultValueType>::OptionalPropertyUnarchiver(Unarchiver& archiver, const std::string& name, const DefaultValueType& defaultValue)
    : _unarchiver(archiver), _propertyName(name), _defaultValue(defaultValue)
    {};

    template <typename DefaultValueType>
    template <typename ValueType>
    void Unarchiver::OptionalPropertyUnarchiver<DefaultValueType>::operator>>(ValueType&& value)
    {
        if (_unarchiver.HasNextPropertyName(_propertyName))
        {
            _unarchiver.Unarchive(_propertyName.c_str(), value);
        }
        else
        {
            value = _defaultValue;
        }
    }

    template <>
    template <typename ValueType>
    void Unarchiver::OptionalPropertyUnarchiver<Unarchiver::NoDefault>::operator>>(ValueType&& value)
    {
        if (_unarchiver.HasNextPropertyName(_propertyName))
        {
            _unarchiver.Unarchive(_propertyName.c_str(), value);
        }
    }

    //
    // Unarchiver class
    //
    template <typename ValueType>
    void Unarchiver::Unarchive(ValueType&& value)
    {
        Unarchive("", value);
    }

    template <typename ValueType>
    void Unarchiver::operator>>(ValueType&& value)
    {
        Unarchive(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Unarchiver::Unarchive(const char* name, ValueType&& value)
    {
        UnarchiveItem(name, value);
    }

    // STYLE: inline to keep next to its sibling overload
    inline Unarchiver::OptionalPropertyUnarchiver<Unarchiver::NoDefault> Unarchiver::OptionalProperty(const std::string& name)
    {
        return OptionalPropertyUnarchiver<Unarchiver::NoDefault>(*this, name, {});
    }

    template <typename DefaultValueType>
    Unarchiver::OptionalPropertyUnarchiver<DefaultValueType> Unarchiver::OptionalProperty(const std::string& name, const DefaultValueType& defaultValue)
    {
        return OptionalPropertyUnarchiver<DefaultValueType>(*this, name, defaultValue);
    }

    // types:
    // Fundamental
    // IArchivable (& ArchivedAsPrimitive)
    // Array
    template <typename ValueType, IsNotVector<ValueType> concept1, IsNotArchivedAsPrimitive<ValueType> concept2>
    void Unarchiver::UnarchiveItem(const char* name, ValueType&& value)
    {
        UnarchiveValue(name, value);
    }

    template <typename ValueType, IsNotVector<ValueType> concept1, IsArchivedAsPrimitive<ValueType> concept2>
    void Unarchiver::UnarchiveItem(const char* name, ValueType&& value)
    {
        UnarchiveObjectAsPrimitive(name, value);
    }

    // unique pointer to non-archivable type
    template <typename ValueType, IsNotArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto ptr = std::make_unique<ValueType>();
        UnarchiveValue(name, *ptr);
        value = std::move(ptr);
    }

    // unique pointer to standard archivable object
    template <typename ValueType, IsStandardArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto baseTypeName = GetArchivedTypeName<ValueType>();
        auto objInfo = BeginUnarchiveObject(name, baseTypeName);
        _objectInfo.push_back(objInfo);
        auto encodedTypeName = objInfo.type;
        std::unique_ptr<ValueType> newPtr = GetContext().GetTypeFactory().Construct<ValueType>(encodedTypeName);
        UnarchiveObject(name, *newPtr);
        EndUnarchiveObject(name, encodedTypeName);
        // TODO: assert back of _objectInfo == objInfo
        _objectInfo.pop_back();
        value = std::move(newPtr);
    }

    // pointer to serializable-as-primitive type
    template <typename ValueType, IsArchivedAsPrimitive<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto baseTypeName = GetArchivedTypeName<ValueType>();
        std::unique_ptr<ValueType> newPtr = std::make_unique<ValueType>();
        UnarchiveObject(name, *newPtr);
        value = std::move(newPtr);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        UnarchiveArray(name, arr);
    }

    // Vector of strings
    inline void Unarchiver::UnarchiveItem(const char* name, std::vector<std::string>& arr)
    {
        arr.clear();
        UnarchiveArray(name, arr);
    }

    // Vector of serializable objects
    template <typename ValueType, IsIArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        auto typeName = GetArchivedTypeName<ValueType>();
        BeginUnarchiveArray(name, typeName);
        while (true)
        {
            auto good = BeginUnarchiveArrayItem(typeName);
            if (!good)
            {
                break;
            }
            ValueType value;
            Unarchive(value);
            arr.push_back(value);
            EndUnarchiveArrayItem(typeName);
        }
        EndUnarchiveArray(name, typeName);
    }

    // Vector of unique pointers to serializable objects
    template <typename ValueType, IsIArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<std::unique_ptr<ValueType>>& arr)
    {
        arr.clear();
        auto typeName = GetArchivedTypeName<ValueType>();
        BeginUnarchiveArray(name, typeName);
        while (true)
        {
            auto good = BeginUnarchiveArrayItem(typeName);
            if (!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Unarchive(newPtr);
            arr.push_back(std::move(newPtr));
            EndUnarchiveArrayItem(typeName);
        }
        EndUnarchiveArray(name, typeName);
    }

    // Vector of raw pointers to serializable objects
    template <typename ValueType, IsIArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<const ValueType*>& arr)
    {
        arr.clear();
        auto typeName = GetArchivedTypeName<ValueType>();
        BeginUnarchiveArray(name, typeName);
        while (true)
        {
            auto good = BeginUnarchiveArrayItem(typeName);
            if (!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Unarchive(newPtr);
            arr.push_back(newPtr.release());
            EndUnarchiveArrayItem(typeName);
        }
        EndUnarchiveArray(name, typeName);
    }

    //
    // Utility classes
    //

    template <typename ValueType>
    EnsureMaxPrecision<ValueType>::EnsureMaxPrecision(std::ostream& out) :
        _flags(out.flags()), _precision(out.precision()), _out(out)
    {
        _out.precision(std::numeric_limits<ValueType>::digits10 + 1);
    }

    template <typename ValueType>
    EnsureMaxPrecision<ValueType>::~EnsureMaxPrecision()
    {
        _out.flags(_flags);
        _out.precision(_precision);
    }

    //
    // Utility functions
    //
    namespace ArchiverImpl
    {
        template <typename T>
        static std::string GetTypeName(...)
        {
            return TypeName<T>::GetName();
        }

        template <typename T, IsIntegral<T> concept = true>
        static std::string GetTypeName(bool)
        {
            return "int";
        }

        template <typename T, IsFloatingPoint<T> concept = true>
        static std::string GetTypeName(bool)
        {
            return "float";
        }
    }

    template <typename T>
    std::string GetArchivedTypeName()
    {
        return ArchiverImpl::GetTypeName<T>(true);
    }
    
    template <typename T>
    std::string GetArchivedTypeName(const T& value)
    {
        return value.GetRuntimeTypeName();
    }
}
}
