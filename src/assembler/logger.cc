#include "logger.hh"

int logger::line_number = 0;
std::vector<std::string> logger::lines{};

std::string format_line_number(int i) {
	char buff[10];
	snprintf(buff, sizeof(buff), "%5d", i);
	std::string buffAsStdStr = buff;
	return buffAsStdStr;
}

void logger::set_lines(std::vector<std::string> const & ls) {
	for (auto const & l : ls) {
		lines.push_back(l);
	}
}

unsigned int logger::get_line_number() {
	return line_number;
}

void logger::increment_line() {
	line_number++;
}

void logger::error(std::string const & s) {
	error(s, line_number);
}

void logger::error(std::string const & s, int line) {
	std::cerr << "[\e[91mERROR\e[0m] " << s << "\n";

	for (int i = std::max(0, line - 2); i < line; i++)
		std::cerr << "\e[90m" << format_line_number(i) << " |\t" << lines.at(i) << "\e[0m\n";

	std::cerr << "\e[91m" << format_line_number(line) << "\e[0m |\t" << lines.at(line) << "\e[0m\n";

	int size = lines.size();
	for (int i = line + 1, l = std::min(size, line + 3) ; i < l; i++)
		std::cerr << "\e[90m" << format_line_number(i) << " |\t" << lines.at(i) << "\e[0m\n";

	exit(-2);
}

void logger::info(std::string const & s) {
	std::cout << "\e[96m[INFO]\t\e[0m " << line_number << ": " << s << "\n";
}
