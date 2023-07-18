#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "9cc.h"

#ifdef _DEBUG
FILE *log_file;

void logger(char *fmt, ...) {
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

  // 文字列結合
  fprintf(log_file, strcat(str, fmt));
  fclose(log_file);
  return;
}
#endif