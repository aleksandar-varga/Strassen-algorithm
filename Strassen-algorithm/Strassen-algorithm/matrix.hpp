#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include "json.hpp"

class Matrix {

	int* m;
	int n;

public:
	Matrix() {
		m = nullptr;
	}

	Matrix(int size) : n(size) {
		m = new int[size * size];
		memset(m, 0, sizeof(int) * size * size);
	}

	Matrix(int size, int* values) : n(size) {
		int s = size * size;
		m = new int[s];
		memcpy_s(m, s * sizeof(int), values, s * sizeof(int));
	}

	Matrix(int size, std::vector<int> values) : n(size) {
		int s = size * size;

		if (s != values.size())
			throw std::runtime_error("Invalid arguments.");

		m = new int[s];
		
		for (int i = 0; i < s; ++i)
			m[i] = values[i];
	}

	Matrix(nlohmann::json j) {
		n = j["size"];

		int s = n * n;
		m = new int[s];
		if (s != j["values"].size())
			throw std::runtime_error("JSON is not valid.");

		for (int i = 0; i < s; ++i)
			m[i] = j["values"];
	}

	~Matrix() {
		delete[] m;
	}

	inline int& at(int i, int j) {
		return m[i * n + j];
	}

	inline int size() const {
		return n;
	}

	inline int* values() {
		return m;
	}

};