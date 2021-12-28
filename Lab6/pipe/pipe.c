// pipe.c file

#define NPIPE 10
#define PSIZE  8
#define FREE   0
#define BUSY   1

typedef struct pipe{
  char buf[PSIZE];
  int head, tail;
  int data, room;

  int writer_present;
  int reader_present;
  int iteration;
  
  int status;  // FREE or BUSY
}PIPE;

PIPE pipe[NPIPE];
extern PROC *running;

int pipe_init()
{
  int i;
  printf("pipe_init()\n");
  for (i=0; i<NPIPE; i++){
    pipe[i].status = FREE;
  }
}

PIPE *create_pipe()
{
  int i; PIPE *p;
  printf("creating pipe: ");
  for (i=0; i<NPIPE; i++){
     p = &pipe[i];
     if (p->status==FREE){
       p->reader_present=0; p->writer_present=0; p->iteration=0;
       p->status = BUSY;
        break;
     }
  }
  p->head = p->tail = 0;
  p->data = 0;  p->room = PSIZE;
  printf("OK\n");
  return p;
}

int read_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;

  while(n){
    while(p->data){
      *buf = p->buf[p->tail++];
      p->tail %= PSIZE;
      p->data--; p->room++; buf++; r++; n--;
      if (n==0)
	      break;
    }

    kwakeup((int)&p->room);

    if (r)
      return r;
    //will only get to here if there was nothing to return
    
    printf("Proc%d sleep for data\n", running->pid);
    ksleep((int)&p->data);
    return 0;
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0; int w = 0;
  while(n){
    w = 0;
    while (p->room){
      p->buf[p->head++] = *buf;
      p->head %= PSIZE;
      p->data++; p->room--; buf++; w++; r++; n--;
      if (n==0)
	      break;
    }
   
    kwakeup((int)&p->data);
    if (n==0){
       return r;
    }
    printf("Proc%d sleep for room; wrote %d bytes\n", running->pid, w);
    ksleep((int)&p->room);
  }
}


  
