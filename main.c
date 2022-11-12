#include "lwcc.h"

Token *token;

char *user_input;

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];

  token = tokenize(argv[1]);
  Node *node = expr();
  generate(node);

  return 0;
}
