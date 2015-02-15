#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int nSig1 = -1;
int pPid = -1;

void
signalHandler(int sig) {
  //ops
  switch (sig) {
  case 10:
    fprintf(stdout,"recieved SIGUSR1\n");
    break;
  case 14:
    if (nSig1 == 0) {
      fprintf(stdout,"Child sending signal 12\n");
      kill(pPid,12);
    }
    fprintf(stdout,"Child sending signal 10\n");
    kill(pPid,10);
    nSig1--;
    break;
  }
}

int
childProcess(char *id,int nSig,int parentPid)
{
  //ops
  fprintf(stdout,"Child Process ID: %s PID: %d Parent PID:%d\nnSig1 = %d\n",id, getpgrp(),parentPid,nSig);
  nSig1 = nSig;
  pPid = parentPid;
  for (;;) {
    alarm(5);
    sleep(10);
    fprintf(stdout,"Child dormant for 5 seconds\n");
  }
  return 1;
}
