////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
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
