#ifndef SCANNER_H
#define SCANNER_H

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.hpp"
#include "location.hh"


class Scanner : public yyFlexLexer {
public:
    Scanner(std::istream & in, std::ostream & out) : yyFlexLexer(in, out) {}
    ~Scanner() = default;

    using FlexLexer::yylex;

    virtual int yylex(  yy::Parser::semantic_type * const lval, yy::Parser::location_type *location );

private:
    yy::Parser::semantic_type * yylval = nullptr;

};

#endif
