typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"

int main()
{
  int pid, ppid; 
  char line[64];

  pid = getpid();
  //printf("hello");
  while(1){
    int a;
	  u2printf("addr of a = %x\n", &a);
    u2printf("This is proc %d in Umode at %x parent=%d\n",
	                pid, getPA(), getppid());
    umenu();
    u2printf("input a command : ");
    ugets(line); 
  
    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();


    // ADD these syscalls to kernel
    if (strcmp(line, "sleep")==0)
       usleep();
    if (strcmp(line, "wakeup")==0)
       uwakeup();
    if (strcmp(line, "kfork")==0)
       ukfork();
    if (strcmp(line, "exit")==0)
       uexit();
    if (strcmp(line, "wait")==0)
      uwait();
    if (strcmp(line, "fork")==0)
      ufork();
    if (strcmp(line, "exec")==0)
      uexec();
  }
}

int umenu()
{
  uprintf("---------------------------------------------\n");
  u2printf("ps chname switch sleep wakeup kfork exit wait fork exec\n");
  uprintf("---------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  u2printf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  u2printf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  u2printf("input a name string : ");
  ugets(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}


// ADD these syscalls to kernel
int usleep()
{
  int event;
  int pid = getpid();
  if (pid==1){
    u2printf("P1 does not sleep in Umode\n");
    return -1;
  }
  u2printf("enter an event value to sleep on : ");
  event = geti();
  u2printf("proc %d go to sleep in kernel on event=%d\n", pid, event);
  return syscall(5, event, 0, 0);
}

int uwakeup()
{
  int event;
  u2printf("enter an event value to wakeup: ");
  event = geti();
  u2printf("event=%d\n", event);
  return syscall(6,event,0,0);
}

int ukfork()
{
  return syscall(7, "u1", 0, 0);
}

int uexit()
{
  int value;
  u2printf("enter an exit value : ");
  value = geti();
  printf("\n");
  syscall(8, value, 0, 0);
}

int uwait()
{
  int pid, status;
  pid = syscall(9, &status, 0, 0);
  u2printf("pid = %d ", pid);
  if (pid > 0)
    u2printf("status = %x",status);
  u2printf("\n");
}

int ufork()
{
	return syscall(10, 0, 0, 0);
}

int uexec()
{
	u2printf("Give a command string: ");
	char command[300];
	ugetline(command);
	return syscall(11, (int)command, 0, 0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}
int getPA()
{
  return syscall(92,0,0,0);
}
int argc;
char *argv[32];
 
int token(char *line)
{
  int i;
  char *cp;
  cp = line;
  argc = 0;
  
  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           argv[argc++] = cp;         
       while (*cp != ' ' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  argv[argc] = 0;
}
