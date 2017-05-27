#include <iostream>
#include <fstream>
#include <chrono>
#include "tbb/tbb.h"

#include "matrix.hpp"
#include "serial.hpp"
#include "parallel.hpp"

using namespace std;
using namespace chrono;
/*
int main() {

	ifstream o("../../test");
	Matrix a, b;
	o >> a;
	o >> b;

	cout << a;
	cout << b;


	return 0;
}*/


void main() {

	srand(time(NULL));
	
	int size = 512;
	int* a = new int[size*size];
	int* b = new int[size*size];

	for (int i = 0; i < size*size; ++i) {
		a[i] = rand() % 100;
		b[i] = rand() % 100;
	}
	
	Matrix* m = new Matrix(size);
	m->values = a;

	Matrix* n = new Matrix(size);
	n->values = b;

	Matrix* o1 = new Matrix(size);

	auto serial_start = high_resolution_clock::now();
	mul(o1, m, 0, 0, n, 0, 0);
	auto serial_end = high_resolution_clock::now();
	auto serial_time = duration_cast<microseconds>(serial_end - serial_start).count();
	cout << "Serial time: " << serial_time << endl;
	
	Matrix* o2 = new Matrix(size);

	auto parallel_start = high_resolution_clock::now();
	pmul(o2, m, 0, 0, n, 0, 0);
	auto parallel_end = high_resolution_clock::now();
	auto parallel_time = duration_cast<microseconds>(parallel_end - parallel_start).count();
	cout << "Parallel time: " << parallel_time << endl;

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
		cout << "Speed-up: " << (float)serial_time / parallel_time << endl;
	}
	delete m, n, o1, o2;
}