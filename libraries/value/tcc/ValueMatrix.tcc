////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueMatrix.tcc (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace value
{

    template <typename T>
    Matrix::Matrix(const std::vector<std::vector<T>>& data)
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

        std::vector<T> coalesced(numRows * numColumns);
        auto it = coalesced.begin();
        for (const auto& row : data)
        {
            if (static_cast<int>(row.size()) != numColumns)
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }

            it = std::copy(row.begin(), row.end(), it);
        }

        _value = Value(coalesced, MemoryLayout({ numRows, numColumns }));
    }

} // namespace value
} // namespace ell