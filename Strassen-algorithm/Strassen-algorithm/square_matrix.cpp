#include "square_matrix.hpp"

#include <iostream>


bool SquareMatrix::parallel = false;


SquareMatrix::SquareMatrix(int size, int d)
	: n(size)
{
	m = new int[n * n];

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			m[i * n + j] = d;
}

SquareMatrix::SquareMatrix(const int* values, int size) 
{
	n = sqrt(size);
	m = new int[size];

	for (int i = 0; i < size; ++i)
		m[i] = values[i];
}

SquareMatrix::~SquareMatrix()
{
	delete m;
}

int SquareMatrix::size() const 
{
	return n;
}

void SquareMatrix::print() const 
{
	std::cout << std::endl;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			std::cout << m[i * n + j] << "\t";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;
}

SquareMatrix& SquareMatrix::submatrix(int row, int col, int size) const 
{
	if (row + size > n || col + size > n)
		throw std::runtime_error("Error: indicies out of range.");

	SquareMatrix* sm = new SquareMatrix(size);

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			(*sm)[i * size + j] = m[(i + row) * n + (j + col)];

	return *sm;
}

void SquareMatrix::set_range(int row, int col, SquareMatrix& values) 
{
	int size = values.size();
	
	if (row + size > n || col + size > n)
		throw std::runtime_error("Error: indicies out of range.");

	
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			m[(i + row) * n + (j + col)] = values[i * size + j];
}

int& SquareMatrix::operator[](int index) {
	return m[index];
}

int SquareMatrix::operator[](int index) const {
	return m[index];
}

SquareMatrix& operator+(const SquareMatrix& a, const SquareMatrix& b)
{
	if (a.n != b.n)
		throw std::runtime_error("Error: matrices must be of the same size.");

	int size = a.n; 
	SquareMatrix* c = new SquareMatrix(size);

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			(*c)[i * size + j] = a[i * size + j] + b[i * size + j];

	return *c;
}

SquareMatrix& operator*(const SquareMatrix& a, const SquareMatrix& b)
{
	if (SquareMatrix::parallel)
		return parallel_strassen(a, b);
	return strassen(a, b);
}

SquareMatrix& strassen(const SquareMatrix& a, const SquareMatrix& b) 
{
	
	if (a.size() == 2) {
		int n = a.size();
		int p1 = a[0] * (b[1] - b[3]);
		int p2 = (a[0] + a[1]) * b[3];
		int p3 = (a[2] + a[3]) * b[0];
		int p4 = a[3] * (b[2] - b[0]);
		int p5 = (a[0] + a[3]) * (b[0] + b[3]);
		int p6 = (a[1] - a[3]) * (b[2] + b[3]);
		int p7 = (a[0] - a[2]) * (b[0] + b[1]);
	
		SquareMatrix* c = new SquareMatrix(2);
		
		(*c)[0] = p5 + p4 - p2 + p6;
		(*c)[1] = p1 + p2;
		(*c)[2] = p3 + p4;
		(*c)[3] = p1 + p5 - p3 - p7;
		return *c;
	}

	int half = a.size() / 2;
	int size = a.size();

	SquareMatrix a1 = a.submatrix(0, 0, half);
	SquareMatrix a2 = a.submatrix(0, half, half);
	SquareMatrix a3 = a.submatrix(half, 0, half);
	SquareMatrix a4 = a.submatrix(half, half, half);

	SquareMatrix b1 = b.submatrix(0, 0, half);
	SquareMatrix b2 = b.submatrix(0, half, half);
	SquareMatrix b3 = b.submatrix(half, 0, half);
	SquareMatrix b4 = b.submatrix(half, half, half);

	SquareMatrix* c = new SquareMatrix(size);
	c->set_range(0, 0, a1 * b1 + a2 * b3);
	c->set_range(0, half, a1 * b2 + a2 * b4);
	c->set_range(half, 0, a3 * b1 + a4 * b3);
	c->set_range(half, half, a3 * b2 + a4 * b4);

	return *c;
}

SquareMatrix& parallel_strassen(const SquareMatrix& a, const SquareMatrix& b) 
{
	return SquareMatrix(2);
}