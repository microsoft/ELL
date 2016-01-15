// linear_test.cpp

#include "DoubleVector.h"
#include "operators.h"
#include "DenseDataVector.h"
#include "DoubleMatrix.h"
#include "IntegerList.h"
#include "CompressedIntegerList.h"
#include "SparseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "RowMatrix.h"
#include "BiMatrix.h"
#include "DataVectorBuilder.h"
#include <memory>
#include <algorithm>
#include <string>

using namespace linear;
using namespace dataset;
using namespace std;


template <typename VectorType>
void fill_vector_binary_random(VectorType& v, int size, double p=0.5)
{
    bernoulli_distribution dist(p);
    default_random_engine rng(123456);
    for (int i = 0; i < size; ++i)
    {
        v.PushBack(i, dist(rng));
    }
}

template <typename MatrixType>
void fill_matrix_binary_random(MatrixType& M, int rows, int cols, bool lower_triangle, bool upper_triangle, double p = 0.5)
{
    bernoulli_distribution dist(p);
    default_random_engine rng(1234);
    for(int i = 0; i < rows; ++i)
    {
        if(lower_triangle)
        {
            for(int j = 0; j < cols && j < i; ++j)
            {
                M.Set(i, j, dist(rng));
            }
        }

        if(i < cols)
        {
            M.Set(i, i, dist(rng));
        }

        if(upper_triangle)
        {
            for(int j = i+1; j < cols; ++j)
            {
                M.Set(i, j, dist(rng));
            }
        }
    }
}

template <typename MatrixType>
void fill_matrix_binary_random2(MatrixType& M, int rows, int cols)
{
    using RowType = typename MatrixType::RowType;
    uniform_int_distribution<int> dist(0, 1);
    default_random_engine rng(1234);
    for (int i = 0; i < rows; ++i)
    {
        RowType row;
        for (int j = 0; j < cols; ++j)
        {
            row.PushBack(j, dist(rng));
        }
        M.PushBackRow(move(row));
    }
}

DoubleVector get_rand_vector(int size)
{
    DoubleVector y(size);
    uniform_real_distribution<double> dist(-1, 1);
    default_random_engine rng(1234);
    auto generator = bind(dist, rng);
    generate(y.begin(), y.end(), generator);
    return move(y);
}

void vector_test1(IDataVector& v, const DoubleVector& x)
{
    cout << "\n\n======================================================================================================\n";
    cout << "Vector Test 1 on " << typeid(v).name() << endl;
    cout << "Specified Dim:\t" << v.Size() << endl;
    cout << "Contents:\n" << v << endl;
    cout << "Norm2:\t" << v.Norm2() << endl;

    DoubleVector w(v.Size());
    v.AddTo(w);
    cout << "Dot:\t" << v.Dot(w) << endl;
}


void matrix_test1(IMatrix& M, const DoubleVector& x, const DoubleVector& y, bool Gevm, string comment = "")
{
    cout << "\n\n======================================================================================================\n";
    cout << "Matrix Test 1 on " << typeid(M).name() << "(" << comment << ")\n";
    cout << "Dimensions:\t" << M.NumRows() << " x " << M.NumColumns() << endl;
    cout << "Contents:\n" << M << endl;

    DoubleVector w(M.NumRows());

    M.Gemv(x, w, 1.0, 0);
    cout << "gemv10:\t" << w << endl;

    M.Gemv(x, w, 1.0, 1.0);
    cout << "gemv11:\t" << w << endl;

    M.Gemv(x, w, 1.0, 2.0);
    cout << "gemv12:\t" << w << endl;

    M.Gemv(x, w, 2.0, 0);
    cout << "gemv20:\t" << w << endl;

    M.Gemv(x, w, 2.0, 1.0);
    cout << "gemv21:\t" << w << endl;

    M.Gemv(x, w, 2.0, 2.0);
    cout << "gemv22:\t" << w << endl;

    if(Gevm)
    {
        DoubleVector z(M.NumColumns());

        M.Gevm(y, z, 1.0, 0);
        cout << "gevm10:\t" << w << endl;

        M.Gevm(y, z, 1.0, 1.0);
        cout << "gevm11:\t" << w << endl;

        M.Gevm(y, z, 1.0, 2.0);
        cout << "gevm12:\t" << w << endl;

        M.Gevm(y, z, 2.0, 0);
        cout << "gevm20:\t" << w << endl;

        M.Gevm(y, z, 2.0, 1.0);
        cout << "gevm21:\t" << w << endl;

        M.Gevm(y, z, 2.0, 2.0);
        cout << "gevm22:\t" << w << endl;
    }
}

int main(int argc, char* argv[])
{

    try
    {
        // get a couple of dense vector<doubles>'s
        auto x = get_rand_vector(9);
        auto y = get_rand_vector(8);

        // vector tests
        DoubleDataVector v1;
        fill_vector_binary_random(v1, 21);
        vector_test1(v1, x);

        UncompressedSparseBinaryDataVector v2;
        fill_vector_binary_random(v2, 21);
        vector_test1(v2, x);

        SparseBinaryDataVector v3;
        fill_vector_binary_random(v3, 21);
        vector_test1(v3, x);

        SparseDoubleDataVector v4;
        fill_vector_binary_random(v4, 21);
        vector_test1(v4, x);

        SparseFloatDataVector v6;
        fill_vector_binary_random(v6, 21);
        vector_test1(v6, x);

        SparseShortDataVector v8;
        fill_vector_binary_random(v8, 21);
        vector_test1(v8, x);

        //DataVectorBuilder builder;
        //fill_vector_binary_random(builder, 21, 0.5);
        //auto v9 = builder.GetVectorAndReset();
        //vector_test1(*v9, x);

        //fill_vector_binary_random(builder, 21, 0.07);
        //auto v10 = builder.GetVectorAndReset();
        //vector_test1(*v10, x);

        //fill_vector_binary_random(builder, 21, 0);
        //auto v11 = builder.GetVectorAndReset();
        //vector_test1(*v11, x);

        //fill_vector_binary_random(builder, 21, 1);
        //auto v12 = builder.GetVectorAndReset();
        //vector_test1(*v12, x);

        // matrix tests
        DoubleMatrix<MatrixStructure::column> M1(8, 9);
        fill_matrix_binary_random(M1, 8, 9, true, true);
        matrix_test1(M1, x, y, true, "column");

        DoubleMatrix<MatrixStructure::row> M4(8, 9);
        fill_matrix_binary_random(M4, 8, 9, true, true);
        matrix_test1(M4, x, y, true, "row");

        DoubleMatrix<MatrixStructure::column_square> M2(9);
        fill_matrix_binary_random(M2, 8, 9, true, true);
        matrix_test1(M2, x, y, true, "column square");

        DoubleMatrix<MatrixStructure::row_square> M5(9);
        fill_matrix_binary_random(M5, 8, 9, true, true);
        matrix_test1(M5, x, y, true, "row square");

        DoubleMatrix<MatrixStructure::row_square_uptriangular> M6(9);
        fill_matrix_binary_random(M6, 8, 9, false, true);
        matrix_test1(M6, x, y, true, "row square upper triangular");

        DoubleMatrix<MatrixStructure::diagonal> M7(9);
        fill_matrix_binary_random(M7, 8, 9, false, false);
        matrix_test1(M7, x, y, true, "diagonal");

        RowMatrix<UncompressedSparseBinaryDataVector> M8;
        fill_matrix_binary_random2(M8, 8, 9);
        matrix_test1(M8, x, y, false);

        RowMatrix<SparseBinaryDataVector> M9;
        fill_matrix_binary_random2(M9, 8, 9);
        matrix_test1(M9, x, y, false);


/*
        RowMatrix<segmented_vector<FloatDataVector>> M11;
        fill_matrix_binary_random2(M11, 8, 9);
        matrix_test1(M11, x, y, false);

        RowMatrix<segmented_vector<auto_vector>> M12;
        fill_matrix_binary_random2(M12, 8, 9);
        M12.auto_type();
        matrix_test1(M12, x, y, false);
*/
        int foo = 5;

        //BiMatrix<RowMatrix<SparseBinaryDataVector<CompressedIntegerList>>> M13;
        //fill_matrix_binary_random(M13, 8, 9, true, true);
        //matrix_test1(M13, x, y, true);

        // vector tests
    }
    catch (...)
    {
        int x = 5;
    }

}




