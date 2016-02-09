////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     BiMatrix.tcc (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace linear
{

    template<typename RowMatrixType>
    const typename BiMatrix<RowMatrixType>::RowType& BiMatrix<RowMatrixType>::GetColumn(uint64 index) const
    {
        return _transp.GetRow(index);
    }

    template<typename RowMatrixType>
    void BiMatrix<RowMatrixType>::Gevm(const double* p_x, double* p_y, double alpha, double beta) const
    {
        _transp.Gemv(p_x, p_y, alpha, beta);
    }
}
