#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

static int CurTok;
static int getNextToken() {
	return CurTok = gettok();
}

ASTNode* parse_expression() {
	ASTNode* node = NULL;

	switch (CurTok) {
		case tok_inc: case tok_dec:
		case tok_rgt: case tok_lft:
		case tok_ech: case tok_get:
			node = create_node(NODE_COMMAND, CurTok);
			getNextToken();
			return node;
		case tok_lpo:
			getNextToken();
			node = create_node(NODE_LOOP, tok_lpo);

			while (CurTok != tok_lpc && CurTok != tok_eof) {
				ASTNode* child = parse_expression();
				if (child) {
					add_to_loop(node, child);
				}
			}

			if (CurTok != tok_lpc) {
				fprintf(stderr, "Ошибка: ожидалась ']' \n");
			}

			getNextToken();
			return node;

		default:
			getNextToken();
			return NULL;
	}
}

ASTNode* parse_program() {
	ASTNode* root = create_node(NODE_LOOP, 'R');
	getNextToken();

	while (CurTok != tok_eof) {
		ASTNode* node = parse_expression();
		if (node) {
			add_to_loop(root, node);
		}
	}
	return root;
}

#endif
