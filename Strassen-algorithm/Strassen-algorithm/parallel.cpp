#include "parallel.hpp"

using namespace tbb;

Task::Task(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col)
	: size(size), my_c(c), my_a_row(m_row), my_a_col(m_col), my_a(m), my_b_row(n_row), my_b_col(n_col), my_b(n)
{}
Task::~Task() {}

task* Task::execute() {
	Matrix* a = my_a;
	int a_row = my_a_row;
	int a_col = my_a_col;

	Matrix* b = my_b;
	int b_row = my_b_row;
	int b_col = my_b_col;

	Matrix* c = my_c;
	int size = c->size;

	if (size <= GRAIN) {
		mul(c, a, a_row, a_col, b, b_row, b_col);
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

		for (int i = 0; i < 7; ++i)
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

void pmul(Matrix* c, Matrix* m, int m_row, int m_col, Matrix* n, int n_row, int n_col) {

	empty_task* et = new(task::allocate_root()) empty_task();
	et->set_ref_count(2);

	Task* t = new(et->allocate_child()) Task(c, m, m_row, m_col, n, n_row, n_col);
	et->spawn(*t);

	et->wait_for_all();

	et->destroy(*et);
}