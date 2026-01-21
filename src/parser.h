#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

extern int CurTok;

int getNextToken(void);
ASTNode *parse_expression(void);
ASTNode *parse_program(void);

#endif
