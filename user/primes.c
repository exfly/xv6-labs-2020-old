#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int primes(int inputch)
{
  int divide, temp;
  if (read(inputch, &divide, sizeof(int)) <= 0)
  {
    exit(0);
  }
  printf("prime %d\n", divide);

  int outputpipe[2];
  if (pipe(outputpipe) < 0)
  {
    fprintf(2, "pipe\n");
  }

  if (fork() == 0)
  {
    close(outputpipe[1]);
    primes(outputpipe[0]);
  }
  else
  {
    int status;
    // 爸爸要等儿子把事情做完
    close(outputpipe[0]);
    while (read(inputch, &temp, sizeof(int)) > 0)
    {
      if (temp % divide != 0)
      {
        write(outputpipe[1], &temp, sizeof(int));
      }
    }
    close(outputpipe[1]);
    close(inputch);
    wait(&status);
    exit(0);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  // 准备向下传递的 chan
  int outputpipe[2];
  pipe(outputpipe);

  if (fork() == 0)
  {
    close(outputpipe[1]);
    // 儿子需要接收 chan，消费数据
    primes(outputpipe[0]);
  }
  else
  {
    int status;
    close(outputpipe[0]);
    // 爸爸 需要向下传递第一次循环的素数
    for (int i = 2; i < 32; i++)
    {
      write(outputpipe[1], &i, sizeof(int));
    }
    close(outputpipe[1]);
    wait(&status);
    exit(0);
  }

  exit(0);
}
