#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int printpath(char *path)
{
  printf("%s\n", path);
  return 0;
}

int find(char *path, char *shortpath, char *expr)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s, fd: %d\n", path, fd);
    return 0;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  switch (st.type)
  {
  case T_FILE:
    // 如果包含 expr，则 print
    if (!strcmp(shortpath, expr))
    {
      printpath(path);
    }
    break;
  case T_DIR:
    // 递归下去查找
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;

      char *name = de.name;
      if (!strcmp(name, ".") || !strcmp(de.name, ".."))
      {
        continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name, expr);
    }
    break;
  default:
    break;
  }
  close(fd);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("args too short\n");
    exit(0);
  }
  if (argc >= 3)
  {
    find(argv[1], argv[2], argv[2]);
  }
  exit(0);
}
