//// segmented_vector.cpp
//
//#include "segmented_vector.h"
//#include "ZeroDataVector.h"
//
//using std::move;
//using std::make_unique;
//
//
//#include <cassert>
//
//namespace linear
//{
//    segmented_vector<auto_vector>::segmented_vector() : segmented_vector_base<auto_vector>()
//    {}
//
//    void segmented_vector<auto_vector>::PushBack(uint index, double value)
//    {
//        uint segment_index = index / SEGMENT_SIZE;
//        uint internal_index = index % SEGMENT_SIZE;
//        uint last_segment = _segments.Size()-1;
//
//        assert(segment_index >= last_segment);
//
//        // case 1: no allocation needed
//        if(segment_index == last_segment)
//        {
//            _segments[last_segment].push_back(internal_index, value);
//            return;
//        }
//
//        // case 2: allocate more segments
//
//        // perhaps recycle a dense vector
//        auto recycled_dense_vector = _segments[last_segment].auto_type();
//
//        // pad with ZeroDataVector segments as needed
//        for(; last_segment<segment_index-1; ++last_segment)
//        {
//            _segments.emplace_back(make_unique<ZeroDataVector>(SEGMENT_SIZE));
//        }
//
//        // Add the a dense segment at the end
//        _segments.emplace_back(move(recycled_dense_vector));
//
//        // set the value
//        _segments[segment_index].push_back(internal_index, value);
//    }
//
//
//    unique_ptr<FloatDataVector> segmented_vector<auto_vector>::auto_type()
//    {
//        return _segments[_segments.Size()-1].auto_type();
//    }
//}