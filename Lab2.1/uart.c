#include <stdarg.h>

char *tab = "0123456789ABCDEF";
#define DR 0x00
#define FR 0x18

typedef struct uart{
  char *base;           // base address
  int  n;
}UART;

UART uart[4];          // 4 UART structs

// For versatile_epb : uarts are at 0x101F1000, 2000, 3000; 10009000 
// For realview : uarts are at 0x10009000, A000, B000, C000

int uart_init()
{
  //altered code for the reslview-pbx-a9
  int i; UART *up;
  for (i=0; i<4; i++){
    up = &uart[i];
    //can do this in port cause each of them is a normal step
    up->base = (char *)(0x10009000 + i*0x1000);
    up->n = i;
  }
  
  //the starting code provided
  /*int i; UART *up;
  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101f1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000);*/
}

int ugetc(UART *up)
{
  while ( *(up->base + FR) & 0x10 );
  return (char)(*(up->base + DR));
}

int uputc(UART *up, char c)
{
  while ( *(up->base + FR) & 0x20 );
  *(up->base + DR) = (int)c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

//for hex
int urpu(UART *up, unsigned int x)
{
  char c;
  if (x){
      c = tab[x%16];
      urpu(up, x/16);
      uputc(up, c);
  }
}
int uprintu(UART *up, unsigned int x)
{
    if (x==0)
        uputc(up, '0');
    else
        urpu(up, x);
    uputc(up, ' ');
}

//for decimal
int urpd(UART *up, unsigned int x)
{
  char c;
  if (x){
      c = tab[x%10];
      urpd(up, x/10);
      uputc(up, c);
  }
}
int uprintd(UART *up, unsigned int x)
{
    if (x==0)
        uputc(up, '0');
    else
        urpd(up, x);
    uputc(up, ' ');
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}


//ufprintf need to change back just doing this to test
int ufprintf(UART *up, char *fmt, ...){
  va_list list;
  va_start(list,fmt);
  while(*fmt){
    if (*fmt == '%'){
      fmt++;
      if(*fmt=='u'){
        uprintu(up, va_arg(list, unsigned int));
      }
      else if(*fmt=='d'){
        uprintd(up, va_arg(list, unsigned int));
      }
      else if(*fmt=='c'){
        uputc(up, va_arg(list, int));
      }
      else if(*fmt=='s'){
        uprints(up, va_arg(list, char *));
      }
      else{
        uprints(up, "Error occured\n");
        break;
      }
    }
    else {
      uputc(up, *fmt);
    }

    fmt++;
  }
}
