//// Coordinatewise.h
//#pragma once
//
//#include "Mapping.h"
//#include "SerializableIndexValue.h"
//
//#include "types.h"
//
//#include <vector>
//using std::vector;
//
//#include <functional>
//using std::function;
//
//namespace mappings
//{
//    class Coordinatewise : public Mapping
//    {
//    public:
//
//        /// A read-only forward iterator for the sparse binary vector.
//        ///
//        class Iterator : public IIndexValueIterator
//        {
//        public:
//
//            /// Default copy ctor
//            ///
//            Iterator(const Iterator&) = default;
//
//            /// Default move ctor
//            ///
//            Iterator(Iterator&&) = default;
//
//            /// \returns True if the iterator is currently pointing to a valid iterate
//            ///
//            bool IsValid() const;
//
//            /// Proceeds to the Next iterate
//            ///
//            void Next();
//
//            /// \returns The current index-value pair
//            ///
//            indexValue Get() const;
//
//        private:
//
//            /// private ctor, can only be called from SparseDataVector class
//            Iterator(const vector<SerializableIndexValue>::const_iterator& begin, const vector<SerializableIndexValue>::const_iterator& end);
//            friend Coordinatewise;
//
//            // members
//            vector<SerializableIndexValue>::const_iterator _begin;
//            vector<SerializableIndexValue>::const_iterator _end;
//        };
//
//        /// Constructs a default Mapping
//        ///
//        Coordinatewise(function<double(double,double)> func);
//
//        /// Converting constructor
//        ///
//        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
//        Coordinatewise(IndexValueIteratorType indexValueIterator, function<double(double, double)> func);
//
//        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
//        ///
//        virtual void Apply(const double* input, double* output) const;
//
//        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
//        ///
//        virtual uint64 GetMinInputDim() const;
//
//        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
//        ///    
//        virtual uint64 GetOutputDim() const;
//
//        /// \Returns a Iterator that points to the beginning of the list.
//        ///
//        Iterator GetIterator() const;
//
//        /// Serializes the Mapping in json format
//        ///
//        virtual void Serialize(JsonSerializer& serializer) const;
//
//        /// Deserializes the Mapping in json format
//        ///
//        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion);
//
//    protected:
//        vector<SerializableIndexValue> _indexValues;
//        function<double(double, double)> _func;
//        static const int _currentVersion = 1;
//    };
//}
//
//#include "../tcc/Coordinatewise.tcc"
//
//
//
//
