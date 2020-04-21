%skeleton "lalr1.cc"
%require "3.0"

%defines
%define api.parser.class { Parser }
%define api.value.type variant
%define parse.assert

%locations

%code requires{
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
%token                  END
%token <int>            NUMBER
%token <std::string>    COMMENT

%type <std::string>                  comment
%type <std::vector<instruction_ptr>> instructions
%type <instruction_ptr>              instruction
%type <std::vector<instruction_ptr>> program

%%

program:
    instructions {
        YYACCEPT;
    }

instructions:
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
    NUMBER {
        std::cout << "nombre : " << $1 << std::endl;
        $$ = nullptr;
    }

comment: COMMENT


%%

void yy::Parser::error( const location_type &l, const std::string & err_msg) {
    std::cerr << "Erreur : " << l << ", " << err_msg << std::endl;
}
