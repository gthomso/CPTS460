#include "type.h"

char *tab = "0123456789ABCDEF";
int color;

#include "string.c"

#include "vid.c"
#include "timer.c"
#include "excepts.c"
#include "kbd.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

TIMER *tp[4];

void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = *(VIC_BASE + VIC_STATUS);
    sicstatus = *(SIC_BASE + SIC_STATUS);  

    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
    }    
    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

int main()
{
   int i; 
   char line[128];

   color = YELLOW;
   row = col = 0; 
   fbuf_init();

   kbd_init();
   
   /* enable timer0,1, uart0,1 SIC interrupts */
   *(VIC_BASE + VIC_INTENABLE) |= (1<<4);  // timer0,1 at bit4 
   *(VIC_BASE + VIC_INTENABLE) |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   *(SIC_BASE + SIC_INTENABLE) |= (1<<3);  // KBD int=3 on SIC
  
   printf("C3.2 start: test timer KBD drivers by interrupts\n");
   timer_init();
   timer_start(0);

   while(1);
}
