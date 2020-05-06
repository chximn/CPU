%{

#include "operand.hh"
#include "scanner.hh"
#include "driver.hh"
#include "logger.hh"
#include <cstdlib>

#define YY_NO_UNISTD_H

using token = yy::Parser::token;

#undef  YY_DECL
#define YY_DECL int Scanner::yylex( yy::Parser::semantic_type * const lval, yy::Parser::location_type *loc )

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option c++
%option yyclass="Scanner"
%option noyywrap

%%

%{
    yylval = lval;
%}

","    return token::COMMA;
"["    return token::LEFT_BRACKET;
"]"    return token::RIGHT_BRACKET;
"+"    return token::PLUS;
"*"    return token::TIMES;
":"    return token::COLON;

"section" return token::SECTION;
".data"   return token::DATA;
".text"   return token::TEXT;

"db" return token::DB;
"dw" return token::DW;
"dd" return token::DD;
"dq" return token::DQ;

"byte"  return token::BYTE;
"word"  return token::WORD;
"dword" return token::DWORD;
"qword" return token::QWORD;

"mov"  return token::MOV;
"lea"  return token::LEA;
"push" return token::PUSH;
"pop"  return token::POP;
"add"  return token::ADD;
"sub"  return token::SUB;
"mul"  return token::MUL;
"div"  return token::DIV;
"neg"  return token::NEG;
"and"  return token::AND;
"or"   return token::OR;
"xor"  return token::XOR;
"not"  return token::NOT;
"shl"  return token::SHL;
"shr"  return token::SHR;
"cmp"  return token::CMP;
"jmp"  return token::JMP;
"je"   return token::JE;
"jne"  return token::JNE;
"jl"   return token::JL;
"jg"   return token::JG;
"jle"  return token::JLE;
"jge"  return token::JGE;
"call" return token::CALL;
"ret"  return token::RET;
"nop"  return token::NOP;
"hlt"  return token::HLT;

"cs"         { yylval->build<register_code>(register_code::cs);     return token::REGISTER; }
"ds"         { yylval->build<register_code>(register_code::ds);     return token::REGISTER; }
"ss"         { yylval->build<register_code>(register_code::ss);     return token::REGISTER; }
"rax"        { yylval->build<register_code>(register_code::rax);    return token::REGISTER; }
"rbx"        { yylval->build<register_code>(register_code::rbx);    return token::REGISTER; }
"rcx"        { yylval->build<register_code>(register_code::rcx);    return token::REGISTER; }
"rdx"        { yylval->build<register_code>(register_code::rdx);    return token::REGISTER; }
"eax"        { yylval->build<register_code>(register_code::eax);    return token::REGISTER; }
"ebx"        { yylval->build<register_code>(register_code::ebx);    return token::REGISTER; }
"ecx"        { yylval->build<register_code>(register_code::ecx);    return token::REGISTER; }
"edx"        { yylval->build<register_code>(register_code::edx);    return token::REGISTER; }
"ax"         { yylval->build<register_code>(register_code::ax);     return token::REGISTER; }
"bx"         { yylval->build<register_code>(register_code::bx);     return token::REGISTER; }
"cx"         { yylval->build<register_code>(register_code::cx);     return token::REGISTER; }
"dx"         { yylval->build<register_code>(register_code::dx);     return token::REGISTER; }
"al"         { yylval->build<register_code>(register_code::al);     return token::REGISTER; }
"bl"         { yylval->build<register_code>(register_code::bl);     return token::REGISTER; }
"cl"         { yylval->build<register_code>(register_code::cl);     return token::REGISTER; }
"dl"         { yylval->build<register_code>(register_code::dl);     return token::REGISTER; }
"ah"         { yylval->build<register_code>(register_code::ah);     return token::REGISTER; }
"bh"         { yylval->build<register_code>(register_code::bh);     return token::REGISTER; }
"ch"         { yylval->build<register_code>(register_code::ch);     return token::REGISTER; }
"dh"         { yylval->build<register_code>(register_code::dh);     return token::REGISTER; }
"rsp"        { yylval->build<register_code>(register_code::rsp);    return token::REGISTER; }
"esp"        { yylval->build<register_code>(register_code::esp);    return token::REGISTER; }
"sp"         { yylval->build<register_code>(register_code::sp);     return token::REGISTER; }
"rflags"     { yylval->build<register_code>(register_code::rflags); return token::REGISTER; }
"eflags"     { yylval->build<register_code>(register_code::eflags); return token::REGISTER; }
"flags"      { yylval->build<register_code>(register_code::flags);  return token::REGISTER; }
"st0"        { yylval->build<register_code>(register_code::st0);    return token::REGISTER; }
"st1"        { yylval->build<register_code>(register_code::st1);    return token::REGISTER; }
"st2"        { yylval->build<register_code>(register_code::st2);    return token::REGISTER; }
"st3"        { yylval->build<register_code>(register_code::st3);    return token::REGISTER; }
"st4"        { yylval->build<register_code>(register_code::st4);    return token::REGISTER; }
"st5"        { yylval->build<register_code>(register_code::st5);    return token::REGISTER; }
"st6"        { yylval->build<register_code>(register_code::st6);    return token::REGISTER; }
"st7"        { yylval->build<register_code>(register_code::st7);    return token::REGISTER; }
"xmm0"       { yylval->build<register_code>(register_code::xmm0);   return token::REGISTER; }
"xmm1"       { yylval->build<register_code>(register_code::xmm1);   return token::REGISTER; }
"xmm2"       { yylval->build<register_code>(register_code::xmm2);   return token::REGISTER; }
"xmm3"       { yylval->build<register_code>(register_code::xmm3);   return token::REGISTER; }
"xmm4"       { yylval->build<register_code>(register_code::xmm4);   return token::REGISTER; }
"xmm5"       { yylval->build<register_code>(register_code::xmm5);   return token::REGISTER; }
"xmm6"       { yylval->build<register_code>(register_code::xmm6);   return token::REGISTER; }
"xmm7"       { yylval->build<register_code>(register_code::xmm7);   return token::REGISTER; }

(;(.|[ ])*) {
	yylval->build<std::string>(YYText());
	return token::COMMENT;
}

([0-9]*\.[0-9]+) {
    yylval->build<double>(std::stod(yytext));
    return token::FLOAT;
}

[0-9]+ {
    yylval->build<int>(std::atoi(yytext));
    return token::INTEGER;
}

([0-1]+b) {
    yylval->build<int>(std::stoi(yytext, nullptr, 2));
    return token::INTEGER;
}

([0-7]+o) {
    yylval->build<int>(std::stoi(yytext, nullptr, 8));
    return token::INTEGER;
}

([0-9A-F]+h) {
    yylval->build<int>(std::stoi(yytext, nullptr, 16));
    return token::INTEGER;
}

\'(\.|[ ])\' {
    yylval->build<int>(yytext[1]);
    return token::INTEGER;
}

(\"(\\.|[^"\\])*\") {
    std::string str(yytext);
    yylval->build<std::string>(str.substr(1, str.length() - 1));
    return token::STRING;
}

(\.?[a-zA-Z_][a-zA-Z0-9_]*) {
    yylval->build<std::string>(yytext);
    return token::SYMBOL;
}

"\n" {
    loc->lines();
    return token::NL;
}

%%
