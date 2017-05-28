#pragma once

#include <vector>
#include <string>

#include "json.hpp"

#include "matrix.hpp"

class Test {

protected:
	int my_size;

	Matrix* my_a;
	Matrix* my_b;
	Matrix* my_s_c;
	Matrix* my_p_c;

public:
	virtual ~Test();
	virtual nlohmann::json run() = 0;

	int matrix_size() const;
};

class TestBundle {
protected:
	std::vector<Test*> my_tests;

public:
	virtual ~TestBundle();

	virtual nlohmann::json run() = 0;
};

class PerformanceTest: public Test {

	int my_it_num;
	std::vector<int> my_grainsize;
	
public:
	PerformanceTest(int size, int it_num, std::vector<int> grainsize);

	nlohmann::json run();
};

class PerformanceTestBundle: public TestBundle {
public:
	PerformanceTestBundle(std::string file_name);
	
	nlohmann::json run();
};

class VerificationTest: public Test {
	Matrix* my_c;
	
public:
	VerificationTest(int size, const std::vector<int>& a, const std::vector<int>& b, const std::vector<int>& c);
	~VerificationTest();

	nlohmann::json run();
};

class VerificationTestBundle: TestBundle {
public:
	VerificationTestBundle(std::string file_name);
	
	nlohmann::json run();
};
