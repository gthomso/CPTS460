// pipe.c file

#define NPIPE 10
#define PSIZE  8
#define FREE   0
#define BUSY   1

typedef struct pipe{
  char buf[PSIZE];
  int head, tail;
  int data, room;

  //my edits
  int writer_present;
  int reader_present;
  //int iteration;

  int status;  // FREE or BUSY
}PIPE;

PIPE pipe[NPIPE];
extern PROC *running;

PIPE *kpipe;

int kexit()
{
  printf("proc%d kexit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}

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
       p->status = BUSY; p->writer_present=1; p->reader_present=2;
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
  if(p->writer_present<=0){
    //what to do when writer has exited
    printf("No writer error, returning 0\n");
    return 0;
  }

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

    printf("Proc%d sleep for data\n", running->pid);
    ksleep((int)&p->data);
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
    uprintf("Proc%d sleep for room; wrote %d bytes\n", running->pid, w);
    ksleep((int)&p->room);
  }
}



int pipe_writer()
{
  char cmd[64];
  int n;
  UART *up = &uart[0];
  char line[128];
  
  while(1){
    uprintf("proc%d PipeWriter: Enter command (write|exit)\n", running->pid);
    ugets(up, cmd);
    uprintf("\n");

    if (strcmp(cmd, "write")==0){
      if(kpipe->reader_present<=0){
        //what to do with broken pipe.
        printf("No Reader Error: Broken Pipe\n");
        return 0;
      }
      uprintf("enter a line to write : ");
      ugets(up, line);
      uprintf("\n");
      n = write_pipe(kpipe, line, strlen(line));
      uprintf("proc%d write line=[%s] to pipe\n", running->pid, line);
    }

    if(strcmp(cmd, "exit")==0){
      kpipe->writer_present=0;
      uprintf("proc%d WRITER exit\n", running->pid);
      kwakeup(&kpipe->data);
      kexit();
    }
  }
}

int pipe_reader()
{
  char line[128];
  char cmd[64];
  int i, n;
  
  while(1){
    printf("proc%d PipeReader: Enter command (read|exit)\n", running->pid);
    kgets(cmd);

    if (strcmp(cmd, "read")==0){
      n = read_pipe(kpipe, line, 20);
      line[n] = 0;
      printf("proc%d read %d chars: line=[%s]\n", running->pid, n, line);
    }
    if(strcmp(cmd, "exit")==0){
      kpipe->reader_present=0;
      printf("proc%d READER exit\n", running->pid);
      kwakeup(&kpipe->room);
      kexit();
    }
  }
}

  
