#include "serial.hpp"


void add(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	for (int i = 0; i < c->size; ++i)
		for (int j = 0; j < c->size; ++j)
			c->at(i, j) = a->at(i + a_row, j + a_col) + b->at(i + b_row, j + b_col);
}

void sub(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	for (int i = 0; i < c->size; ++i)
		for (int j = 0; j < c->size; ++j)
			c->at(i, j) = a->at(i + a_row, j + a_col) - b->at(i + b_row, j + b_col);
}

void add2(Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	for (int i = 0; i < b->size; ++i)
		for (int j = 0; j < b->size; ++j)
			a->at(i + a_row, j + a_col) += b->at(i + b_row, j + b_col);
}

void sub2(Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	for (int i = 0; i < b->size; ++i)
		for (int j = 0; j < b->size; ++j)
			a->at(i + a_row, j + a_col) -= b->at(i + b_row, j + b_col);
}


void mul(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	int size = c->size;


	if (size == 2) {
		int s1 = b->at(b_row, b_col + 1) - b->at(b_row + 1, b_col + 1);
		int s2 = a->at(a_row, a_col) + a->at(a_row, a_col + 1);
		int s3 = a->at(a_row + 1, a_col) + a->at(a_row + 1, a_col + 1);
		int s4 = b->at(b_row + 1, b_col) - b->at(b_row, b_col);
		int s5 = a->at(a_row, a_col) + a->at(a_row + 1, a_col + 1);
		int s6 = b->at(b_row, b_col) + b->at(b_row + 1, b_col + 1);
		int s7 = a->at(a_row, a_col + 1) - a->at(a_row + 1, a_col + 1);
		int s8 = b->at(b_row + 1, b_col) + b->at(b_row + 1, b_col + 1);
		int s9 = a->at(a_row, a_col) - a->at(a_row + 1, a_col);
		int s10 = b->at(b_row, b_col) + b->at(b_row, b_col + 1);

		int p1 = a->at(a_row, a_col) * s1;
		int p2 = s2 * b->at(b_row + 1, b_col + 1);
		int p3 = s3 * b->at(b_row, b_col);
		int p4 = a->at(a_row + 1, a_col + 1) * s4;
		int p5 = s5 * s6;
		int p6 = s7 * s8;
		int p7 = s9 * s10;

		c->at(0, 0) = p5 + p4 - p2 + p6;
		c->at(0, 1) = p1 + p2;
		c->at(1, 0) = p3 + p4;
		c->at(1, 1) = p5 + p1 - p3 - p7;
	}
	else {
		int half = size / 2;

		Matrix** s = new Matrix*[10];
		for (int i = 0; i < 10; ++i)
			s[i] = new Matrix(half);

		sub(s[0], b, 0, half, b, half, half);
		add(s[1], a, 0, 0, a, 0, half);
		add(s[2], a, half, 0, a, half, half);
		sub(s[3], b, half, 0, b, 0, 0);
		add(s[4], a, 0, 0, a, half, half);
		add(s[5], b, 0, 0, b, half, half);
		sub(s[6], a, 0, half, a, half, half);
		add(s[7], b, half, 0, b, half, half);
		sub(s[8], a, 0, 0, a, half, 0);
		add(s[9], b, 0, 0, b, 0, half);

		Matrix** p = new Matrix*[7];
		for (int i = 0; i < 7; ++i)
			p[i] = new Matrix(half);


		mul(p[0], a, 0, 0, s[0], 0, 0);
		mul(p[1], s[1], 0, 0, b, half, half);
		mul(p[2], s[2], 0, 0, b, 0, 0);
		mul(p[3], a, half, half, s[3], 0, 0);
		mul(p[4], s[4], 0, 0, s[5], 0, 0);
		mul(p[5], s[6], 0, 0, s[7], 0, 0);
		mul(p[6], s[8], 0, 0, s[9], 0, 0);

		for (int i = 0; i < 10; ++i)
			delete s[i];
		delete[] s;

		add2(c, 0, 0, p[4], 0, 0);
		add2(c, 0, 0, p[3], 0, 0);
		sub2(c, 0, 0, p[1], 0, 0);
		add2(c, 0, 0, p[5], 0, 0);

		add2(c, 0, half, p[0], 0, 0);
		add2(c, 0, half, p[1], 0, 0);

		add2(c, half, 0, p[2], 0, 0);
		add2(c, half, 0, p[3], 0, 0);

		add2(c, half, half, p[4], 0, 0);
		add2(c, half, half, p[0], 0, 0);
		sub2(c, half, half, p[2], 0, 0);
		sub2(c, half, half, p[6], 0, 0);

		for (int i = 0; i < 7; ++i)
			delete p[i];
		delete[] p;
	}
}
