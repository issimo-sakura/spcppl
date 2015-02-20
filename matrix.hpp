#pragma once

#include <vector>
#include "assert.hpp"
#include "matrix_multiplication.hpp"
#include "identity.hpp"

template <typename T, typename Container>
class Row {
public:
	explicit Row(Container& value): value(value) {

	}

	T& operator [] (size_t index) {
		SPCPPL_ASSERT(index < value.size());
		return value[index];
	}

	const T& operator [] (size_t index) const {
		SPCPPL_ASSERT(index < value.size());
		return value[index];
	}

private:
	Container& value;
};

template <typename T>
class Matrix {
public:
	Matrix(std::size_t n, std::size_t m, const T& value = T()): value(n, std::vector<T>(m, value)) {

	}

	size_t rows() const {
		return value.size();
	}

	size_t columns() const {
		return value[0].size();
	}

	Row<T, std::vector<T>> operator [] (std::size_t index) {
		SPCPPL_ASSERT(index < value.size());
		return Row<T, std::vector<T>>(value[index]);
	}

	Row<const T, const std::vector<T>> operator[] (std::size_t index) const {
		SPCPPL_ASSERT(index < value.size());
		return Row<const T, const std::vector<T>>(value[index]);
	}

	Matrix& operator *= (const Matrix& rhs) {
		SPCPPL_ASSERT(rows() == columns() && rhs.rows() == rhs.columns());
		return *this = *this * rhs;
	}

	Matrix& operator += (const Matrix& rhs) {
		SPCPPL_ASSERT(rows() == rhs.rows() && columns() == rhs.columns());
		for(size_t i = 0; i < rows(); ++i) {
			for(size_t j = 0; j < columns(); ++j) {
				value[i][j] += rhs.value[i][j];
			}
		}
		return *this;
	}

	Matrix operator - () const {
		Matrix copy = *this;
		for(int i = 0; i < rows(); ++i) {
			for(int j  = 0; j < columns(); ++j) {
				copy[i][j] = -copy[i][j];
			}
		}
		return copy;
	}

	Matrix operator -= (const Matrix& rhs) {
		return *this += -rhs;
	}

	Matrix transposed() const {
		Matrix res(columns(), rows());
		for (std::size_t i = 0; i < rows(); ++i) {
			for(std::size_t j = 0; j < columns(); ++j) {
				res[j][i] = value[i][j];
			}
		}
		return res;
	}

private:
	std::vector<std::vector<T>> value;
};


template <typename T>
Matrix<T> operator * (const Matrix<T>& lhs, const Matrix<T>& rhs) {
	SPCPPL_ASSERT(lhs.columns() == rhs.rows());
	Matrix<T> res{lhs.rows(), rhs.columns()};
	impl__matrix_multiplication(lhs, rhs, res);
	return res;
}

template <typename T, typename C1, typename C2>
Matrix<T> operator + (Matrix<T> lhs, const Matrix<T>& rhs) {
	Matrix<T> copy = std::move(lhs);
	return copy += rhs;
}

template <typename T, typename C1, typename C2>
Matrix<T> operator - (Matrix<T> lhs, const Matrix<T>& rhs) {
	Matrix<T> copy = std::move(lhs);
	return copy -= rhs;
}

template<typename T>
struct impl__IdentityHelper<Matrix<T>> {
	static Matrix<T> identity(const Matrix<T>& sample) {
		SPCPPL_ASSERT(sample.rows() == sample.columns());
		Matrix<T> res(sample.rows(), sample.rows());
		for (std::size_t i = 0; i < sample.rows(); ++i) {
			res[i][i] = ::identity(sample[0][0]);
		}
		return res;
	}
};