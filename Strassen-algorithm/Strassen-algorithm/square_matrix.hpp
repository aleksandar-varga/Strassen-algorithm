#pragma once



class SquareMatrix
{

	int* m;
	int n;

public:
	static bool parallel;

	SquareMatrix(int size, int dvalue=0);
	SquareMatrix(const int* values, int size);
	~SquareMatrix();

	int size() const;
	void print() const;

	SquareMatrix& submatrix(int row, int col, int size) const;
	void set_range(int row, int col, SquareMatrix& values);
	
	int& operator[](int index);
	int operator[] (int index) const;

	friend SquareMatrix& operator+(const SquareMatrix& a, const SquareMatrix& b);
	friend SquareMatrix& operator*(const SquareMatrix& a, const SquareMatrix& b);
	
};

SquareMatrix& strassen(const SquareMatrix& a, const SquareMatrix& b);
SquareMatrix& parallel_strassen(const SquareMatrix& a, const SquareMatrix& b);