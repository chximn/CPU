%skeleton "lalr1.cc"
%require "3.0"

%defines
%define api.parser.class { Parser }
%define api.value.type variant
%define parse.assert

%locations

%code requires{
    #include <utility>
    #include "logger.hh"
    #include "instruction.hh"

    class Scanner;
    class Driver;
    class Logger;

    struct data_item {
        uint64_t value;
        bool is_real;
    };
}

%parse-param { Scanner & scanner }
%parse-param { Driver  & driver }
%parse-param { Logger  & logger }

%code{
    #include <iostream>
    #include <string>

    #include <vector>
    #include "scanner.hh"
    #include "program.hh"
    #include "driver.hh"

    #undef  yylex
    #define yylex scanner.yylex

}

%token                  NL
%token                  COMMA         ","
%token                  LEFT_BRACKET  "["
%token                  RIGHT_BRACKET "]"
%token                  PLUS          "+"
%token                  TIMES         "*"
%token                  COLON         ":"

%token SECTION DATA TEXT

%token <std::string>    SYMBOL
%token <double>         FLOAT
%token <int>            INTEGER
%token <std::string>    STRING
%token <std::string>    COMMENT
%token <register_code>  REGISTER

/* instructions */
%token MOV LEA PUSH POP ADD SUB MUL DIV NEG AND OR XOR NOT SHL SHR CMP JMP JE JNE JL JG JLE JGE CALL RET NOP HLT

/* size modifiers */
%token BYTE WORD DWORD QWORD
%token DB DW DD DQ

%type <std::vector<instruction_ptr>> instructions
%type <instruction_ptr>              instruction
%type program
%type <std::pair<std::vector<std::pair<std::string, std::vector<uint8_t>>>, std::vector<instruction_ptr>>> sections

%type <std::vector<instruction_ptr>> text_section
%type <std::vector<std::pair<std::string, std::vector<uint8_t>>>> data_section

%type <std::vector<std::pair<std::string, std::vector<uint8_t>>>> definitions
%type <std::pair<std::string, std::vector<uint8_t>>> definition
%type <uint8_t> definition_size_specifier;
%type <std::vector<data_item>> data_list
%type <std::vector<data_item>> data

%type <std::vector<operand_ptr>> one_alu_operand two_alu_operands
%type <operand_ptr> register_op
%type <operand_ptr> immediate_op
%type <operand_ptr> immediate_op_without_size
%type <operand_ptr> memory_op
%type <operand_ptr> memory_op_without_segment
%type <operand_ptr> memory_op_without_size
%type <operand_ptr> label_op
%type <operand_ptr> object_op
%type <operand_ptr> imm_label_op
%type <uint8_t>     scale
%type <uint8_t>     size_specifier

%%

program:
    sections {
        auto definitions = $1.first;
        auto instructions = $1.second;

        Program program;

        for (auto const & definition : definitions) {
            driver.symbol_table.objects[definition.first] = program.add_data(definition.second);
        }

        for (auto & instruction : instructions) {
            auto operands = instruction->get_operands();

            int i = 0;
            for (auto const & op : operands) {

                auto object_operand = std::dynamic_pointer_cast<ObjectOperand>(op);
                auto memory_operand = std::dynamic_pointer_cast<MemoryOperand>(op);
                auto label_operand  = std::dynamic_pointer_cast<LabelOperand>(op);

                if (object_operand != nullptr) {
                    auto name = object_operand->get_name();
                    if (driver.symbol_table.objects.find(name) == driver.symbol_table.objects.end())
                        logger.error(name + " was not declared", object_operand->get_line());

                    auto value = driver.symbol_table.objects[name];
                    auto new_operand = std::make_shared<ImmediateOperand>(value);
                    operands.at(i) = new_operand;
                }

                else if (memory_operand != nullptr && memory_operand->get_use_object_displacement()) {
                    auto displacement = std::dynamic_pointer_cast<ObjectOperand>(memory_operand->get_object_displacement());

                    auto name = displacement->get_name();
                    if (driver.symbol_table.objects.find(name) == driver.symbol_table.objects.end())
                        logger.error(name + " was not declared", displacement->get_line());

                    auto value = driver.symbol_table.objects[name];
                    memory_operand->set_displacement(value);
                }

                else if (label_operand != nullptr) {
                    auto name = label_operand->get_name();
                    if (driver.symbol_table.labels.find(name) == driver.symbol_table.labels.end())
                        logger.error("label" + name + " was not found", label_operand->get_line());

                    auto source = std::find(instructions.begin(), instructions.end(), instruction);
                    auto destination = std::find(instructions.begin(), instructions.end(), driver.symbol_table.labels[name]);

                    auto diff = destination - (source + 1);
                    auto new_operand = std::make_shared<ImmediateOperand>(diff * 8);
                    operands.at(i) = new_operand;
                }

                instruction->set_operands(operands);
                i++;
            }

            program.add_instruction(instruction);
        }

        driver.set_program(program);

        YYACCEPT;
    }

sections:
    NL sections { $$ = $2; } |
    COMMENT NL sections { $$ = $3; } |
    data_section text_section { $$ = std::pair<std::vector<std::pair<std::string, std::vector<uint8_t>>>, std::vector<instruction_ptr>>{$1, $2}; } |
    text_section data_section { $$ = std::pair<std::vector<std::pair<std::string, std::vector<uint8_t>>>, std::vector<instruction_ptr>>{$2, $1}; } |
    text_section { $$ = std::pair<std::vector<std::pair<std::string, std::vector<uint8_t>>>, std::vector<instruction_ptr>>{ std::vector<std::pair<std::string, std::vector<uint8_t>>>{}, $1}; }

data_section:
    SECTION DATA NL definitions {
        $$ = $4;
    }

text_section:
    SECTION TEXT NL instructions {
        $$ = $4;
    }

definitions:
    COMMENT NL definitions {
        $$ = $3;
    } |

    definition COMMENT NL definitions {
        $4.insert($4.begin(), $1);
        $$ = $4;
    } |

    definition NL definitions {
        $3.insert($3.begin(), $1);
        $$ = $3;
    } |

    comment_line {
        $$ = std::vector<std::pair<std::string, std::vector<uint8_t>>>();
    } |

    definition COMMENT NL {
        auto v = std::vector<std::pair<std::string, std::vector<uint8_t>>>();
        v.push_back($1);
        $$ = v;
    } |

    definition NL {
        auto v = std::vector<std::pair<std::string, std::vector<uint8_t>>>();
        v.push_back($1);
        $$ = v;
    } |

    NL definitions {

        $$ = $2;
    } |

    empty_line {
        $$ = std::vector<std::pair<std::string, std::vector<uint8_t>>>();
    }

definition:
    SYMBOL definition_size_specifier data_list {
        if ($1[0] == '.') logger.error("syntax error", @1.begin.line);

        std::vector<uint8_t> data{};

        for (auto const & item : $3) {
            const uint8_t * bytes;

            if (item.is_real) {
                double value = *reinterpret_cast<const double*>(&(item.value));
                float f_value = (float) value;

                if ($2 == 32) bytes = reinterpret_cast<const uint8_t *>(&f_value);
                else if ($2 == 64) bytes = reinterpret_cast<const uint8_t *>(&value);
                else logger.error("floating point numbers need at least 32bits to be represented", @1.begin.line);
            }

            else bytes = reinterpret_cast<const uint8_t *>(&(item.value));

            for (int i = 0, n_items = $2 / 8; i < n_items; i++) {
                data.push_back(bytes[i]);
            }

        }

        $$ = std::pair<std::string, std::vector<uint8_t>>{$1, data};
    }

definition_size_specifier:
    DB { $$ = 8; }  |
    DW { $$ = 16; } |
    DD { $$ = 32; } |
    DQ { $$ = 64; }

data_list:
    data { $$ = $1; } |

    data "," data_list {
        $3.insert($3.begin(), $1.begin(), $1.end());
        $$ = $3;
    }

data:
    FLOAT {
        $$ = std::vector<data_item>{
            data_item{ *reinterpret_cast<uint64_t *>(&$1), true }
        };
    } |

    INTEGER {
        $$ = std::vector<data_item>{
            data_item{ static_cast<uint64_t>($1), false}
        };
    } |

    STRING {
        auto v = std::vector<data_item>();

        for (char c : $1) {
            v.push_back(data_item{static_cast<uint64_t>(c), false});
        }

        $$ = v;
    };


instructions:
    SYMBOL ":" instructions {
        driver.symbol_table.labels[$1] = $3.at(0);
        $$ = $3;
    } |

	COMMENT  NL instructions {
		$$ = $3;
	} |

	instruction COMMENT  NL instructions {
		$4.insert($4.begin(), $1);
		$$ = $4;
	} |

	instruction NL instructions {
		$3.insert($3.begin(), $1);
		$$ = $3;
	} |

	comment_line {
		$$ = std::vector<instruction_ptr>();
	} |

	instruction COMMENT  NL {
		auto v = std::vector<instruction_ptr>();
		v.push_back($1);
		$$ = v;
	} |

	instruction NL {
		auto v = std::vector<instruction_ptr>();
		v.push_back($1);
		$$ = v;
	} |

	NL instructions {
		$$ = $2;
	} |

	empty_line {
		$$ = std::vector<instruction_ptr>();
	}

instruction:

    MOV two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::mov, $2, $2.at(0)->get_size()); } |

    LEA register_op "," memory_op {
        auto op = std::dynamic_pointer_cast<MemoryOperand>($4);

        if (op->get_size() != 0) logger.error("invalid size specification", @1.begin.line);
        if (op->get_use_segment()) logger.error("invalid segment specification", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::lea, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    PUSH one_alu_operand {
        if ($2.at(0)->get_size() == 0) logger.error("size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::push, $2, $2.at(0)->get_size());
    } |

    POP one_alu_operand  {
        if ($2.at(0)->get_size() == 0) logger.error("size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<ImmediateOperand>($2.at(0)) != nullptr) logger.error("invalid operand", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::push, $2, $2.at(0)->get_size());
    } |

    ADD two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::add, $2, $2.at(0)->get_size()); } |
    SUB two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::sub, $2, $2.at(0)->get_size()); } |

    MUL one_alu_operand { $$ = std::make_shared<Instruction>(instruction_code::mul, $2, $2.at(0)->get_size()); } |
    DIV one_alu_operand { $$ = std::make_shared<Instruction>(instruction_code::_div, $2, $2.at(0)->get_size()); } |

    NEG one_alu_operand { $$ = std::make_shared<Instruction>(instruction_code::neg, $2, $2.at(0)->get_size()); } |

    AND two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::_and, $2, $2.at(0)->get_size()); } |
    OR  two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::_or,  $2, $2.at(0)->get_size()); } |
    XOR two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::_xor, $2, $2.at(0)->get_size()); } |

    NOT one_alu_operand { $$ = std::make_shared<Instruction>(instruction_code::_not, $2, $2.at(0)->get_size()); } |

    SHL register_op { $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2}, $2->get_size()); } |
    SHL register_op "," immediate_op {
        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHL register_op "," register_op {
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger.error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHL memory_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2}, $2->get_size());
    } |

    SHL memory_op "," immediate_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHL memory_op "," register_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger.error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR register_op { $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2}, $2->get_size()); } |
    SHR register_op "," immediate_op {
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR register_op "," register_op {
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger.error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR memory_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2}, $2->get_size());
    } |

    SHR memory_op "," immediate_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR memory_op "," register_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger.error("memory operand size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger.error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    CMP two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::cmp, $2, $2.at(0)->get_size()); } |

    JMP imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jmp, std::vector<operand_ptr>{ $2 }, 0); } |
    JE  imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::je,  std::vector<operand_ptr>{ $2 }, 0); } |
    JNE imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jne, std::vector<operand_ptr>{ $2 }, 0); } |
    JL  imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jl,  std::vector<operand_ptr>{ $2 }, 0); } |
    JLE imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jle, std::vector<operand_ptr>{ $2 }, 0); } |
    JG  imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jg,  std::vector<operand_ptr>{ $2 }, 0); } |
    JGE imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::jge, std::vector<operand_ptr>{ $2 }, 0); } |

    CALL imm_label_op { $$ = std::make_shared<Instruction>(instruction_code::call, std::vector<operand_ptr>{ $2 }, 0); } |

    RET  { $$ = std::make_shared<Instruction>(instruction_code::ret, std::vector<operand_ptr>{}, 0); } |

    NOP { $$ = std::make_shared<Instruction>(instruction_code::nop, std::vector<operand_ptr>{}, 0); } |
    HLT { $$ = std::make_shared<Instruction>(instruction_code::hlt, std::vector<operand_ptr>{}, 0); }

one_alu_operand:
    register_op  { $$ = std::vector<operand_ptr>{$1}; } |
    memory_op    { $$ = std::vector<operand_ptr>{$1}; } |
    immediate_op { $$ = std::vector<operand_ptr>{$1}; } |
    object_op    { $$ = std::vector<operand_ptr>{$1}; }

two_alu_operands:
    register_op "," immediate_op {
        if ($1->get_size() < $3->get_size()) logger.error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    register_op "," object_op { $$ = std::vector<operand_ptr>{$1, $3}; } |

    register_op "," memory_op    {
        auto mem_op = std::dynamic_pointer_cast<MemoryOperand>($3);
        if ($3->get_size() == 0) mem_op->set_size($1->get_size());
        else if ($1->get_size() != $3->get_size()) logger.error("size mismatch", @1.begin.line);
        if (!mem_op->get_use_segment()) mem_op->set_segment(register_code::ds);
        $$ = std::vector<operand_ptr>{$1, $3};
    } |

    register_op "," register_op {
        if ($1->get_size() != $3->get_size()) logger.error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    memory_op   "," register_op  {
        auto mem_op = std::dynamic_pointer_cast<MemoryOperand>($1);
        if ($1->get_size() == 0) mem_op->set_size($3->get_size());
        if ($1->get_size() != $3->get_size()) logger.error("size mismatch", @1.begin.line);
        if (!mem_op->get_use_segment()) mem_op->set_segment(register_code::ds);
        $$ = std::vector<operand_ptr>{$1, $3};
    } |

    memory_op   "," immediate_op {
        auto mem_op = std::dynamic_pointer_cast<MemoryOperand>($1);
        if ($1->get_size() == 0) logger.error("memory operand size must be provided", @1.begin.line);
        if ($1->get_size() < $3->get_size()) logger.error("size mismatch", @1.begin.line);
        if (!mem_op->get_use_segment()) mem_op->set_segment(register_code::ds);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    memory_op   "," object_op {
        auto mem_op = std::dynamic_pointer_cast<MemoryOperand>($1);
        if ($1->get_size() == 0) logger.error("memory operand size must be provided", @1.begin.line);
        if (!mem_op->get_use_segment()) mem_op->set_segment(register_code::ds);
        $$ = std::vector<operand_ptr>{$1, $3}; }

imm_label_op: label_op | immediate_op

label_op:
    SYMBOL { $$ = std::make_shared<LabelOperand>($1, @1.begin.line); }

object_op:
    SYMBOL {
        if ($1[0] == '.') logger.error("syntax error", @1.begin.line);
        $$ = std::make_shared<ObjectOperand>($1, @1.begin.line);
    }

register_op:
    REGISTER { $$ = std::make_shared<RegisterOperand>($1); }

immediate_op:
    size_specifier immediate_op_without_size { std::dynamic_pointer_cast<ImmediateOperand>($2)->set_size($1);  $$ = $2; } |
    immediate_op_without_size

immediate_op_without_size:
    INTEGER { $$ = std::make_shared<ImmediateOperand>($1); }

memory_op:
    size_specifier memory_op_without_size { std::dynamic_pointer_cast<MemoryOperand>($2)->set_size($1);  $$ = $2; } |
    memory_op_without_size

memory_op_without_size:
    REGISTER ":" memory_op_without_segment {
        if ($1 != register_code::cs && $1 != register_code::ds && $1 != register_code::ss)
            logger.error("invalid segment", @1.begin.line);

        std::dynamic_pointer_cast<MemoryOperand>($3)->set_segment($1);
        $$ = $3;
    } |

    memory_op_without_segment

memory_op_without_segment:
    "[" REGISTER "+" REGISTER "]"                      { $$ = std::make_shared<MemoryOperand>($2, $4, 1, 0, 0);   } |
    "[" REGISTER "+" REGISTER "*" scale "]"            { $$ = std::make_shared<MemoryOperand>($2, $4, $6, 0, 0);  } |
    "[" REGISTER "+" REGISTER "*" scale "+" INTEGER "]" { $$ = std::make_shared<MemoryOperand>($2, $4, $6, $8, 0); } |
    "[" REGISTER "*" scale "]"                         { $$ = std::make_shared<MemoryOperand>($2, $4, 0, 0);      } |
    "[" REGISTER "*" scale "+" INTEGER "]"              { $$ = std::make_shared<MemoryOperand>($2, $4, $6, 0);     } |
    "[" REGISTER "]"                                   { $$ = std::make_shared<MemoryOperand>($2, 0, 0);          } |
    "[" REGISTER "+" INTEGER "]"                        { $$ = std::make_shared<MemoryOperand>($2, $4, 0);         } |
    "[" INTEGER "]"                                     { $$ = std::make_shared<MemoryOperand>($2, 0);             } |

    "[" REGISTER "+" REGISTER "*" scale "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, $4, $6, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($8, @8.begin.line));
        $$ = op;
    } |

    "[" REGISTER "*" scale "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, $4, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($6, @6.begin.line));
        $$ = op;
    } |

    "[" REGISTER "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($4, @4.begin.line));
        $$ = op;
    } |

    "[" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>(0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($2, @2.begin.line));
        $$ = op;
    }

scale:
    INTEGER {
        if ($1 != 1 && $1 != 2 && $1 != 4 && $1 != 8)
            logger.error("invalid scale", @1.begin.line);

        $$ = $1;
    }

size_specifier:
    BYTE  { $$ = 8; } |
    WORD  { $$ = 16; } |
    DWORD { $$ = 32; } |
    QWORD { $$ = 64; }


empty_line: NL
comment_line: COMMENT NL

%%

void yy::Parser::error(const location_type &l, const std::string & err_msg) {
    logger.error(err_msg, l.begin.line);
    exit(-1);
}
