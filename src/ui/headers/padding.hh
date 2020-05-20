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

    ftxui::Element apply(ftxui::Element) const;
};

ftxui::Element operator|(ftxui::Element, padding const &);
