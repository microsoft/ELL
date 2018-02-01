////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilter.tcc (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace dsp
{
    template <typename ValueType>
    IIRFilter<ValueType>::IIRFilter(std::vector<ValueType> b, std::vector<ValueType> a)
        : _previousInput(b.size()), _previousOutput(a.size()), _b(b), _a(a)
    {
        Reset();
    }

    template <typename ValueType>
    ValueType IIRFilter<ValueType>::FilterSample(ValueType x)
    {
        _previousInput.Append(x);
        ValueType output = 0;
        assert(_b.size() == _previousInput.Size());
        assert(_a.size() == _previousOutput.Size());
        for (size_t index = 0; index < _b.size(); index++)
        {
            output += _b[index] * _previousInput[static_cast<int>(index)];
        }

        for (size_t index = 0; index < _a.size(); index++)
        {
            output -= _a[index] * _previousOutput[static_cast<int>(index)];
        }

        _previousOutput.Append(output);
        return output;
    }

    template <typename ValueType>
    std::vector<ValueType> IIRFilter<ValueType>::FilterSamples(const std::vector<ValueType>& x)
    {
        std::vector<ValueType> result(x.size());
        std::transform(x.begin(), x.end(), result.begin(), [this](ValueType elem) {
            return FilterSample(elem);
        });
        return result;
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::Reset()
    {
        _previousInput.Fill(0);
        _previousOutput.Fill(0);
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["b"] << _b;
        archiver["a"] << _a;
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["b"] >> _b;
        archiver["a"] >> _a;
    }

}
}
