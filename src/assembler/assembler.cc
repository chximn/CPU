#include "assembler.hh"

Assembler::Assembler(std::string const & input):
    code(input),
    driver(),
    scanner(in, out),
    parser(scanner, driver) {

    std::vector<std::string> lines;
    std::stringstream ss(input);
    std::string line;

    while(std::getline(ss, line, '\n')) lines.push_back(line);

    logger::set_lines(lines);
}

Program const & Assembler::assemble() {
    in << code;
    parser.parse();
    return driver.get_program();
}
