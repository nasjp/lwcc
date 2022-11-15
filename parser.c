#include "lwcc.h"

void parse();

void program();      // = stmt*
Node *stmt();        // = expr ";" | "return" expr ";" | "if" "(" expr ")" stmt ("else" stmt)?
Node *expr();        // = assign
Node *assign();      // = equality ("=" assign)?
Node *equality();    // = relational ("==" relational | "!=" relational)*
Node *relational();  // = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();         // = mul ("+" mul | "-" mul)*
Node *mul();         // = unary ("*" unary | "/" unary)*
Node *unary();       // = ("+" | "-")? primary
Node *primary();     // = num | ident | "(" expr ")"

void append_new_lvar(char *name, int len) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->len = len;
  int before_offset = 0;
  if (locals) before_offset = locals->offset;
  lvar->offset = before_offset + 8;
  locals = lvar;
}

// トークンからローカル変数を検索し返却する
// なければNULLを返却する
LVar *find_lvar(Token *token) {
  for (LVar *var = locals; var; var = var->next) {
    bool is = var->len == token->len && !memcmp(token->str, var->name, var->len);
    if (is) return var;
  }
  return NULL;
}

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
  Node *node = new_node(ND_NUM, NULL, NULL);
  node->val = val;
  return node;
}

// 新しいノードを作成する(ローカル変数用)
Node *new_node_lvar(int offset) {
  Node *node = new_node(ND_LVAR, NULL, NULL);
  node->offset = offset;
  return node;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す
// それ以外の場合には偽を返す
bool consume(char *op) {
  bool is = token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len);
  if (is) return false;
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
  bool is = token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len);
  if (is) error_at(token->str, "'%s'ではありません", op);
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

void parse() { program(); }

void program() {
  Node head;
  Node *cur = &head;

  while (!at_eof()) {
    cur->next = stmt();
    cur = cur->next;
  }

  node = head.next;
}

Node *stmt() {
  if (consume("return")) {
    Node *node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
    return node;
  }

  if (consume("if")) {
    Node *node = new_node(ND_IF, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else")) {
      node->els = stmt();
    }
    return node;
  }

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
  if (ident) {
    LVar *lvar = find_lvar(ident);
    if (lvar) {
      return new_node_lvar(lvar->offset);
    } else {
      append_new_lvar(ident->str, ident->len);
      return new_node_lvar(locals->offset);
    }
  };

  return new_node_num(expect_number());
}
