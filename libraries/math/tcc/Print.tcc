////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Print.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace math
{
    template <typename ElementType, VectorOrientation Orientation>
    void Print(const ConstVectorReference<ElementType, Orientation>& v, std::ostream& os)
    {
        os << "{ " << v[0];
        for (size_t i = 1; i < v.Size(); ++i)
        {
            os << ', ' << v[i];
        }
        os << " }";
    }

    template <typename ElementType, MatrixLayout Layout>
    void Print(const ConstMatrixReference<ElementType, Layout>& M, std::ostream& os)
    {
        os << "{\n";
        for (size_t i = 0; i < M.NumRows(); ++i)
        {
            os << "\t{ " << M(i, 0);
            for (size_t j = 1; j < M.NumColumns(); ++j)
            {
                os << ", " << M(i, j);
            }
            os << " }\n";
        }
        os << '}';
    }
}
}