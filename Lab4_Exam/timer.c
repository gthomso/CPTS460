// timer.c file
// timer register offsets from base address
/**** byte offsets of registers ***
#define TLOAD   0x00
#define TVALUE  0x04
#define TCNTL   0x08
#define TINTCLR 0x0C
#define TRIS    0x10
#define TMIS    0x14
#define TBGLOAD 0x18
**********************************/
/** u32 * offsets of registers ****/
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer{
  u32 *base;             // timer's base address; as u32 pointer
  int tick, hh, mm, ss;  // per timer data area
  char clock[16]; 
}TIMER;

volatile TIMER timer[4];  // 4 timers; 2 timers per unit; at 0x00 and 0x20

int kprintf(char *fmt, ...);
int kpchar(char, int, int);
int unkpchar(char, int, int);
int clrcursor();

int seconds;

void timer_init()
{
  int i;
  TIMER *tp;
  kprintf("timer_init()\n");
  
  for (i=0; i<4; i++){
    tp = &timer[i];
    if (i==0) tp->base = (u32 *)0x101E2000; 
    if (i==1) tp->base = (u32 *)0x101E2020; 
    if (i==2) tp->base = (u32 *)0x101E3000; 
    if (i==3) tp->base = (u32 *)0x101E3020;

    *(tp->base+TLOAD) = 0x0;     // reset
    *(tp->base+TVALUE)= 0xF0000; // start up count
    //*(tp->base+TVALUE)= 0xFFFFFFFF;
    *(tp->base+TRIS)  = 0x0;
    *(tp->base+TMIS)  = 0x0;
    *(tp->base+TLOAD) = 0x100;
    *(tp->base+TCNTL) = 0x62; //011-0000=|NOTEn|Pe|IntE|-|scal=00|32-bit|0=wrap|
    //*(tp->base+TCNTL) = 0x66;
    *(tp->base+TBGLOAD) = 0x80000/60;  // reload count value
    //*(tp->base+TBGLOAD) = 0x1C00;

    tp->tick = tp->hh = tp->mm = tp->ss = 0;
    strcpy((char *)tp->clock, "00:00:00"); //strcpy
    seconds = 0;
  }
}

void timer_handler(int n)
{
  //copied a lot out of book
  // NOTE: the timer count value generate 60 interrupts per second
  int i=0;
  TIMER *tp = &timer[n];
  tp->tick++;
  if(tp->tick==60){
    clrcursor();
  }
  if (tp->tick == 120){
    color=YELLOW;
    putcursor();
    //some stuff for interrupt
    PROC *p = timerQueue;
    p->exitCode--;
    if(p->exitCode==0){
      p->priority=0;
      //printf("lolfail\n");
      dequeue(&timerQueue);
      enqueue(&freeList,p);
      p->status=0;
    }

    if (p){
      kpchar(('0'+((char)p->pid)),(n+1), 70);
      int tempnum=p->exitCode;
      int temp=0;
      clrpix(n+1,71);
      char s[3];
      temp=tempnum/100;
      tempnum=tempnum%100;
      kpchar('0'+(char)temp,n+1,72);
      temp=tempnum/10;
      tempnum=tempnum%10;
      s[1]='0'+(char)temp;
      temp=tempnum;
      s[2]='0'+(char)temp;
      //printf("%c",s[2]);

      
      for (int index=1;index<3;index++){
        unkpchar('0',n+1,72+index);
        unkpchar('1',n+1,72+index);
        unkpchar('2',n+1,72+index);
        unkpchar('3',n+1,72+index);
        unkpchar('4',n+1,72+index);
        unkpchar('5',n+1,72+index);
        unkpchar('6',n+1,72+index);
        unkpchar('7',n+1,72+index);
        unkpchar('8',n+1,72+index);
        unkpchar('9',n+1,72+index);
        color = running->pid;
        kpchar(s[index],n+1,72+index);
      }
    }
    
    tp->tick = 0; tp->ss++;
    if(tp->ss==60){
      tp->ss = 0; tp->mm++;
      if(tp->mm==60){
        tp->mm = 0; tp->hh++;
      }
    }
    tp->clock[7]='0'+(tp->ss%10); tp->clock[6]='0'+(tp->ss/10);
    tp->clock[4]='0'+(tp->mm%10); tp->clock[3]='0'+(tp->mm/10);
    tp->clock[1]='0'+(tp->hh%10); tp->clock[0]='0'+(tp->hh/10);
    //printf("seconds = %d\n", seconds);
  }
  //color = n;
  //timer_clearInterrupt(n);
  for(i=0;i<8;i++){
    //unkpchar(tp->clock[i],n,70+i);
    unkpchar('0',n,70+i);
    unkpchar('1',n,70+i);
    unkpchar('2',n,70+i);
    unkpchar('3',n,70+i);
    unkpchar('4',n,70+i);
    unkpchar('5',n,70+i);
    unkpchar('6',n,70+i);
    unkpchar('7',n,70+i);
    unkpchar('8',n,70+i);
    unkpchar('9',n,70+i);
    color = running->pid;
    kpchar(tp->clock[i], n, 70+i);
  }

  //timer for timerqueue


  timer_clearInterrupt(n);
} 

void timer_start(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  kprintf("timer_start %d base=%x\n", n, tp->base);
  *(tp->base+TCNTL) |= 0x80;  // set enable bit 7
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TCNTL) &= 0x7F;  // clear enable bit 7
}
