#include <iostream>
#include <chrono>
#include <fstream>
#include <typeinfo>

#include "json.hpp"
#include "tbb/tbb.h"

#include "matrix.hpp"
#include "parallel.hpp"
#include "serial.hpp"
#include "test.hpp"

#define VERIFICATION_TEST
#define PERFORMANCE_TEST
#define RANDOM


using namespace nlohmann;
using namespace std;
using namespace std::chrono;


int main() {
#ifdef VERIFICATION_TEST
	VerificationTestBundle vtb("../../Test/verification-test.json");
	json verification_result = vtb.run();


	std::ofstream vout("../../Test/verification-results.json");

	if (vout.fail()) {
		throw runtime_error("Failed to open output file. Aborting..");
		return 1;
	}

	vout << std::setw(4) << verification_result;
	vout.close();

#endif

#ifdef PERFORMANCE_TEST
	srand(time(NULL));

	PerformanceTestBundle ptb("../../Test/performance-test.json");
	json performance_result = ptb.run();
	
	std::ofstream pout("../../Test/performance-results.json");
	
	if (pout.fail()) {
		throw runtime_error("Failed to open output file. Aborting..");
		return 1;
	}

	pout << std::setw(4) << performance_result;
	pout.close();

#endif

#ifndef VERIFICATION_TEST
#ifndef PERFORMANCE_TEST

	Matrix* m;
	Matrix* n;

#ifdef RANDOM
	std::srand(time(NULL));

	int size;
	std::cout << "Enter matrix size: ";
	std::cin >> size;
	std::cout << std::endl;

	Matrix* m = new Matrix(size);
	Matrix* n = new Matrix(size);

	int* a = m->values();
	int* b = n->values();

	for (int i = 0; i < size*size; ++i) {
		a[i] = rand() % 100;
		b[i] = rand() % 100;
	}

#else
	std::ifstream i("input.json");
	if (i.fail()) {
		cout << "Input file does not exist!" << endl;
		return 1;
	}

	json j;
	i >> j;
	i.close();

	m = new Matrix(j["A"]);
	n = new Matrix(j["B"]);
		

#endif;

	Matrix* c = new Matrix(size);

	auto serial_start = high_resolution_clock::now();
	mul(c, m, 0, 0, n, 0, 0);
	auto serial_end = high_resolution_clock::now();
	auto serial_time = duration_cast<microseconds>(serial_end - serial_start).count();
	std::cout << "Serial time: " << serial_time << std::endl;

	delete c;

	Matrix* d = new Matrix(size);

	auto parallel_start = high_resolution_clock::now();
	pmul(d, m, 0, 0, n, 0, 0);
	auto parallel_end = high_resolution_clock::now();
	auto parallel_time = duration_cast<microseconds>(parallel_end - parallel_start).count();
	std::cout << "Parallel time: " << parallel_time << std::endl;

	delete m, n, d;
#endif
#endif

	return 0;
}
