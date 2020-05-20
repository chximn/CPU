#include "padding.hh"

padding::padding(int t, int r, int b, int l): top(t), right(r), bottom(b), left(l) {}
padding::padding(int v, int h): top(v), bottom(v), right(h), left(h) {}
padding::padding(int h): top(0), bottom(0), right(h), left(h) {}

ftxui::Element padding::apply(ftxui::Element e) const {
    using namespace ftxui;

    std::wstring padding_right  = right  ? std::wstring(right, L' ')  : L"";
    std::wstring padding_left   = left   ? std::wstring(left, L' ')   : L"";

    auto h = hbox(
        text(padding_left),
        std::move(e),
        text(padding_right)
    );

    Elements lines;

    for (int i = 0; i < top; i++) {
        lines.push_back(text(L" "));
    }

    lines.push_back(std::move(h));

    for (int i = 0; i < bottom; i++) {
        lines.push_back(text(L" "));
    }

    return vbox(std::move(lines));
}

ftxui::Element operator|(ftxui::Element e, padding const & d) {
    return d.apply(std::move(e));
}
