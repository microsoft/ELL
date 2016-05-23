////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StringUtil.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>

namespace features
{
    template <typename T>
    std::string to_string(const std::vector<T>& list)
    {
        std::stringstream resultStream;
        bool isFirst = true;
        for (auto val : list)
        {
            if (!isFirst)
            {
                resultStream << ',';
                isFirst = false;
            }

            resultStream << val;
        }
        return resultStream.str();
    }
}