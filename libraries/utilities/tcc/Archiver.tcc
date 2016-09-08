////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Archiver.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, const std::vector<ValueType>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
        std::vector<const utilities::IArchivable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(&item);
        }
        ArchiveArray(name, baseTypeName, tmpArray);
    }

    // Vector of serializable pointers
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Archiver::ArchiveItem(const char* name, const std::vector<const ValueType*>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
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

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, ValueType&& value)
    {
        UnarchiveValue(name, value);
    }

    // pointer to non-serializable type
    template <typename ValueType, IsNotArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto ptr = std::make_unique<ValueType>();
        UnarchiveValue(name, *ptr);
        value = std::move(ptr);
    }

    // pointer to serializable type
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto baseTypeName = ValueType::GetTypeName();
        auto encodedTypeName = BeginUnarchiveObject(name, baseTypeName);

        std::unique_ptr<ValueType> newPtr = GetContext().GetTypeFactory().Construct<ValueType>(encodedTypeName);
        UnarchiveObject(name, *newPtr);
        EndUnarchiveObject(name, encodedTypeName);
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
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
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
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<std::unique_ptr<ValueType>>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
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
    template <typename ValueType, IsArchivable<ValueType> concept>
    void Unarchiver::UnarchiveItem(const char* name, std::vector<const ValueType*>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
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
}
