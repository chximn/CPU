#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define LOGGER_PADDING 7

class Logger {
private:
	std::vector<std::string> lines;

public:
	Logger() = default;

	void set_lines(std::vector<std::string> const &);

	void error(std::string const &, int);
	void info(std::string const &);
};
