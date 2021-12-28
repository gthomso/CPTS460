// kernel.c file

extern PROC *getproc();
extern PROC *dequeue();
extern int pipe_writer(), pipe_reader();
extern PIPE *kpipe;

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);
int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];
  sleepList = 0;
  readyQueue = 0;

  running = dequeue(&freeList);
  running->priority = 0;
  //kprintf("running = %d\n", running->pid);
}

int scheduler()
{
  //kprintf("proc %d in scheduler\n", running->pid);
  if (running->status==READY)
     enqueue(&readyQueue, running);
  //printQ(readyQueue);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);
  color = RED + running->pid;
}  

int ksleep(int event)
{
  int sr = int_off();
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);

  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  int sr = int_off();
  while((p = dequeue(&sleepList))!=0){
    if (p->event==event){
       p->status = READY;
       enqueue(&readyQueue, p);
    }
    else{
       enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
  int_on(sr);
}

int kexit()
{
  //  printf("proc %d kexit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}
  
int kfork(int (*func)(), int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("kfork failed\n");
    return -1;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = priority;

  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
    p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);

  return p->pid;
}
