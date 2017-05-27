#pragma once

#include "tbb/tbb.h"
#include "matrix.hpp"
#include "serial.hpp"

#define GRAIN 64

using namespace tbb;

class Task : public task {

	int size;
	Matrix* my_c;

	int my_a_row, my_a_col;
	Matrix* my_a;

	int my_b_row, my_b_col;
	Matrix* my_b;

public:
	Task(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col);
	~Task();

	task* execute();
};

void pmul(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col);
