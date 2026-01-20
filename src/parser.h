#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

int CurTok;
static int getNextToken() {
	return CurTok = gettok();
}

ASTNode *parse_expression() {
	ASTNode *node = NULL;

	switch (CurTok) {
		case tok_inc: case tok_dec:
		case tok_rgt: case tok_lft:
		case tok_ech: case tok_get:
			node = create_node(NODE_COMMAND, CurTok);
			getNextToken();
			return node;
		case tok_lpo:
			getNextToken();
			node = create_node(NODE_LOOP, 0);

			while (CurTok != tok_lpc && CurTok != tok_eof) {
				ASTNode *child = parse_expression();
				if (child) {
					add_child(&node->loop_body, child);
				}
			}

			if (CurTok == tok_lpc) {
				getNextToken();
			} else {
				fprintf(stderr, "Error: expected ']' (unclosed loop)\n");
				exit(1);
			}

			// [+] [-]
			if (node->loop_body.size == 1) {
				ASTNode *child = node->loop_body.children[0];
				if (child->type == NODE_COMMAND) {
					if (child->token == '+' || child->token == '-') {
						free_ast(child);
						node->type = NODE_COMMAND;
						node->token = 'Z';
						node->loop_body.children = NULL;
						node->loop_body.size = 0;
						node->loop_body.capacity = 0;
					}
				}
			}

			return node;

		default:
			getNextToken();
			return NULL;
	}
}

ASTNode *parse_program() {
	ASTNode *root = create_node(NODE_LOOP, 0);
	getNextToken();

	while (CurTok != tok_eof) {
		ASTNode *node = parse_expression();
		if (node) {
			add_child(&root->loop_body, node);
		}
	}
	return root;
}

#endif
