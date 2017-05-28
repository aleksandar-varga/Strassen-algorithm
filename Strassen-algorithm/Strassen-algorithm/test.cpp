#include "test.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

#include "json.hpp"

#include "parallel.hpp"
#include "serial.hpp"

using namespace std;
using namespace nlohmann;

Test::~Test() {
	delete my_a, my_b, my_s_c, my_p_c;
}

int Test::matrix_size() const {
	return my_size;
}

TestBundle::~TestBundle() {
	for (auto it = my_tests.begin(); it != my_tests.end(); ++it)
		delete *it;
}

PerformanceTest::PerformanceTest(int size, int it_num, vector<int> grainsize) : my_it_num(it_num), my_grainsize(grainsize) {
	my_size = size;
	my_a = new Matrix(size);
	my_b = new Matrix(size);
	my_s_c = new Matrix(size);
	my_p_c = new Matrix(size);


	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			my_a->at(i, j) = rand() % 100;
			my_b->at(i, j) = rand() % 100;
		}
	}
}

json PerformanceTest::run() {
	using namespace std::chrono;

	json result;

	steady_clock::time_point start, end;
	float avg_serial_time = 0;

	cout << "Running serial version." << endl;
	for (int i = 0; i < my_it_num; ++i) {
		cout << "\rIteration: " << i + 1;
		
		start = high_resolution_clock::now();
		mul(my_s_c, my_a, 0, 0, my_b, 0, 0);
		end = high_resolution_clock::now();
		
		avg_serial_time += duration_cast<milliseconds>(end - start).count();
		
	}
	avg_serial_time /= my_it_num;
	cout << endl << "Avarage serial time: " << avg_serial_time << "ms" << endl << endl;
	result["serial"]["time"] = avg_serial_time;

	vector<float> avg_parallel_time(my_grainsize.size());
	float speedup = 0;
	
	for (int i = 0; i < my_grainsize.size(); ++i) {
		json parallel_result;
		
		Task::grainsize = my_grainsize[i];
		parallel_result["grainsize"] = my_grainsize[i];

		cout << "Running parallel version for grainsize " << my_grainsize[i] << "." << endl;
		for (int j = 0; j < my_it_num; ++j) {
			cout << "\rIteration: " << j + 1;
			
			start = high_resolution_clock::now();
			pmul(my_p_c, my_a, 0, 0, my_b, 0, 0);
			end = high_resolution_clock::now();
			
			avg_parallel_time[i] += duration_cast<milliseconds>(end - start).count();

		}
		avg_parallel_time[i] /= my_it_num;
		cout << endl << "Avarage parallel time: " << avg_parallel_time[i] << "ms" << endl;
		parallel_result["time"] = avg_parallel_time[i];

		speedup = avg_serial_time / avg_parallel_time[i];
		cout << "Avarage speedup: " << speedup << "x" << endl << endl;
		parallel_result["speedup"] = speedup;

		result["parallel"].push_back(parallel_result);

	}

	return result;
}

PerformanceTestBundle::PerformanceTestBundle(std::string file_name) {
	using namespace nlohmann;

	std::ifstream i(file_name);
	json j;
	i >> j;

	for (auto test : j["tests"])
		my_tests.push_back(new PerformanceTest(test["size"], j["iterations"], test["grainsize"]));

	i.close();
}

json PerformanceTestBundle::run() {
	json result;

	cout << "--------------------------" << endl;
	cout << "Starting perfomance tests." << endl << endl;
	for (int i = 0; i < my_tests.size(); ++i) {
		cout << "--------------------------" << endl;
		cout << "Running test number: " << i + 1 << endl << endl;
		
		json test;
		test = my_tests[i]->run();
		test["matrix size"] = my_tests[i]->matrix_size();

		result.push_back(test);
	}
	cout << "Perfomance tests finished." << endl;
	cout << "--------------------------" << endl << endl;

	return result;
}

VerificationTest::VerificationTest(int size, const std::vector<int>& a, const std::vector<int>& b, const std::vector<int>& c) {
	my_size = size;
	my_a = new Matrix(size, a);
	my_b = new Matrix(size, b);
	my_c = new Matrix(size, c);
}

VerificationTest::~VerificationTest() {
	delete my_c;
}

json VerificationTest::run() {
	json result;

	my_s_c = new Matrix(my_size);

	mul(my_s_c, my_a, 0, 0, my_b, 0, 0);

	bool succesful = true;

	for (int i = 0; i < my_size; ++i)
		for (int j = 0; j < my_size; ++j)
			if (my_s_c->at(i, j) != my_c->at(i, j))
				succesful = false;

	if (succesful) {
		cout << "Serial version verified." << endl;
		result["serial"] = "succesful";
	}
	else {
		cout << "Serial version verification failed." << endl;
		result["serial"] = "failed";
	}

	delete my_s_c;

	my_p_c = new Matrix(my_size);

	pmul(my_p_c, my_a, 0, 0, my_b, 0, 0);

	succesful = true;

	for (int i = 0; i < my_size; ++i)
		for (int j = 0; j < my_size; ++j)
			if (my_p_c->at(i, j) != my_c->at(i, j))
				succesful = false;

	if (succesful) {
		std::cout << "Parallel version verified." << std::endl;
		result["parallel"] = "succesful";
	}
	else {
		cout << "Parallel version verification failed." << endl;
		result["parallel"] = "failed";
	}
	delete my_p_c;

	return result;
}

VerificationTestBundle::VerificationTestBundle(std::string file_name) {
	std::ifstream i(file_name);
	json j;
	i >> j;

	for (auto test : j["tests"])
		my_tests.push_back(new VerificationTest(test["size"], test["A"], test["B"], test["C"]));
	
	i.close();
}

json VerificationTestBundle::run() {
	json result;

	cout << "--------------------------" << endl;
	std::cout << "Starting verification tests.." << std::endl;
	for (int i = 0; i < my_tests.size(); ++i) {
		cout << "--------------------------" << endl;
		std::cout << "Running test number: " << i + 1 << std::endl;
		std::cout << std::endl;
	
		result.push_back(my_tests[i]->run());
	}
	std::cout << "Verification tests finished." << std::endl;
	cout << "--------------------------" << endl;

	return result;
}