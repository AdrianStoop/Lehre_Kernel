
#ifndef funktionen_H
#define funktionen_H

#define TRUE		1
#define FALSE		0

typedef unsigned int   size_t;
typedef unsigned int   UINT;
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef unsigned long  ULONG;
typedef signed int     INT;
typedef signed char    CHAR;
typedef signed short   SHORT;
typedef signed long    LONG;

/*
Diese Struktur definiert(speicher/laden) den stack, der nach einem ISR ausgelösst wurde.
*/
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};


//video.c
extern void k_clear_screen();
extern void settextcolor(UCHAR forecolor, UCHAR backcolor);
extern void putch(UCHAR c);
extern void puts(UCHAR* text);
extern void scroll();
extern void k_printf(UCHAR* message, UINT line, UCHAR attribute);
extern void set_cursor(UCHAR x, UCHAR y);
extern void update_cursor();
extern void move_cursor_right();
extern void move_cursor_left();
extern void move_cursor_home();
extern void move_cursor_end();
extern void save_cursor();
extern void restore_cursor();
extern void printformat (char *args, ...);

//timer.c
extern void timer_handler(struct regs* r);
extern void timer_wait (ULONG ticks);
extern void sleepSeconds (ULONG seconds);
extern void sleepMilliSeconds (ULONG seconds);
extern void timer_install();
extern void timer_uninstall();

//keyboard.c
extern void keyboard_install();
extern void keyboard_init();
extern UCHAR k_getch();
extern UCHAR FetchAndAnalyzeScancode();
extern void keyboard_handler(struct regs* r);

//util.c
extern void outportb(UINT port, UINT val);
inline UINT inportb(UINT port);
extern void* k_memset(void* dest, char val, size_t count);
extern USHORT* k_memsetw(USHORT* dest, USHORT val, size_t count);
extern void* k_memcpy(void* dest, const void* src, size_t count);
extern size_t k_strlen(const char* str);
extern void reboot();
extern void cli();
extern void sti();
extern void k_itoa(int value, char* valuestring);
extern void k_i2hex(UINT val, UCHAR* dest, int len);
extern void float2string(float value, int decimal, char* valuestring);

//gdt.c
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

//idt.c
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

//irq.c and isrs.c
extern void isrs_install();
extern void irq_install();
extern void fault_handler(struct regs* r);
extern void irq_install_handler(int irq, void (*handler)(struct regs* r));
extern void irq_uninstall_handler(int irq);
extern void irq_remap(void);
extern void irq_handler(struct regs* r);

#endif
