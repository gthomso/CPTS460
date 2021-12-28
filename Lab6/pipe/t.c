#include "type.h"

char *tab = "0123456789ABCDEF";

int kprintf(char *fmt, ...);
int kputc(char);

#include "string.c"
#include "queue.c"
#include "vid.c"

#include "kbd.c"
#include "uart.c"
#include "pipe.c"
#include "exceptions.c"
#include "kernel.c"


PIPE *kpipe;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// IRQ interrupts handler entry point
void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIC status reg to find out which interrupt
    vicstatus = *(VIC_BASE + VIC_STATUS);
    sicstatus = *(SIC_BASE + SIC_STATUS);  

    if (vicstatus & (1<<12)){
      uart_handler(0);
    }
    
    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

int pipe_writer()
{
  //  char c, *cp; 
  struct uart *up = &uart[0];
  char line[128];
  kpipe->writer_present=running->pid;

  
  /*uprintf("Enter a line for task1 to get : ");
  ugets(up, line);
  uprints(up, "\r\n");
  printf("proc%d write line=[%s] to pipe\n", running->pid, line);
  write_pipe(kpipe, line, strlen(line));*/

  while(1){
    uprintf("Enter a line for task1 to get : ");
    ugets(up, line);
    if (kpipe->reader_present<=0 && kpipe->iteration>0){
      printf("BROKEN PIPE: no reader present.");
      kpipe->writer_present=0;
      kgetc();
      return -1;
    }
    uprints(up, "\r\n");
    printf("proc%d write line=[%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));
    kpipe->iteration++;
  }
  kpipe->writer_present=0;
  ksleep(0);
}

int pipe_reader()
{
  char line[128];
  int i, n;
  kpipe->reader_present=running->pid;

  /*printf("proc%d read from pipe\n", running->pid);
  n = read_pipe(kpipe, line, 20);
  printf("proc%d read n=%d bytes from pipe : [", running->pid, n);
  for (i=0; i<n; i++)
      kputc(line[i]);
  kprintf("]\n");
  kpipe->iteration++;*/

  while(1){
    printf("proc%d read from pipe\n", running->pid);
    n = read_pipe(kpipe, line, 20);
    if(n==0 && kpipe->writer_present<=0){
      printf("No Writer and no data for read_pipe\n");
      kpipe->reader_present=0;
      kgetc();
      return 0;
    }
    printf("proc%d read n=%d bytes from pipe : [", running->pid, n);
    for (i=0; i<n; i++)
        kputc(line[i]);
    kprintf("]\n");
  }
  kpipe->reader_present=0;
  ksleep(0);
}
  
int main()
{ 
   color = WHITE;
   //row = col = 0; 

   fbuf_init();
   kprintf("Welcome to Wanix in ARM\n");

   kbd_init();   

   // enable IRQ31 on VIC  
   *(VIC_BASE + VIC_INTENABLE) |= (1<<12); // UART0
   *(VIC_BASE + VIC_INTENABLE) |= (1<<31); // SIC to VIC's IRQ31

   // enable KBD IRQ on SIC 
   *(SIC_BASE + SIC_INTENABLE) = (1<<3);   // KBD int=bit3 on SIC

   uart_init();

   pipe_init();
   kpipe = create_pipe();

   init();

   kprintf("P0 kfork tasks: ");
   kfork(pipe_writer, 1);
   kfork(pipe_reader, 1);

   printQ(readyQueue);

   unlock();
   while(1){
     if (readyQueue)
        tswitch();
   }
}
