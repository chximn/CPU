%skeleton "lalr1.cc"
%require "3.0"

%defines
%define api.parser.class { Parser }
%define api.value.type variant
%define parse.assert

%locations

%code requires{
    #include "logger.hh"
    #include "instruction.hh"
    class Scanner;
    class Driver;
}

%parse-param { Scanner &scanner }
%parse-param { Driver &driver }

%code{
    #include <iostream>
    #include <string>

    #include <vector>
    #include "scanner.hh"
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

%token <std::string>    SYMBOL
%token <int>            NUMBER
%token <std::string>    COMMENT
%token <register_code>  REGISTER

/* instructions */
%token MOV LEA PUSH POP ADD SUB MUL DIV NEG AND OR XOR NOT SHL SHR CMP JMP JE JNE JL JG JLE JGE CALL RET NOP HLT

/* size modifiers */
%token BYTE WORD DWORD QWORD

%type <std::string>                  comment
%type <std::vector<instruction_ptr>> instructions
%type <instruction_ptr>              instruction
%type <std::vector<instruction_ptr>> program

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
    instructions {

        logger::info("instructions: ");
        for (auto const & i : $1) {
            logger::info(i->to_string());
        }

        logger::info("labels: ");
        for (auto const & i : driver.symbol_table.labels) {
            logger::info(i.first + " --> " + i.second->to_string());
        }

        YYACCEPT;
    }

instructions:
    SYMBOL ":" instructions {
        logger::info("found label " + $1 + " at line " + std::to_string(@1.begin.line) );
        driver.symbol_table.labels[$1] = $3.at(0);
        $$ = $3;
    } |

	comment NL instructions {
		$$ = $3;
	} |

	instruction comment NL instructions {
		$4.insert($4.begin(), $1);
		$$ = $4;
	} |

	instruction NL instructions {
		$3.insert($3.begin(), $1);
		$$ = $3;
	} |

	comment NL {
		$$ = std::vector<instruction_ptr>();
	} |

	instruction comment NL {
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

	NL {
		$$ = std::vector<instruction_ptr>();
	}

instruction:

    MOV two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::mov, $2, $2.at(0)->get_size()); } |

    LEA register_op "," memory_op {
        auto op = std::dynamic_pointer_cast<MemoryOperand>($4);

        if (op->get_size() != 0) logger::error("invalid size specification", @1.begin.line);
        if (op->get_use_segment()) logger::error("invalid segment specification", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::lea, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    PUSH one_alu_operand {
        if ($2.at(0)->get_size() == 0) logger::error("size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::push, $2, $2.at(0)->get_size());
    } |

    POP one_alu_operand  {
        if ($2.at(0)->get_size() == 0) logger::error("size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<ImmediateOperand>($2.at(0)) != nullptr) logger::error("invalid operand", @1.begin.line);
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
            logger::error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHL memory_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2}, $2->get_size());
    } |

    SHL memory_op "," immediate_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHL memory_op "," register_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger::error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shl, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR register_op { $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2}, $2->get_size()); } |
    SHR register_op "," immediate_op {
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR register_op "," register_op {
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger::error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR memory_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2}, $2->get_size());
    } |

    SHR memory_op "," immediate_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    SHR memory_op "," register_op {
        if (std::dynamic_pointer_cast<MemoryOperand>($2)->get_size() == 0) logger::error("memory operand size must be specified", @1.begin.line);
        if (std::dynamic_pointer_cast<RegisterOperand>($4)->get_reg() != register_code::cl)
            logger::error("only CL can be used as a register operand", @1.begin.line);

        $$ = std::make_shared<Instruction>(instruction_code::shr, std::vector<operand_ptr>{$2, $4}, $2->get_size());
    } |

    CMP two_alu_operands { $$ = std::make_shared<Instruction>(instruction_code::cmp, $2, $2.at(0)->get_size()); } |

    JMP { $$ = std::make_shared<Instruction>(instruction_code::jmp, std::vector<operand_ptr>{}, 0); } |
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
    immediate_op { $$ = std::vector<operand_ptr>{$1}; }

two_alu_operands:
    register_op "," immediate_op {
        if ($1->get_size() < $3->get_size()) logger::error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    register_op "," label_op { $$ = std::vector<operand_ptr>{$1, $3}; } |

    register_op "," memory_op    {
        if ($3->get_size() == 0) std::dynamic_pointer_cast<MemoryOperand>($3)->set_size($1->get_size());
        else if ($1->get_size() != $3->get_size()) logger::error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    register_op "," register_op {
        if ($1->get_size() != $3->get_size()) logger::error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    memory_op   "," register_op  {
        if ($1->get_size() == 0) std::dynamic_pointer_cast<MemoryOperand>($1)->set_size($3->get_size());
        if ($1->get_size() != $3->get_size()) logger::error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    memory_op   "," immediate_op {
        if ($1->get_size() == 0) logger::error("memory operand size must be provided", @1.begin.line);
        if ($1->get_size() < $3->get_size()) logger::error("size mismatch", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; } |

    memory_op   "," label_op {
        if ($1->get_size() == 0) logger::error("memory operand size must be provided", @1.begin.line);
        $$ = std::vector<operand_ptr>{$1, $3}; }

comment: COMMENT

imm_label_op: label_op | immediate_op

label_op:
    SYMBOL { $$ = std::make_shared<LabelOperand>($1); }

object_op:
    SYMBOL { $$ = std::make_shared<ObjectOperand>($1); }

register_op:
    REGISTER { $$ = std::make_shared<RegisterOperand>($1); }

immediate_op:
    size_specifier immediate_op_without_size { std::dynamic_pointer_cast<ImmediateOperand>($2)->set_size($1);  $$ = $2; } |
    immediate_op_without_size

immediate_op_without_size:
    NUMBER { $$ = std::make_shared<ImmediateOperand>($1); }

memory_op:
    size_specifier memory_op_without_size { std::dynamic_pointer_cast<MemoryOperand>($2)->set_size($1);  $$ = $2; } |
    memory_op_without_size

memory_op_without_size:
    REGISTER ":" memory_op_without_segment {
        if ($1 != register_code::cs && $1 != register_code::ds && $1 != register_code::ss)
            logger::error("invalid segment", @1.begin.line);

        std::dynamic_pointer_cast<MemoryOperand>($3)->set_segment($1);
        $$ = $3;
    } |

    memory_op_without_segment

memory_op_without_segment:
    "[" REGISTER "+" REGISTER "]"                      { $$ = std::make_shared<MemoryOperand>($2, $4, 1, 0, 0);   } |
    "[" REGISTER "+" REGISTER "*" scale "]"            { $$ = std::make_shared<MemoryOperand>($2, $4, $6, 0, 0);  } |
    "[" REGISTER "+" REGISTER "*" scale "+" NUMBER "]" { $$ = std::make_shared<MemoryOperand>($2, $4, $6, $8, 0); } |
    "[" REGISTER "*" scale "]"                         { $$ = std::make_shared<MemoryOperand>($2, $4, 0, 0);      } |
    "[" REGISTER "*" scale "+" NUMBER "]"              { $$ = std::make_shared<MemoryOperand>($2, $4, $6, 0);     } |
    "[" REGISTER "]"                                   { $$ = std::make_shared<MemoryOperand>($2, 0, 0);          } |
    "[" REGISTER "+" NUMBER "]"                        { $$ = std::make_shared<MemoryOperand>($2, $4, 0);         } |
    "[" NUMBER "]"                                     { $$ = std::make_shared<MemoryOperand>($2, 0);             } |

    "[" REGISTER "+" REGISTER "*" scale "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, $4, $6, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($8));
        $$ = op;
    } |

    "[" REGISTER "*" scale "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, $4, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($6));
        $$ = op;
    } |

    "[" REGISTER "+" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>($2, 0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($4));
        $$ = op;
    } |

    "[" SYMBOL "]" {
        auto op = std::make_shared<MemoryOperand>(0, 0);
        op->set_object_displacement(std::make_shared<ObjectOperand>($2));
        $$ = op;
    }

scale:
    NUMBER {
        if ($1 != 1 && $1 != 2 && $1 != 4 && $1 != 8)
            logger::error("invalid scale", @1.begin.line);

        $$ = $1;
    }

size_specifier:
    BYTE  { $$ = 8; } |
    WORD  { $$ = 16; } |
    DWORD { $$ = 32; } |
    QWORD { $$ = 64; }

%%

void yy::Parser::error(const location_type &l, const std::string & err_msg) {
    logger::error(err_msg, l.begin.line);
    exit(-1);
}
