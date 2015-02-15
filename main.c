#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 1

int exitVal = 0;
int globalIncrement;
int childPid = -1;
int offset = 0;
char *id = "Signalee";
sigjmp_buf buf,buf2;

void
keyboardInterruptHandler(int sig)
{
  //variables
  int parentPid,newIncrement;
  char input;
  //ops
  fprintf(stdout,"\nSig detected\n");
  fprintf(stdout,"Current Options---\n");
  fprintf(stdout,"\ta--quit program\n\tb--resume counting with same increment\n\tc--Change the increment and resume counting\n");
  fprintf(stdout,"\td--reset the count\te--create child process\n");
  fprintf(stdout,"Enter input: ");
  fscanf(stdin,"%c",&input);
  switch (input) {
  case 'a':
    printf("Exiting Now\n");
    exitVal++;
    return;
  case 'b':
    if (DEBUG) fprintf(stdout,"Case b\n");
    break;
  case 'c':
    if (DEBUG) fprintf(stdout,"Case c\n");
    fprintf(stdout,"Enter new increment:");
    fscanf(stdin,"%d",&globalIncrement);
    break;
  case 'd':
    if (DEBUG) fprintf(stdout,"Case d\n");
    longjmp(buf,3);
    break;
  case 'e':
    fprintf(stdout,"Creating Child\n");
    childPid = fork();
    parentPid = getpgrp();
    execlp("childProcess.o","5",id,parentPid,NULL);
    break;
  default:
    fprintf(stdout,"Input not recognized\n");
    keyboardInterruptHandler(sig);
    return;
  }
}

void
faultHandler(int sig)
{
  //ops
  switch (sig) {
  case 8:
    fprintf(stdout,"INF ");
    offset++;
    return;
  case 11:
    fprintf(stderr,"Segmentation Fault Detected\n");
    longjmp(buf2,3);
    return;
  default:
    fprintf(stdout,"Sig: %d\n",sig);
    exitVal++;
    return;
  }
}

void
childSignalHandler(int sig) {
  //variables
  int *status;
  //ops
  switch (sig) {
  case 10:
    fprintf(stdout,"Sig: %d\nSending kill\n",sig);
    kill(childPid,10);
    break;
  case 12:
    fprintf(stdout,"Sig: %d\nSending kill\n",sig);
    kill(childPid,9);
    break;
  case 17:
    wait(status);
    fprintf(stdout,"Sig: %d\nStatus: %d\n",sig,*status);
    break;
  }
}

int
main(int argc,char **argv)
{
  //variables
  int i;
  int localCounter;
  int globalIncrement;
  short numbers[6];
  char words[6];
  char *name = "SignalName";
  int *pointer;
  //ops
  if (argc != 2) {
    fprintf(stdout,"Usage ./signals <SleepTime>\n");
    return 0;
  }
  sigsetjmp(buf,3);
  localCounter = 0;
  globalIncrement = 0;
  signal(2,keyboardInterruptHandler);
  signal(8,faultHandler);
  signal(11,faultHandler);
  signal(10,childSignalHandler);
  signal(12,childSignalHandler);
  signal(17,childSignalHandler);
  globalIncrement++;
  for (i=0;i<5;i++) {
    numbers[i] = i + 100;
    words[i] = name[i];
  }
  fprintf(stdout,"Begin Counting\n");
  for (;;) {
    if (exitVal)
      return 0;
    fprintf(stderr,"words[%d] = 0x%x = %d = %c  ",localCounter,words[localCounter],words[localCounter],words[localCounter]);
    fprintf(stderr,"1.0/%d = %f\n",(words[localCounter]+offset),(float)(1/(words[localCounter]+offset)));
    offset = 0;
    localCounter += globalIncrement;
    sigsetjmp(buf2,3);
    sleep(3);
  }
  return 0;
}

