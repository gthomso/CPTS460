int prints(char *s) {
  while(*s){
    putc(*s);
    s++;
  }
}

int gets(char *s) {
  *s=getc();
  //the '0' wprks
  while(*s!='\r'){
    s++;
    *s = getc();
  }
  *(s)=0;
}

char ans[64];

int main() {
  
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
    prints("\n");
  }
  
}
