#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define LOGGER_PADDING 5

class logger {
private:
	static std::vector<std::string> lines;

public:
	logger() = delete;

	static void set_lines(std::vector<std::string> const &);

	static void error(std::string const &, int);
	static void info(std::string const &);
};
