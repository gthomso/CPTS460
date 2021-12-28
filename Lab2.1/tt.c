/************** tt.c file **************/
// uprintf(char *fmt, ...) is ufprintf(&uart[0], char *fmt, ...)

int N;
int v[] = {1,2,3,4,5,6,7,8,9,10};
int sum;

#include "string.c"
#include "uart.c"

/*int uprintf(char *fmt, ...){

  va_list list;

  uart_init();
  UART *up;
  up = &uart[0];
  ufprintf(up, fmt)
}*/

int uprintf(char *fmt, ...){
  va_list list;
  va_start(list,fmt);
  uart_init();
  UART *up;
  up = &uart[0];
  while(*fmt){
    if (*fmt == '%'){
      fmt++;
      if(*fmt=='x'){
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

int main()
{
  int i;
  char string[64]; 
  UART *up;
  
  N = 10;

  uart_init();

  up = &uart[0];
  uprintf("Enter lines from UART terminal, enter quit to exit\n");
  
  while(1){
    ugets(up, string);
    uprintf("    ");
    if (strcmp(string, "quit")==0)
       break;
    uprintf("%s\n", string);
  }
  uprintf("\n");

  uprintf("Compute sum of array\n");
  sum = 0;
  for (i=0; i<N; i++)
    sum += v[i];
  uprintf("sum = %d\n", sum);
  uprintf("sum = %x\n", sum);
  uprintf("END OF UART DEMO\n");
}
