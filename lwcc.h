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
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

// トークン
typedef struct Token Token;

struct Token {
  TokenKind kind;  // トークンの型
  Token *next;     // 次の入力トークン
  int val;         // kindがTK_NUMの場合、その数値
  char *str;       // トークン文字列
  size_t len;      // トークンの長さ
};

// ローカル変数
typedef struct LVar LVar;

struct LVar {
  LVar *next;  // 次の変数かNULL
  char *name;  // 変数の名前
  size_t len;  // 名前の長さ
  int offset;  // RBPからのオフセット
};

// 入力文字列をトークナイズする
void tokenize();

// ------------------------- パーサー
// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,     // +
  ND_SUB,     // -
  ND_MUL,     // *
  ND_DIV,     // /
  ND_EQ,      // ==
  ND_NE,      // !=
  ND_LT,      // <
  ND_LE,      // <=
  ND_ASSIGN,  // =
  ND_LVAR,    // ローカル変数
  ND_NUM,     // 整数
  ND_RETURN,  // return
  ND_IF,      // if
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノード
struct Node {
  NodeKind kind;  // ノードの型
  Node *next;     // Next node
  Node *lhs;      // 左辺
  Node *rhs;      // 右辺

  // if
  Node *cond;
  Node *then;
  Node *els;

  int val;     // kindがND_NUMの場合のみ使う
  int offset;  // kindがND_LVARの場合のみ使う
};

// トークンをノードにパースする
void parse();

// ------------------------- ジェネレーター
// ノードからアセンブリを生成する
void generate();

// ------------------------- グローバル変数
// 入力プログラム
extern char *user_input;

// 現在着目しているトークン
extern Token *token;

// プログラムのノード群
extern Node *node;

// ローカル変数
extern LVar *locals;

// ------------------------- ユーティリティ
// エラーを報告する
// printfと同じ引数を取る
void error(char *fmt, ...);

// エラー箇所を報告する
// ポインタ（エラー箇所)とprintfと同じ引数を取る
void error_at(char *loc, char *fmt, ...);
