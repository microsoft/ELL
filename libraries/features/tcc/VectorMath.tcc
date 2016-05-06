////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VectorMath.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace vectorOps
{
    // +
    template <typename T>
    features::Vector<T> operator +(const std::vector<T>& a, const std::vector<T>& b)
    {
        std::vector<T> result{ a };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] += b[index];
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator +(std::vector<T>&& a, const std::vector<T>& b)
    {
        std::vector<T> result{ a };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] += b[index];
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator +(const std::vector<T>& a, std::vector<T>&& b)
    {
        std::vector<T> result{ b };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] += a[index];
        }
        return result;
    }

    // -
    template <typename T>
    features::Vector<T> operator -(const std::vector<T>& a, const std::vector<T>& b)
    {
        std::vector<T> result{ a };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] -= b[index];
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator -(std::vector<T>&& a, const std::vector<T>& b)
    {
        std::vector<T> result{ a };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] -= b[index];
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator -(const std::vector<T>& a, std::vector<T>&& b)
    {
        std::vector<T> result{ b };
        auto n = b.size();
        for (size_t index = 0; index < n; ++index)
        {
            result[index] = a[index] - result[index];
        }
        return result;
    }

    // *
    template <typename T>
    features::Vector<T> operator *(const std::vector<T>& a, double scale)
    {
        std::vector<T> result{ a };
        for (auto& x : result)
        {
            x *= scale;
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator *(std::vector<T>&& a, double scale)
    {
        std::vector<T> result{ a };
        for (auto& x : result)
        {
            x *= scale;
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator *(double scale, const std::vector<T>& a)
    {
        return a*scale;
    }

    template <typename T>
    features::Vector<T> operator *(double scale, std::vector<T>&& a)
    {
        return a*scale;
    }

    // /
    template <typename T>
    features::Vector<T> operator /(const std::vector<T>& a, double scale)
    {
        std::vector<T> result{ a };
        for (auto& x : result)
        {
            x /= scale;
        }
        return result;
    }

    template <typename T>
    features::Vector<T> operator /(std::vector<T>&& a, double scale)
    {
        std::vector<T> result{ a };
        for (auto& x : result)
        {
            x /= scale;
        }
        return result;
    }
}
