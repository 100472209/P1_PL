/*Jorge García de Marina Metola  Rodrigo Hernández Bonache  grupo 40*/
/*100472344@alumnos.uc3m.es 100472209@alumnos.uc3m.es*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T_NUMBER 1001
#define T_OPERATOR 1002

char* ParseExpression(); // Prototype for forward reference

struct token_st {
  int token;          // Here we store the current token/literal
  int old_token; // Sometimes we need to check the previous token
  int number;         // The value of the number
  int old_number;
  int token_val; // or the arithmetic operator
  int old_token_val;
};

struct token_st myToken;

int line_counter = 1;

void update_old_token() { // Sometimes we need to check the previous token
  myToken.old_token = myToken.token;
  myToken.old_number = myToken.number;
  myToken.old_token_val = myToken.token_val;
}

int rd_lex() {
  int c;

  do {
    c = getchar();
    if (c == '\n')
      line_counter++; // info for rd_syntax_error()
  } while (c == ' ' || c == '\t');

  if (isdigit(c)) {
    ungetc(c, stdin);
    update_old_token();
    scanf("%d", &myToken.number);
    myToken.token = T_NUMBER;
    return (myToken.token); // returns the Token for Number
  }

  if (c == '+' || c == '-' || c == '*' || c == '/') {
    update_old_token();
    myToken.token_val = c;
    myToken.token = T_OPERATOR;
    return (myToken.token);
  } // returns the Token for Arithmetic Operators

  if (c == EOF) {                         // Detect End Of FIle
    printf ("\nbye\n") ;        // Force an exit to avoid
    exit (0) ;                         // a Syntax Error
  }

  update_old_token();
  myToken.token = c;
  return (myToken.token); // returns a literal
}

void rd_syntax_error(int expected, int token, char *output) {
  fprintf(stderr, "ERROR in line %d. ", line_counter);
  fprintf(stderr, output, expected, token);

  printf("\nbye\n");
  exit(0);
}

void MatchSymbol(int expected_token) {
  if (myToken.token != expected_token) {
    rd_syntax_error(expected_token, myToken.token,
                    "token %d expected, but %d was read\n");
  }
}

#define ParseLParen()                                                          \
  MatchSymbol('('); // More concise and efficient definitions
#define ParseRParen() MatchSymbol(')'); // rather than using functions
// This is only useful for matching Literals

int ParseNumber() // Parsing Non Terminals and some Tokens require more
{                 // complex functions
  MatchSymbol(T_NUMBER);
  return myToken.number;
}

/*
 Grammar:
    Axioma -> Expresion \n
    Expresion -> (Expresion’ | Numero | Variable
    Expresion’ -> Operador Expresion Expresion) | = Variable Expresion)
    Operador -> + | - | * | /
    Numero -> 0-9
    Variable -> A-Z
 */

char* ParseExpressionRest() { // Expresion’ -> Operador Expresion Expresion) | = Variable Expresion)
  char* result = malloc(100);
  char* param1;
  char* param2;
  if (myToken.token == T_OPERATOR) {
    char operator = myToken.token_val;
    rd_lex();
    param1 = ParseExpression();
    rd_lex();
    param2 = ParseExpression();
    rd_lex();
    ParseRParen();
    sprintf(result, "%s%s%c ", param1, param2, operator);
  } else if (myToken.token == '=') {
    rd_lex();
    char variable = myToken.token;
    rd_lex();
    param1 = ParseExpression();
    rd_lex();
    ParseRParen();
    sprintf(result, "%sdup %c ! ", param1, variable);
  } else {
    rd_syntax_error(T_OPERATOR, myToken.token, "token %d expected, but %d was read");
  }
  return result;
}

char* ParseExpression() { // Expresion -> (Expresion’ | Numero | Variable
  char* result = malloc(100);
  if (myToken.token == T_NUMBER) {
    sprintf(result, "%d ", ParseNumber());
  } else if (isalpha(myToken.token)) {
    sprintf(result, "%c @ ", myToken.token);
  } else if (myToken.token == '(') {
    ParseLParen();
    rd_lex();
    result = ParseExpressionRest();
  } else {
    rd_syntax_error('(', myToken.token, "token %d expected, but %d was read");
  }
  return result;
}

char* ParseAxiom() { // Axioma -> Expresion \n
  char* result = ParseExpression();
  rd_lex();
  MatchSymbol('\n');
  // Add a dot to the end of the string
  strcat(result, ".");
  return result;
}


int main (int argc, char **argv)
{
  // Usage :  drLL -s  ==> evaluate a single Input Line
  //          drLL     ==> evalute multiple Input Lines until some error appears

  int flagMultiple = 1 ;

  if (argc >= 2) {
    if (strcmp ("-s", argv [1]) == 0) {
      flagMultiple = 0 ;
    }
  }

  do {
    rd_lex () ;
    char* result = ParseAxiom();
    int printed[26] = {0}; // Array to keep track of printed variables
    for (int i = 0; result[i] != '\0'; i++) {
      if (isupper(result[i]) && !printed[result[i] - 'A']) {
        printf("variable %c\n", result[i]);
        printed[result[i] - 'A'] = 1; // Mark the variable as printed
      }
    }
    printf("%s\n", result);
  } while (flagMultiple);

  printf("\nbye\n");
}


