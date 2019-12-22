#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED, // operator
  TK_NUM, // integer
  TK_EOF, // eof
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind; // token type
  Token *next; // next token
  int val; // value (TK_NUM)
  char *str; // token string
};

Token *token;
char *user_input;

// a function to report error
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// a function to report error with location
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos * " "
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// If the token equals to given operator,
// set the pointer to the next token and return true.
// Otherwise, return false.
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) return false;
  token = token->next;
  return true;
}

// If the token equals to given operator,
// set the pointer to the next token.
// Otherwise, return error.
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) error_at(token->str, "is not '%c'", op);
  token = token->next;
}

// If the token is a number,
// set the pointer to the next token and return the number.
// Otherwise, return error.
int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "is not a number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// create a new token and set it to the next of `cur`
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *t = calloc(1, sizeof(Token));
  t->kind = kind;
  t->str = str;
  cur->next = t;
  return t;
}

Token *tokenize(char *p) {
  Token head; // dummy token
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    // skip spaces
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("could not tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main (int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "invalid args number\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // the first character should be a number
  printf("  mov rax, %d\n", expect_number());

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
