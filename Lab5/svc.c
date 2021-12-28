/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
//#include "type.h"
extern PROC proc[], *running;
extern int tswitch();

int ktswitch()
{
  tswitch();
}

int kgetpid()
{
  return running->pid;
}

int kgetppid()
{
  return running->ppid;
}

char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  PROC *p;
  for(int i=0;i<NPROC;i++){
    p=&proc[i];
    printf("proc[%d]: pid=%d, ppid=%d, name=%s\t status=",i,p->pid,p->ppid,p->name);
    if(p==running){
      printf("RUN\n");
    }
    else{
      printf("%s\n", pstatus[p->status]);
    }
  }
}

int kchname(char *s)
{ 
  printf("Changed name to %s\n", s);
  strcpy(running->name, s);
}

int kgetPA()
{
  //This is where physical Adress is put
  return running->pgdir[2048];
}

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
  printf("proc %d running, ", running->pid);
  printf("parent proc: %d, ",running->ppid);
  printf("child list: ");
  while(tempP){
    printf("[%d,%d]->", tempP->pid, tempP->status);
    tempP=tempP->sibling;
  }
  printf("NULL\n");
  tempP = running->child;
  if (!running->child){
    //this is an error
    printf("welp\n\n");
    return -1;
  }
  while(1){
    //printf("\noon\n");
    tempP=running->child;
    while((tempP->status!=ZOMBIE) && tempP){
      printf("\nbloom\n");
      tempP = tempP->sibling;
    }
    if(tempP->status == ZOMBIE){
      printf("\ntest\n");
      *status = tempP->exitCode;
      tempP->status = FREE;
      enqueue(&freeList,tempP);
      tempP->priority=0;
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
  tswitch();
}

int kexit(int exitVal)  // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  //assigning to running parents
  PROC *p=running->child;
  PROC *temp;
  PROC *p1=running;
  //if pid is 1
  if(running->pid==1){
    printf("Operation failed cant exit p1\n");
    return 0;
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
    running->status = FREE;
    enqueue(&freeList,running);

  }

  tswitch();
}


int svc_handler(int a, int b, int c, int d)
{
  int r;
  
  // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);

  switch(a){ // a is the call number
    
     case 0: r = kgetpid();          break;
     case 1: r = kgetppid();         break;
     case 2: r = kps();              break;
     case 3: r = kchname((char *)b); break;
     case 4: r = ktswitch();         break;
     //new ones
     case 5: r = ksleep(b);          break;
     case 6: r = kwakeup(b);         break;
     case 7: r = kfork((char *)b);   break;
     case 8: r = kexit(b);           break;
     case 9: r = kwait((int*)b);     break;

     case 90: r = kgetc();           break;
     case 91: r = kputc(b);          break;
     case 92: r = kgetPA();          break;
     default: printf("invalid syscall %d\n", a);
  }
  
  return r; //return to goUmode in ts.s, which will replace r0 in Kstack with r
}

