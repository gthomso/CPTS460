#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

int shift=0, control=0, release=0;

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;
}

void kbd_handler()
{
  color=YELLOW;
  u8 scode, c;
  KBD *kp = &kbd;
  scode = *(kp->base + KDATA);

  // printf("scan code = %x ", scode); // uncomment to see scan codes
  if (scode & 0x80){ //releases a key
    release=1; //signifies that next has just been released
    //printf("kbd interrupt: data=0xF0 c=%x %c\n", c, c);
    return;
  }
  if(release==1){
    release=0;
    if (scode==0x12 || scode==0x59){
      shift=0;
    }
    else if(scode==0x14){
      control=0;
    }
    return; //prevents the second print from being executed
  }
  if(control==1){
    if(scode==0x23){//if D
      c = 0x4; //either c or scode
      kp->buf[kp->head++] = c;
      kp->head %= 128;
      kp->data++; kp->room--;
      return;
    }
    if(scode==0x21){//if c
      printf("kbd interrupt: c=Control-C Control-C\n");
      return;
    }
  }
   //key pressed
  if (scode==0x59 || scode==0x12){ //shift
    shift=1;
    return;
  }
  else if(scode==0x14){
    control=1;
    return;
  }
  else{ //testing if shift and then printing
    if(shift==0){
      c=ltab[scode];
    }
    else{
      c=utab[scode];
    }
  }
  //printf("scode: %x\n",scode);
  printf("kbd interrupt: c=%x %c\n", c, c);
  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
  
  //c = unsh[scode];
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
   while(kp->data == 0);
  lock();

  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  kp->data--; kp->room++;
  unlock();

  return c;
}
