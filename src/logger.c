#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "9cc.h"

#ifdef _DEBUG
FILE *log_file;

void logger(char *fmt, ...) {
  // 可変長引数を扱うため
  va_list ap;
  va_start(ap, fmt);

  time_t timer;
  struct tm *date;
  char str[256];

  // 時間取得
  timer = time(NULL);
  date = localtime(&timer);
  strftime(str, sizeof(str), "[%Y/%x %H:%M:%S] ", date);
  if ((log_file = fopen(LOG_FILE, "a")) == NULL) {
    error("file open error: log/log.txt");
    exit(EXIT_FAILURE);
  }

  // 文字列結合とフォーマット
  vfprintf(log_file, strcat(str, fmt), ap);
  fprintf(log_file, "\n");
  fclose(log_file);
  return;
}
#endif

char *get_token_name(TokenKind kind) {
  char *token_names[9] = {"TK_RESERVED", "TK_IDENT",  "TK_NUM",
                          "TK_EOF",      "TK_RETURN", "TK_IF",
                          "TK_ELSE",     "TK_WHILE",  "TK_FOR"};
  return token_names[kind];
}

char *get_node_name(NodeKind kind) {
  char *node_names[16] = {"ND_ADD", "ND_SUB",    "ND_MUL",   "ND_DIV",
                          "ND_EQ",  "ND_NE",     "ND_LT",    "ND_LE",
                          "ND_NUM", "ND_ASSIGN", "ND_LVAR",  "ND_RETURN",
                          "ND_IF",  "ND_ELSE",   "ND_WHILE", "ND_FOR"};
  return node_names[kind];
}
