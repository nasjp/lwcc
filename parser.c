#include "lwcc.h"

void parse();

void program();      // = stmt*
Node *stmt();        // = expr ";"
Node *expr();        // = assign
Node *assign();      // = equality ("=" assign)?
Node *equality();    // = relational ("==" relational | "!=" relational)*
Node *relational();  // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();         // = mul ("+" mul | "-" mul)*
Node *mul();         // = unary ("*" unary | "/" unary)*
Node *unary();       // = ("+" | "-")? primary
Node *primary();     // = num | ident | "(" expr ")"

// 新しいノードを作成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// 新しいノードを作成する(整数用)
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

// 新しいノードを作成する(識別子用)
Node *new_node_ident(char *ident) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (ident[0] - 'a' + 1) * 8;  // 今は8バイト固定
  return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
// それ以外の場合には偽を返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) return false;
  token = token->next;
  return true;
}

// 次のトークンが識別子のときには、トークンを1つ読み進めてTokenを返す
// それ以外の場合にはNULLを返す
Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *tmp = token;
    token = token->next;
    return tmp;
  }
  return NULL;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める
// それ以外の場合にはエラーを報告する
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) error_at(token->str, "'%s'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す
// それ以外の場合にはエラーを報告する
int expect_number() {
  if (token->kind != TK_NUM) error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

// 次のトークンが入力の終わりの場合は真を返す
// それ以外の場合には偽を返す
bool at_eof() {
  if (token->kind == TK_EOF) return true;
  return false;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

void parse() { program(); }

void program() {
  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("=")) return new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume("<"))
      node = new_node(ND_LT, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+")) return unary();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *ident = consume_ident();

  if (ident) return new_node_ident(ident->str);

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}
