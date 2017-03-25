/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE)) {
  	readChar();
  }
}

void skipComment() {
  // TODO
  while (currentChar != EOF) {
  	if (charCodes[currentChar] == CHAR_TIMES) {
  		readChar ();
  		if (charCodes[currentChar] == CHAR_RPAR) {readChar (); break;}
  	}
  	readChar ();
  }
}

Token* readIdentKeyword(void) {
  // TODO
  Token * ident;
  char string[MAX_IDENT_LEN + 1];

  int i = 0;
  int linePivot = lineNo, colPivot = colNo;
  
  while ((currentChar != EOF)) {
  	CharCode ch = charCodes[currentChar];
  	if ((ch == CHAR_SPACE) 
  			|| (ch == CHAR_SEMICOLON)
  			|| (ch == CHAR_COLON)
  			|| (ch == CHAR_PERIOD)
  			|| (ch == CHAR_LPAR)
  			|| (ch == CHAR_RPAR)
  			|| (ch == CHAR_COMMA)
			|| (ch == CHAR_EQ)
			|| (ch == CHAR_LT)
			|| (ch == CHAR_GT)
			|| (ch == CHAR_PLUS)
			|| (ch == CHAR_MINUS)
			|| (ch == CHAR_TIMES)
			|| (ch == CHAR_SLASH)
			|| (ch == CHAR_SINGLEQUOTE)
			|| (ch == CHAR_EXCLAIMATION))
  		break;
  	string[i] = currentChar;
  	i++;
  	readChar();
  }
  
  string[i] = '\0';
  
  // if ident is a keyword
  if (checkKeyword(string) != TK_NONE)
  	ident = makeToken(checkKeyword(string), linePivot, colPivot);
  else 
  	ident = makeToken(TK_IDENT, linePivot, colPivot);
  strcpy (ident->string, string);
  
  return ident;
}

Token* readNumber(void) {
  // TODO
  Token * number;
  
  int linePivot = lineNo, colPivot = colNo;
  char string[MAX_IDENT_LEN + 1];
  int i = 0;
  while (charCodes[currentChar] == CHAR_DIGIT) {
  	string[i] = currentChar;
  	i++;
  	readChar();
  }
  string[i] = '\0';
  
  number = makeToken (TK_NUMBER, linePivot, colPivot);
  strcpy (number->string, string);
  
  return number;
}

Token* readConstChar(void) {
  // TODO
  Token * constChar;
  
  int linePivot = lineNo, colPivot = colNo;
  int len = 20;
  int i = 0;
  char * string = (char *) malloc (len * sizeof(char));
  
  readChar();
  
  while (charCodes[currentChar] != CHAR_SINGLEQUOTE) {
  	string[i] = currentChar;
  	i++;
  	if (i >= len) {
  		char * temp = (char *) malloc (len * sizeof(char));
  		strcpy(temp, string);
  		free(string);
  		
   		len *= 2;
  		string = (char * ) malloc (len * sizeof(char));
  		strcpy(string, temp);
  		free(temp);
  	}
  	readChar();
  }
  string[i] = '\0';
  
  constChar = makeToken (TK_CHAR, linePivot, colPivot);
  strcpy(constChar->string, string);
  readChar();
  return constChar;
}

Token * readSymbol (void) {
	Token * token;
	int ln, cn;
	switch (charCodes[currentChar]) {
	case CHAR_PLUS: 		token = makeToken (SB_PLUS, lineNo, colNo); break;
	case CHAR_SEMICOLON:	token = makeToken (SB_SEMICOLON, lineNo, colNo); break;
	case CHAR_PERIOD: 		token = makeToken (SB_PERIOD, lineNo, colNo); break;
	case CHAR_COLON:
	ln = lineNo; cn = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_EQ)
		token = makeToken (SB_ASSIGN, lineNo, colNo);
	else 
		token = makeToken (SB_COLON, ln, cn); 
	break;
	case CHAR_RPAR:			token = makeToken (SB_RPAR, lineNo, colNo); break;
	case CHAR_LPAR:			token = makeToken (SB_LPAR, lineNo, colNo); break;
	case CHAR_EQ: 			token = makeToken (SB_EQ, lineNo, colNo); break;
	case CHAR_TIMES: 		token = makeToken (SB_TIMES, lineNo, colNo); break;
	case CHAR_MINUS:		token = makeToken (SB_MINUS, lineNo, colNo); break;
	case CHAR_COMMA:		token = makeToken (SB_COMMA, lineNo, colNo); break;
	case CHAR_EXCLAIMATION: 
	ln = lineNo; cn = colNo;
	readChar();
	if (charCodes[currentChar] != CHAR_EQ) {
		token = makeToken(TK_NONE, lineNo, colNo);
		error(ERR_INVALIDSYMBOL, lineNo, colNo);
		readChar(); 
		return token;
	}
	token = makeToken (SB_NEQ, ln, cn);
	break;

	default:
		break;
	} 
	
	if (token->tokenType != SB_COLON) 
  		readChar();
  		
  	return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_SINGLEQUOTE: return readConstChar();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    // ....
    // TODO
    // ....
  case CHAR_LPAR:
  	ln = lineNo; cn = colNo;
  	readChar(); 
  	if (charCodes[currentChar] == CHAR_TIMES) 
  		skipComment();
  	else return makeToken (SB_LPAR, ln, cn);
  	return getToken();
  case CHAR_RPAR:
  case CHAR_TIMES:
  case CHAR_SEMICOLON:
  case CHAR_PERIOD: 
  case CHAR_COLON: 
  case CHAR_EQ:
  case CHAR_EXCLAIMATION:
  case CHAR_COMMA:
  case CHAR_MINUS: return readSymbol();
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



