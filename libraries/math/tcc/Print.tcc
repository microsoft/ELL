////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Print.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace math
{
    template <typename ElementType, VectorOrientation orientation>
    void Print(const ConstVectorReference<ElementType, orientation>& v, std::ostream& os, size_t indent)
    {
        os << std::string(indent, '\t');
        os << "{ " << v[0];
        for (size_t i = 1; i < v.Size(); ++i)
        {
            os << ", " << v[i];
        }
        os << " }";
    }

    template <typename ElementType, MatrixLayout layout>
    void Print(const ConstMatrixReference<ElementType, layout>& M, std::ostream& os, size_t indent)
    {
        os << std::string(indent, '\t') << "{\n";
        for (size_t i = 0; i < M.NumRows(); ++i)
        {
            Print(M.GetRow(i), os, indent + 1);
            os << std::endl;
        }
        os << std::string(indent, '\t') << "}\n";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(const ConstTensorReference<ElementType, dimension0, dimension1, dimension2>& T, std::ostream& os, size_t indent)
    {
        os << std::string(indent, '\t') << "{\n";
        for (size_t k = 0; k < T.NumChannels(); ++k)
        {
            os << std::string(indent+1, '\t') << "{\n";
            for (size_t i = 0; i < T.NumRows(); ++i)
            {
                os << std::string(indent+2, '\t') << "{" << T(i,0,k);
                for (size_t j = 1; j < T.NumColumns(); ++j)
                {
                    os << ", " << T(i, j, k);
                }
                os << "}\n";
            }
            os << std::string(indent + 1, '\t') << "}\n";
        }
        os << std::string(indent, '\t') << "}\n";
    }
}
}