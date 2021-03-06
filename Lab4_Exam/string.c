#define memcpy kmemcpy
#define strcpy kstrcpy

int kprintf(char *, ...);
int strlen(char *s)
{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int kbcopy(const void *src, void *dest, unsigned int n)
{
   const char *s = (const char *)src;
   char *d = (char *)dest;
   if (s <= d){
      for (; n>0; --n)
         d[n-1] = s[n-1];
   } 
   else{
     for (; n>0; --n)
	*d++ = *s++;
   }
}

char *kmemcpy(void *dest, const void *src, unsigned int n)
{
      kbcopy(src, dest, n);
      return dest;
}

int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int kstrcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

int atoi(char *s)
{
  int v = 0;
  while (*s){
    v = 10*v + (*s - '0');
    s++;
  }
  //kprintf("v=%d\n", v);
  return v;
}
int geti()
{
  char s[16];
  kgets(s);
  return atoi(s);
}
