// DoubleMatrix.h

#pragma once

#include "IMatrix.h"

#include <vector>
using std::vector;

namespace linear
{
	enum class MatrixStructure { Column, Row, column_square, row_square, row_square_uptriangular, diagonal };

	/// Templated DoubleMatrix class with ElementType and StructureType template arguments
	///
	template<MatrixStructure StructureType = Row>
	class DoubleMatrix
	{};

	/// DoubleMatrix Base class
	class DoubleMatrixBase : public vector<double>, public IMatrix
	{
	public:

		/// Move constructor
		///
		DoubleMatrixBase(DoubleMatrixBase&& other) = default;

		/// Deleted copy constructor
		///
		DoubleMatrixBase(const DoubleMatrixBase& other) = delete;

		/// \returns The number of rows in the matrix
		///
		virtual uint NumRows() const override;

		/// \returns The number of columns in the matrix
		///
		virtual uint NumColumns() const override;

		/// Sets an entry in the matrix
		///
		virtual void set(uint i, uint j, double value = 1.0);

		/// Sets all of the matrix elements to zero
		///
		void Reset();

		/// \returns A reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double& operator()(uint i, uint j) =0;

		/// \returns A Constant reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double operator()(uint i, uint j) const =0;	

		/// Performs a general matrix vector product: y = alpha * M * x + beta * y
		///
		virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

		/// Performs a general vector matrix product: y = alpha * x * M + beta * y
		///
		virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

		/// Prints the matrix to an output stream
		///
		virtual void Print(ostream& os) const override;

	protected:
		DoubleMatrixBase(uint size, uint NumRows, uint NumColumns);

		uint _num_rows;
		uint _num_columns;

	private:
		double RowDot(uint i, const double* p_x) const;
		double ColumnDot(uint j, const double* p_x) const;
	};

	/// Templated DoubleMatrix class specialization for Column-major matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::Column> : public DoubleMatrixBase
	{
	public:
		
		/// Constructs a Column major dense matrix
		///
		DoubleMatrix(uint NumRows, uint NumColumns);

		/// \returns A reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double& operator()(uint i, uint j) override;

		/// \returns A Constant reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double operator()(uint i, uint j) const override;
	};

	/// Templated DoubleMatrix class specialization for Row-major matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::Row> : public DoubleMatrixBase
	{
	public:

		/// Constructs a Row major dense matrix
		///
		DoubleMatrix(uint NumRows, uint NumColumns);

		/// \returns A reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double& operator()(uint i, uint j) override;

		/// \returns A Constant reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double operator()(uint i, uint j) const override;
	};

	/// Templated DoubleMatrix class specialization for Column-major square matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::column_square> : public DoubleMatrix<MatrixStructure::Column>
	{
	public:

		/// Constructs a Column major square dense matrix
		///
		DoubleMatrix(uint dimension);
	};

	/// Templated DoubleMatrix class specialization for Row-major square matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::row_square> : public DoubleMatrix<MatrixStructure::Row>
	{
	public:

		/// Constructs a Row major square dense matrix
		///
		DoubleMatrix(uint dimension);
	};

	/// Templated DoubleMatrix class specialization for Row-major square upper-triangular matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::row_square_uptriangular> : public DoubleMatrixBase
	{
	public:

		/// Constructs a Row major square upper-triangular dense matrix
		///
		DoubleMatrix(uint dimension);

		/// Sets an entry in the matrix
		///
		virtual void set(uint i, uint j, double value = 1.0) override;

		/// \returns A reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double& operator()(uint i, uint j) override;

		/// \returns A Constant reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double operator()(uint i, uint j) const override;

		/// Performs a general matrix vector product: y = alpha * M * x + beta * y
		///
		virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

		/// Performs a general vector matrix product: y = alpha * x * M + beta * y
		///
		virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

	private:
		double _dummy = 0;
	};

	/// Templated DoubleMatrix class specialization for diagonal matrices
	///
	template<>
	class DoubleMatrix<MatrixStructure::diagonal> : public DoubleMatrixBase
	{
	public:

		/// Constructs a Row major square upper-triangular dense matrix
		///
		DoubleMatrix(uint dimension);

		/// Sets an entry in the matrix
		///
		virtual void set(uint i, uint j, double value = 1.0) override;

		/// \returns A reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double& operator()(uint i, uint j) override;

		/// \returns A Constant reference to an element of the matrix, specified by coordinates i,j
		///
		virtual double operator()(uint i, uint j) const override;

		/// Performs a general matrix vector product: y = alpha * M * x + beta * y
		///
		virtual void Gemv(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

		/// Performs a general vector matrix product: y = alpha * x * M + beta * y
		///
		virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

	private:
		double _dummy = 0;
	};
}
