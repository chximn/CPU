#include <iostream>
#include <cxxopts.hpp>
#include "ui.hh"
#include <fstream>

int main(int argc, char * argv[]) {
    // std::string code =


    // UserInterface ui(code, false, 30);
    // ui.start();

    using namespace cxxopts;

    Options options(argv[0], "Intel's x86-64 Architecture Simulation");
    options.allow_unrecognised_options()
           .add_options()
                ("f,filename",    "Input filename", cxxopts::value<std::string>())
                ("i,interactive", "Enable interactive execution")
                ("s,speed",       "Execution speed (instruction per minute)", cxxopts::value<int>()->default_value("100"))
                ("h,help",        "Print usage");

    options.positional_help("filename")
           .show_positional_help();

    std::string filename;
    bool interactive;
    int speed;

    try {
        options.parse_positional({"filename", "positional"});
        auto result = options.parse(argc, argv);

        if (result.count("help") || !result.count("filename")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        filename = result["filename"].as<std::string>();
        speed = result["speed"].as<int>();
        interactive = result.count("interactive") > 0;
    }

    catch (const cxxopts::OptionException& e)
    {
        std::cerr << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    std::ifstream file(filename.c_str());
    if (!file.good()) {
        std::cerr << "cannot access file: " << filename << std::endl;
        exit(2);
    }

    std::string file_contents { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    UserInterface ui(file_contents, interactive, speed);
    ui.start();
    return 0;
}
