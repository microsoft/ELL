////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueTensor.tcc (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace value
{

    template <typename T>
    Tensor::Tensor(const std::vector<std::vector<std::vector<T>>>& data)
    {
        using namespace utilities;

        int numRows = static_cast<int>(data.size());
        if (numRows <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        int numColumns = static_cast<int>(data[0].size());
        if (numColumns <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        int numChannels = static_cast<int>(data[0][0].size());
        if (numChannels <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        std::vector<T> coalesced(numRows * numColumns * numChannels);
        auto it = coalesced.begin();
        for (const auto& row : data)
        {
            if (static_cast<int>(row.size()) != numColumns)
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }

            for (const auto& column : row)
            {
                if (static_cast<int>(column.size()) != numChannels)
                {
                    throw InputException(InputExceptionErrors::invalidSize);
                }

                it = std::copy(column.begin(), column.end(), it);
            }
        }

        _value = Value(coalesced, MemoryLayout({ numRows, numColumns, numChannels }));
    }

} // namespace value
} // namespace ell