#pragma once

#include "matrix.hpp"

void add(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col);

void sub(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col);

void add2(Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col);

void sub2(Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col);

void mul(Matrix* c, Matrix* a, int a_row, int a_col, Matrix* b, int b_row, int b_col);