#include "square_matrix.hpp"


int main() {
	int v1[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

	SquareMatrix a(v1, 16);
	a.print();

	int v2[] = { 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5 };

	SquareMatrix b(v2, 16);
	b.print();

	SquareMatrix c = a * b;
	c.print();
	int i = 0;
	return 0;
}