#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char ping[] = "ping";
char pong[] = "pong";

int pingpong()
{
  int req_pipe[2];
  int resp_pipe[2];
  pipe(req_pipe);
  pipe(resp_pipe);

  if (fork() == 0)
  {
    char buf[5];
    // received ping request
    close(req_pipe[1]);
    read(req_pipe[0], buf, sizeof buf);
    close(req_pipe[0]);
    printf("%d: received %s\n", getpid(), buf);

    // response pong
    close(resp_pipe[0]);
    write(resp_pipe[1], pong, sizeof pong);
  }
  else
  {
    // request ping first
    close(req_pipe[0]);
    write(req_pipe[1], ping, 12);
    close(req_pipe[1]);

    // received pong
    char buf[5];
    close(resp_pipe[1]);
    read(resp_pipe[0], buf, sizeof buf);
    close(resp_pipe[0]);
    printf("%d: received %s\n", getpid(), buf);
  }
  return 0;
}

int main(int argc, char *argv[])
{

  pingpong();

  exit(0);
}
