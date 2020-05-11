#pragma once
#include "ftxui/dom/elements.hpp"

class padding {
private:
    int top;
    int right;
    int bottom;
    int left;

public:
    padding() = default;
    padding(int, int, int, int);
    padding(int, int);
    padding(int);

    std::unique_ptr<ftxui::Node> apply(std::unique_ptr<ftxui::Node>) const;
};

std::unique_ptr<ftxui::Node> operator|(std::unique_ptr<ftxui::Node>, padding const &);
