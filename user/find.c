#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int 
match(char *name, char *pattern)
{
    if (*pattern == '\0')
        return *name == '\0';

    if (*pattern == '*') {
        return match(name, pattern + 1) ||
            (*name != '\0' && match(name + 1, pattern));
    }

    return *name == *pattern &&
            match(name + 1, pattern + 1);
}


void
find(char *path, char *pattern, int type)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }

  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
    fprintf(2, "find: path too long\n");
    close(fd);
    exit(1);
  }

  strcpy(buf, path);
  p = buf + strlen(buf);

  if (p == buf || p[-1] != '/')
    *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
      continue;

    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      close(fd);
      exit(1);
    }

    if (match(p, pattern) &&
        (type == 0 ||
         (type == T_DIR && st.type == T_DIR) ||
         (type == T_FILE &&
          (st.type == T_FILE || st.type == T_DEVICE)))) {
      printf("%s\n", buf);
    }

    if (st.type == T_DIR)
      find(buf, pattern, type);
  }

  close(fd);
}


int
main(int argc, char *argv[])
{
  int type = 0;

  if (argc != 3 && argc != 5) {
    fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
    exit(1);
  }

  if (argc == 5) {
    if (strcmp(argv[3], "-type") != 0) {
      fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
      exit(1);
    }

    if (strcmp(argv[4], "f") == 0)
      type = T_FILE;
    else if (strcmp(argv[4], "d") == 0)
      type = T_DIR;
    else {
      fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
      exit(1);
    }
  }

  find(argv[1], argv[2], type);
  exit(0);
}
