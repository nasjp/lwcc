#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------- トークナイザー
// トークンの種類
typedef enum {
  TK_RESERVED,  // 記号
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン
struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  int len;         // トークンの長さ
};

// 現在着目しているトークン(グローバル変数)
extern Token *token;

// トークナイズする
Token *tokenize(char *p);

// ------------------------- パーサー
// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_EQ,   // ==
  ND_NE,   // !=
  ND_LT,   // <
  ND_LE,   // <=
  ND_NUM,  // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノード
struct Node {
  NodeKind kind;  // ノードの型
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
};

// パースする
Node *expr();

// ------------------------- ジェネレーター
// 抽象構文木を下りながらコードを生成する
void generate(Node *node);

// ------------------------- ユーティリティ
// 入力プログラム(エラー箇所報告用(グローバル変数))
extern char *user_input;

// エラーを報告する
// printfと同じ引数を取る
void error(char *fmt, ...);

// エラー箇所を報告する
// ポインタ（エラー箇所)とprintfと同じ引数を取る
void error_at(char *loc, char *fmt, ...);
