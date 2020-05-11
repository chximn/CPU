#include "padding.hh"

padding::padding(int t, int r, int b, int l): top(t), right(r), bottom(b), left(l) {}
padding::padding(int v, int h): top(v), bottom(v), right(h), left(h) {}
padding::padding(int h): top(0), bottom(0), right(h), left(h) {}

std::unique_ptr<ftxui::Node> padding::apply(std::unique_ptr<ftxui::Node> e) const {
    using namespace ftxui;

    std::wstring padding_top    = top    ? std::wstring(top, L' ')    : L"";
    std::wstring padding_right  = right  ? std::wstring(right, L' ')  : L"";
    std::wstring padding_bottom = bottom ? std::wstring(bottom, L' ') : L"";
    std::wstring padding_left   = left   ? std::wstring(left, L' ')   : L"";

    auto h = hbox(
        text(padding_left),
        std::move(e),
        text(padding_right)
    );

    if (top && bottom)
        return vbox(
            text(padding_top),
            std::move(h),
            text(padding_bottom)
        );

    else if (top)
        return vbox(
            text(padding_top),
            std::move(h)
        );
    else if (bottom)
        return vbox(
            std::move(h),
            text(padding_bottom)
        );
    else
        return std::move(h);
}

std::unique_ptr<ftxui::Node> operator|(std::unique_ptr<ftxui::Node> e, padding const & d) {
    return d.apply(std::move(e));
}
