int color;

#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "timer.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void IRQ_handler()
{
    int vicstatus, sicstatus;
    vicstatus = *(VIC_BASE + VIC_STATUS); // VIC_STATUS=0x10140000=status reg
    sicstatus = *(SIC_BASE + SIC_STATUS);  
    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
    }
    if (vicstatus & (1 << 31)){
      if (sicstatus & (1 << 3)){
          kbd_handler();
       }
    }
}

void timer_handler();

TIMER *tp[4];

int body();
int main()
{ 
   int i; 
   char line[128]; 

   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kbd_init();
   
   *(VIC_BASE + VIC_INTENABLE) |= (1<<4);  // timer0,1 at bit4 
   // allow KBD interrupts   
   *(VIC_BASE + VIC_INTENABLE) |= (1<<31); // allow VIC IRQ31

   // enable KBD IRQ 
   *(SIC_BASE + SIC_INTENABLE) |= (1<<3);  // KBD int=3 on SIC
   timer_init();
   timer_start(0);
 
   kprintf("Welcome to WANIX in Arm\n");
   init();
   kfork((int)body, 1);
   printf("P0 switch to P1\n");
   while(1){
     if (readyQueue)
        tswitch();
   }
}
