typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
#define printf kprintf

#define BLUE   0
#define GREEN  1
#define RED    2
#define WHITE  3
#define CYAN   4
#define YELLOW 5
#define BLACK  6

#define N_SCAN 64

u32 *VIC_BASE = (u32 *)0x10140000;
#define VIC_STATUS    0x00/4
#define VIC_INTENABLE 0x10/4
#define VIC_VADDR     0x30/4

u32 *SIC_BASE = (u32 *)0x10003000;
#define SIC_STATUS    0x00/4
#define SIC_INTENABLE 0x08/4
#define SIC_ENSET     0x08/4
#define SIC_PICENSET  0x20/4

char *UART0_BASE = (char *)0x101f1000;
char *UART1_BASE = (char *)0x101f2000;
#define UDR   0x00
#define UFR   0x18
#define UIMSC 0x38

char *KBD_BASE = 0x10006000;
#define KBD_CR 0x00
#define KBD_DR 0x08
