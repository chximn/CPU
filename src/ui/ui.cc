#include "ui.hh"

UserInterface::UserInterface(std::string const & c, bool i, int s):
    code(c), interactive(i), speed(s),
    assembler(code),
    ram(),
    cpu(ram),
    loader(cpu, ram),
    program(assembler.assemble())
{
    loader.load(program);
}



void UserInterface::render() {
    using namespace ftxui;

    auto document =
    vbox(
        hbox(
            text(L"rax = 0"),
            separator(),
            text(L"rbx = 0"),
            separator(),
            text(L"rcx = 0"),
            separator(),
            text(L"rdx = 0"),
            separator()
        ) | border,

        hbox(
            vbox(
                vbox(
                    text(L"instruction"),
                    text(L"instruction"),
                    text(L"instruction"),
                    text(L"instruction"),
                    text(L"instruction")
                ) | align_right
            ) | border | flex,

            vbox(
                text(L"Hi, i'm the memory...") | size(WIDTH, EQUAL, 40)
            ) | border
        )
    ) | size(WIDTH, LESS_THAN, 100);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString();
    reset_position = screen.ResetPosition();
}
