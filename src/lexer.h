#ifndef LEXER_H
#define LEXER_H

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

	tok_szr = 'Z',
};

static int gettok() {
	static int LastChar = ' ';

	while (LastChar == ' ') {
		LastChar = getchar();
	}

	if (LastChar == '%') {
		do {
			LastChar = getchar();
		} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
		
		if (LastChar != EOF) {
			return gettok();
		}
	}

	if (LastChar == EOF) {
		return tok_eof;
	}

	if (LastChar == '+' || LastChar == '-' ||
	    LastChar == '>' || LastChar == '<' ||
	    LastChar == '.' || LastChar == ',' ||
	    LastChar == '[' || LastChar == ']' ||
	    LastChar == 'Z') {
		int ThisChar = LastChar;
		LastChar = getchar();
		return ThisChar;
	}

	LastChar = getchar();
	return gettok();
}

extern int CurTok;

#endif
