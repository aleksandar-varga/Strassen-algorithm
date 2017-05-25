#include <iostream>
#include <chrono>
#include "tbb/tbb.h"

#define ROWGRAIN 16
#define COLGRAIN 1024

using namespace std;
using namespace chrono;
using namespace tbb;

class Matrix{
public:
	int* values;
	int size;
	
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

	void print() {
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j)
				cout << values[i * size + j] << "\t";
			cout << endl;
		}
		cout << endl;
	}
};

#pragma region serial

void add(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col) {
	for (int i = 0; i < c->size; ++i)
		for (int j = 0; j < c->size; ++j)
			c->at(i,j) = a->at(i + a_row,j + a_col) + b->at(i + b_row, j + b_col);
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

		c->at(0,0) = p5 + p4 - p2 + p6;
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

#pragma endregion


#pragma region parallel

class Task : public task {

	int size;
	Matrix* my_c;

	int my_a_row, my_a_col;
	Matrix* my_a;

	int my_b_row, my_b_col;
	Matrix* my_b;

public:
	Task(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col)
		: size(size), my_c(c), my_a_row(m_row), my_a_col(m_col), my_a(m), my_b_row(n_row), my_b_col(n_col), my_b(n)
	{}
	~Task() {
	}

	task* execute() {
		Matrix* a = my_a;
		int a_row = my_a_row;
		int a_col = my_a_col;

		Matrix* b = my_b;
		int b_row = my_b_row;
		int b_col = my_b_col;

		Matrix* c = my_c;
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

			set_ref_count(8);

			Task** t = new Task*[7];

			t[0] = new(allocate_child())Task(p[0], a, 0, 0, s[0], 0, 0);
			t[1] = new(allocate_child())Task(p[1], s[1], 0, 0, b, half, half);
			t[2] = new(allocate_child())Task(p[2], s[2], 0, 0, b, 0, 0);
			t[3] = new(allocate_child())Task(p[3], a, half, half, s[3], 0, 0);
			t[4] = new(allocate_child())Task(p[4], s[4], 0, 0, s[5], 0, 0);
			t[5] = new(allocate_child())Task(p[5], s[6], 0, 0, s[7], 0, 0);
			t[6] = new(allocate_child())Task(p[6], s[8], 0, 0, s[9], 0, 0);

			for(int i = 0; i < 7; ++i)
				spawn(*t[i]);
			
			wait_for_all();
		
			delete[] t;

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

		destroy(*this);

		return nullptr;
	}
};

void pmul(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col) {

	empty_task* et = new(task::allocate_root()) empty_task();
	et->set_ref_count(2);

	Task* t = new(et->allocate_child()) Task(c, m, m_row, m_col, n, n_row, n_col);
	et->spawn(*t);

	et->wait_for_all();

	et->destroy(*et);
}

#pragma endregion


void main() {

	srand(rand());
	
	int size = 4096;
	int* a = new int[size*size];
	int* b = new int[size*size];

	for (int i = 0; i < size*size; ++i) {
		a[i] = rand() % 100;
		b[i] = rand() % 100;
	}

	/*
	int size = 4;
	int* a = new int[size*size] {  2,  3,  4,  5,
	6,  7,  8,  9,
	10, 11, 12, 13,
	14, 15, 16, 17 };

	int* b = new int[size*size] { 20, 19, 18, 17,
	16, 28, 14, 13,
	12, 11, 10,  9,
	8,  7,  6,  5 };*/
	
	Matrix* m = new Matrix(size);
	m->values = a;

	Matrix* n = new Matrix(size);
	n->values = b;

	Matrix* o1 = new Matrix(size);

	auto serial_start = high_resolution_clock::now();
	mul(o1, m, 0, 0, n, 0, 0);
	auto serial_end = duration_cast<microseconds>(high_resolution_clock::now() - serial_start).count();
	cout << "Serial time: " << serial_end << endl;
	
	Matrix* o2 = new Matrix(size);

	auto parallel_start = high_resolution_clock::now();
	pmul(o2, m, 0, 0, n, 0, 0);
	auto parallel_end = duration_cast<microseconds>(high_resolution_clock::now() - parallel_start).count();
	cout << "Parallel time: " << parallel_end << endl;

	cout << "Validating.." << endl;
	bool succesful = true;
	for (int i = 0; i < size*size; ++i) {
		if (o1->values[i] != o2->values[i]) {
			cout << "Validation failed." << endl;
			succesful = false;
			break;
		}
	}
	if (succesful) {
		cout << "Validation succesful!" << endl;
		cout << "Speed-up: " << (float)serial_end / parallel_end << endl;
	}
	delete m, n, o1, o2;
}