#include "ui.hh"

using namespace ftxui;

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

std::unique_ptr<ftxui::Node> UserInterface::render_data_segment() {
    auto registers = cpu.get_registers();
    auto ds = registers[register_code::ds]->get_value();
    std::vector<uint8_t> data(ram.get_data(ds, 108));
    std::vector<std::wstring> lines;

    int i = 0;
    std::wstring current_line;
    for (auto const & byte : data) {
        if (i > 0 && i % MEMORY_BYTES_PER_LINE == 0) {
            lines.push_back(current_line);
            current_line = L"";
        }
        else if (i) current_line += L" ";
        current_line += helpers::to_wstring(helpers::to_hex(byte, ""));
        i++;
    }

    if (current_line != L"") lines.push_back(current_line);

    Elements data_lines;
    for (auto const & line : lines) data_lines.push_back(text(line));

    return vbox(
        text(L"Data") | padding(1),
        separator(),
        vbox(std::move(data_lines)) | color(Color::GrayDark) | padding(1)
    ) | border;
}

std::unique_ptr<ftxui::Node> UserInterface::render_registers() {
    auto registers = cpu.get_registers();

    Elements elements;

    int i = 0;
    Elements current_line;
    for (auto const & l : registers) {
        auto reg = l.second;

        if (std::dynamic_pointer_cast<FullRegister>(reg) == nullptr) continue;

        if (i > 0 && i % REGISTERS_PER_LINE == 0) {
            elements.push_back(hbox(std::move(current_line)));
            current_line = std::vector<Element>{};
        }

        auto name  = helpers::to_wstring(Register::to_string(reg->get_code()));
        auto value = helpers::to_wstring(helpers::zero_extend(helpers::to_hex(reg->get_value(), ""), 16));

        current_line.push_back(
            hbox(text(name) | align_right | size(WIDTH, EQUAL, 5), text(L" "), text(value) | color(Color::GrayDark)) | padding(1) | notflex
        );

        i++;
    }
    elements.push_back(hbox(std::move(current_line)));


    return vbox(std::move(elements)) | border;
}

std::unique_ptr<ftxui::Node> UserInterface::render_instructions() {
    auto registers = cpu.get_registers();
    auto reg = cpu.get_control_unit().get_instruction_pointer_register();
    auto reg_value = static_cast<int>(reg->get_value());
    auto rip = helpers::to_wstring(helpers::zero_extend(helpers::to_hex(reg->get_value(), ""), 16));
    auto cs = registers[register_code::cs];
    auto cs_value = static_cast<int>(cs->get_value());

    auto start_address = std::max(cs_value, reg_value - INSTRUCTIONS_RANGE);
    auto instructions = ram.get_instructions(start_address, INSTRUCTIONS_RANGE * 2);

    auto current_instruction = reg_value + cs_value;

    Elements addresses_elements, instructions_elements;
    for (auto const & p : instructions) {
        auto address_e = text(helpers::to_wstring(helpers::zero_extend(helpers::to_hex(p.first, ""), 8))) | align_right;
        auto instruction_e = text(helpers::to_wstring(p.second));

        if (p.first != current_instruction) {
            address_e = std::move(address_e) | color(Color::GrayDark);
            instruction_e = std::move(instruction_e) | color(Color::GrayDark);
        }

        addresses_elements.push_back(std::move(address_e));
        instructions_elements.push_back(std::move(instruction_e));
    }

    return vbox(
        hbox(
            text(L"instructions") | padding(1),
            separator(),
            hbox(text(L"rip"), text(L" "), text(rip) | color(Color::GrayDark)) | padding(1) | notflex
        ),
        separator(),
        hbox(
            vbox(
                std::move(addresses_elements)
            ) | padding(1) | size(WIDTH, EQUAL, 10) | size(HEIGHT, EQUAL, 30),

            separator(),

            vbox(
                std::move(instructions_elements)
            ) | padding(1)
        )
    ) | border;
}

Element UserInterface::render_stack() {
    auto registers = cpu.get_registers();

    auto stack_bottom = LOADER_DEFAULT_STACK_SEGMENT + LOADER_DEFAULT_STACK_SIZE;
    auto sp = registers[register_code::rsp]->get_value();

    std::vector<uint8_t> stack(ram.get_data(sp, stack_bottom - sp));
    std::list<std::wstring> lines;

    int i = 0;
    std::wstring current_line;
    for (auto const & byte : stack) {
        if (i > 0 && i % STACK_BYTES_PER_LINE == 0) {
            lines.push_front(current_line);
            current_line = L"";
        }
        else if (i) current_line += L" ";
        current_line += helpers::to_wstring(helpers::to_hex(byte, ""));
        i++;
    }
    if (current_line != L"") lines.push_back(current_line);

    Elements stack_lines;

    int extend_lines = STACK_HEIGHT - lines.size();
    for (i = 0; i < extend_lines; i++) stack_lines.push_back(text(L""));
    for (auto const & line : lines) stack_lines.push_back(text(line));

    return vbox(
            vbox(std::move(stack_lines)) | color(Color::GrayDark) | padding(1),
            separator(),
            text(L" Stack")
        ) | size(WIDTH, EQUAL, 13) |  border;
}

Element UserInterface::render_sse() {
    auto registers = cpu.get_vector_unit().registers;
    Elements elements;

    for (int i = 0; i < 8; i++) {
        auto bytes = registers[i]->value_byte();
        std::string s;

        for (int j = 0; j < 16; j++) {
            if (j) s += " ";
            s += helpers::to_hex(bytes[j], "");
        }

        elements.push_back(text(helpers::to_wstring(s)));
    }

    return vbox(
        text(L"SSE") | padding(1),
        separator(),
        hbox(
            vbox(
                text(L"xmm0"),
                text(L"xmm1"),
                text(L"xmm2"),
                text(L"xmm3"),
                text(L"xmm4"),
                text(L"xmm5"),
                text(L"xmm6"),
                text(L"xmm7")
            ),

            text(L"  "),

            vbox(std::move(elements)) | color(Color::GrayDark)
        ) | padding(1)
    ) | border | notflex;
}

Element UserInterface::render_fpu() {
    auto stages = cpu.get_floating_point_unit().stages;
    Elements elements;
    Elements values;

    for (int i = 0; i < 8; i++) {
        uint64_t value = stages[i]->get_value();
        double d = *reinterpret_cast<double *>(&value);
        auto bytes = reinterpret_cast<uint8_t *>(&value);
        std::string s;

        for (int j = 0; j < 8; j++) {
            if (j) s += " ";
            s += helpers::to_hex(bytes[j], "");
        }

        elements.push_back(text(helpers::to_wstring(s)));
        values.push_back(text(helpers::to_wstring(std::to_string(d))));
    }

    return vbox(
        text(L"FPU") | padding(1),
        separator(),
        hbox(
            vbox(
                text(L"st0"),
                text(L"st1"),
                text(L"st2"),
                text(L"st3"),
                text(L"st4"),
                text(L"st5"),
                text(L"st6"),
                text(L"st7")
            ) | padding(1) | notflex,

            separator(),

            vbox(std::move(elements))  | padding(1) | color(Color::GrayDark) | notflex,

            separator(),

            vbox(std::move(values)) | padding(1)
        )
    ) | border | size(WIDTH, EQUAL, 55);
}

void UserInterface::render() {

    auto document = vbox(
        std::move(render_registers()),

        hbox(
            std::move(render_instructions()) | flex,

            text(L" "),

            vbox(
                std::move(render_data_segment()),
                std::move(render_sse()),
                std::move(render_fpu())
            ),

            text(L" "),

            std::move(render_stack())
        ) | flex
    ) | padding(1, 1);

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString();
    reset_position = screen.ResetPosition();
}

void UserInterface::start() {
    ControlUnit & cu = cpu.get_control_unit();

    while(!cu.halt) {
        render();
        auto wait_time = static_cast<int>((60.0 / static_cast<float>(speed)) * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
        cpu.step();
    }
}
