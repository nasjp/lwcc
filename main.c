#include "lwcc.h"

char *user_input;
Token *token;
Node *code[100];
LVar *locals;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  tokenize();
  parse();
  generate();

  return 0;
}
