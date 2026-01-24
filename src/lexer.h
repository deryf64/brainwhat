#ifndef LEXER_H
#define LEXER_H

#include <string.h>

enum Token {
	tok_eof = -1,
	
	tok_inc = '+',
	tok_dec = '-',
	tok_rgt = '>',
	tok_lft = '<',
	tok_ech = '.',
	tok_get = ',',
	tok_lpo = '[',
	tok_lpc = ']',
	tok_mvr = '}', // Move data right
	tok_mvl = '{', // Move data left
	tok_szr = 'Z', // Set Zero
	tok_div = 'd', // Divide
	tok_mul = 'm', // Multiply
	tok_sub = 's', // Subtract
	tok_add = 'a', // Add
	tok_sqr = 'q', // Square root
};

typedef struct {
	int col;
	int row;
} CharPosition;

typedef struct {
	CharPosition *positions;
	size_t size;
	size_t capacity;
} PositionStack;

extern int CurTok;
extern CharPosition charPos;
extern PositionStack openBracketStack;

int gettok(void);
void initBracketStack(void);
void pushBracketPosition(CharPosition pos);
CharPosition popBracketPosition(void);
int isBracketStackEmpty(void);
void freeBracketStack(void);

#endif
