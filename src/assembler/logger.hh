#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

class logger {
private:
	static int line_number;
	static std::vector<std::string> lines;

public:
	logger() = delete;

	static void set_lines(std::vector<std::string> const &);
	static unsigned int get_line_number();
	static void increment_line();

	static void error(std::string const &, int);
	static void error(std::string const &);


	static void info(std::string const &);
};
