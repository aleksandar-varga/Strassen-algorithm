#pragma once

#include <fstream>
#include <iostream>

using namespace std;

struct Matrix {

	int* values;
	int size;

	Matrix() {

	}

	Matrix(int size) : size(size) {
		values = new int[size*size];
		memset(values, 0, sizeof(int) * size * size);
	}

	~Matrix() {
		delete[] values;
	}

	inline int& at(int i, int j) {
		return values[i * size + j];
	}

	friend ostream& operator<<(ostream& out, Matrix& m) {
		out << endl;
		for (int i = 0; i < m.size; ++i) {
			for (int j = 0; j <m.size; ++j)
				out << m.values[i * m.size + j] << "\t";
			out << endl;
		}
		out << endl;

		return out;
	}

	friend istream& operator>>(istream& in, Matrix& m) {
		in >> m.size;
		m.values = new int[m.size * m.size];
		
		for(int i =0 ; i < m.size * m.size; ++i)
			in >> m.values[i];

		return in;
	}
};