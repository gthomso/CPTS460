// wait.c file

extern PROC *running;
extern PROC *sleepList;

int kexit(int exitVal)  // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  //assigning to running parents
  PROC *p=running->child;
  PROC *temp;
  PROC *p1=running;
  //if pid is 1
  if(running->pid==1){
    init();
  }
  while(p1->pid!=1){
    p1=p1->parent;
  }
  while(p){
    //printf("wow\n");
    p->ppid = p1->pid;
    p->parent = p1;
    temp = p->sibling;
    p->sibling = p1->child;
    p1->child = p;

    p=temp;
  }
  running->exitCode = exitVal;
  running->status = ZOMBIE;

  if(running->parent->status==SLEEP){
    kwakeup(running->parent->pid);
  }

  tswitch();
}

//ksleep
int ksleep(int event)
{
  int SR = int_off();
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList,running);
  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  int SR=int_off();
  PROC * p, *temp;
  int index=0;
  // sleepList will probably never have more than 10
  while(p=dequeue(&sleepList)){
    if(p->event==event){
      p->status = READY;
      //printf("proc being reinserted in readylist %d\n", p->pid);
      enqueue(&readyQueue,p);
      //break;
    }
    else{
      //printf("proc being reinserted in sleeplist %d\n", p->pid);
      enqueue(&sleepList,p);//was sleeplist
    }
  }
  int_on(SR);
}

int kwait(int *status){
  PROC * tempP = running->child;
  if (!running->child){
    //this is an error
    return -1;
  }
  while(1){
    //printf("\noon\n");
    tempP=running->child;
    while((tempP->status!=ZOMBIE) && tempP){
      //printf("\nbloom\n");
      tempP = tempP->sibling;
    }
    if(tempP->status == ZOMBIE){
      //printf("\ntest\n");
      *status = tempP->exitCode;
      enqueue(&freeList,tempP);
      if(running->child==tempP){
        running->child=tempP->sibling;
      }
      else{
        //do this when we need to
      }
      return tempP->pid;
    }
    ksleep(running->pid);
  }
}
