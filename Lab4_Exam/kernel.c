// kernel.c file

#define NPROC 9
/*********** in type.h ***************
typedef struct proc{
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    priority;
  int    status;
  int    event;
  int    exitCode;

  struct proc *parent;
  struct proc *child;
  struct proc *sibling;
  
  int    kstack[SSIZE];
}PROC;
***************************************/
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList, *timerQueue;
int procsize = sizeof(PROC);

int body();

int init()
{
  int i; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->ppid = 0;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; 
  freeList = &proc[0];    // freeList = ALL free procs 
  printList("freeList", freeList);
  
  readyQueue = 0;
  sleepList = 0;
  timerQueue = 0;
  
  // creat P0 as running process
  p = dequeue(&freeList); // take proc[0] off freeList
  p->priority = 0;
  p->status = READY;
  p->ppid = 0;
  running = p;           // running -> proc[0] with pid=0

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

int t(int amounOfTime){
  PROC * p = running;
  p->exitCode=amounOfTime;
  //timeEnqueue(&timerQueue, running);
  enqueue(&timerQueue, p);
  p->status=SLEEP;
  tswitch();
  return 0;
}
  
int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  //building fam here
  p->ppid = running->pid;
  p->parent = running;
  p->sibling = running->child;
  p->child = 0;
  running->child = p;
  
  // set kstack for new proc to resume to func()
  // stmfd sp!, {r0-r12, lr} saves regs in stack as
  // stack = lr r12 r11 r10 r9 r8 r7 r6 r5  r4  r3  r2  r1  r0
  // HIGH    -1 -2  -3  -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14   LOW
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // saved regs in dec address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p->pid;
}

int scheduler()
{
  // kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  // kprintf("next running = %d\n", running->pid);
  if (running->pid){
     color = running->pid;
  }
}

// code of processes
int body()
{
  char c, cmd[64];

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    PROC * tempP=running->child;
    printf("proc %d running, ", running->pid);
    printf("parent proc: %d, ",running->ppid);
    printf("child list: ");
    while(tempP){
      printf("[%d,%d]->", tempP->pid, tempP->status);
      tempP=tempP->sibling;
    }
    printf("NULL\n");
    //need to list some print function here
    printTimerList("TimerQueue", timerQueue);
    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    printsleepList(sleepList);

    printf("\nEnter a command [switch|kfork|sleep|wakeup|wait|t|exit] : ");
    kgets(cmd);
    printf("\n");
    
    if (strcmp(cmd, "switch")==0)
      tswitch();
    else if (strcmp(cmd, "kfork")==0){
      kfork((int)body, 1);
    }
    else if (strcmp(cmd, "sleep")==0){
      int event;
      printf("enter sleep event value: ");
      event = geti();
      ksleep(event);
    }
    else if (strcmp(cmd, "wakeup")==0){
      int event;
      printf("enter wakeup event value: ");
      event = geti();
      kwakeup(event);
    }
    else if (strcmp(cmd, "wait")==0){
      int status, pid;
      pid=kwait(&status);
      printf("pid %d\n",pid);
    }
    else if (strcmp(cmd, "t")==0){
      int event;
      printf("how long till it expires: ");
      event = geti();
      t(event);
    }
    else if (strcmp(cmd, "exit")==0){
       kexit(running->pid);
    }
  }
}
