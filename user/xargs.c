#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define MAXARG 32
#define WORDSIZE 512

char word_store[MAXARG][WORDSIZE];

int
run_command(char *base[], int basec, int nw)
{
  char *args[MAXARG];
  int i, pid, status;

  for (i = 0; i < basec; i++)
    args[i] = base[i];

  for (i = 0; i < nw; i++)
    args[basec + i] = word_store[i];

  args[basec + nw] = 0;

  pid = fork();
  if (pid < 0) {
    fprintf(2, "xargs: fork failed\n");
    return 1;
  }

  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "xargs: exec %s failed\n", args[0]);
    exit(1);
  }

  if (wait(&status) < 0 || status != 0)
    return 1;

  return 0;
}

int
main(int argc, char *argv[])
{
  int n = 0;
  int cmd = 1;

  int basec, limit, nw = 0, len = 0;
  int ch, r, failed = 0;
  char *base[MAXARG];
  char word[WORDSIZE];

  if (argc < 2) {
    fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
    exit(1);
  }

  if (strcmp(argv[1], "-n") == 0) {
    if (argc < 4) {
      fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
      exit(1);
    }

    if (argv[2][0] == '\0') {
      fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
      exit(1);
    }

    for (int i = 0; argv[2][i] != '\0'; i++) {
      if (argv[2][i] < '0' || argv[2][i] > '9') {
        fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
        exit(1);
      }

      n = n * 10 + argv[2][i] - '0';
    }

    if (n < 1) {
      fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
      exit(1);
    }

    cmd = 3;
  }

  basec = argc - cmd;

  if (basec >= MAXARG) {
    fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
    exit(1);
  }

  for (int i = 0; i < basec; i++)
    base[i] = argv[cmd + i];

  limit = n == 0 ? MAXARG - basec : n;
  if (limit > MAXARG - basec)
    limit = MAXARG - basec;

  while ((r = read(0, &ch, 1)) == 1) {
    if (ch == ' ' || ch == '\t' || ch == '\n') {
      if (len > 0) {
        word[len] = '\0';

        if (nw >= limit) {
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
        
        strcpy(word_store[nw], word);
        nw++;
        len = 0;

        if (n > 0 && nw == limit) {
          if (run_command(base, basec, nw))
            failed = 1;
          nw = 0;
        }
      }

      if (ch == '\n' && n == 0 && nw > 0) {
        if (run_command(base, basec, nw))
          failed = 1;
        nw = 0;
      }
    } else {
      if (len >= WORDSIZE - 1) {
        fprintf(2, "xargs: input word too long\n");
        exit(1);
      }
      word[len++] = ch;
    }
  }

  if (r < 0) {
    fprintf(2, "xargs: read failed\n");
    exit(1);
  }

  if (len > 0) {
    word[len] = '\0';
    strcpy(word_store[nw], word);
    nw++;
  }

  if (nw > 0) {
    if (run_command(base, basec, nw))
      failed = 1;
  }

  exit(failed);


  exit(0);
}
