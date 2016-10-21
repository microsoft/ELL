////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ExampleIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace data
{
    template <typename ExampleType>
    ExampleIterator<ExampleType>::ExampleIterator(std::unique_ptr<IExampleIterator<ExampleType>>&& iterator)
        : _iterator(std::move(iterator))
    {
    }
}
}
