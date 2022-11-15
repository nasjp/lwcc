#include "lwcc.h"

bool is_alnum(char c) {
  return isalpha(c) || isdigit(c) || c == '_';
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 新しいトークン(識別子)を作成してcurに繋げる
Token *new_token_ident(Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = TK_IDENT;
  tok->str = str;
  int len = 1;
  while (is_alnum(str[len])) len++;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool starts_with(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

bool is_keyword(char *p, char *q) {
  int len = strlen(q);
  return strncmp(p, q, len) == 0 && !is_alnum(p[len]);
}

Token *tokenize_keyword(char **p, Token *token) {
  char *keywords[] = {"return", "if", "else"};
  for (size_t i = 0; i < (sizeof(keywords) / sizeof(char *)); i++) {
    if (is_keyword(*p, keywords[i])) {
      token = new_token(TK_RESERVED, token, *p, strlen(keywords[i]));
      *p += token->len;
      return token;
    }
  }
  return token;
}

void tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    char *q = p;
    cur = tokenize_keyword(&p, cur);
    if (q != p) {
      continue;
    }

    if (starts_with(p, "==") || starts_with(p, "!=") || starts_with(p, "<=") || starts_with(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()><=;{}", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    if (isalpha(*p)) {
      cur = new_token_ident(cur, p);
      p += cur->len;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  token = head.next;
}
