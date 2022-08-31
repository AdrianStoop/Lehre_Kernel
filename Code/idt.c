/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              idt.c                                  ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"

/*
Struktur für IDT einträge
*/
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
}__attribute__((packed)); 

/*
Struktur für IDT Pointer
*/
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed)); 

/*
Deklariert eine IDT mit max. 256 plätzen und ein Zeiger zu dieser IDT
*/
struct idt_entry idt[256];
struct idt_ptr   idt_register;

/*
lade IDT Register (IDTR)
*/
static void idt_load(){ asm volatile("lidt %0" : "=m" (idt_register)); } 

/*
schreibt einen Eintrag in die IDT
*/
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
    {
        idt[num].base_lo = (base        & 0xFFFF);
        idt[num].base_hi = (base >> 16) & 0xFFFF;
        idt[num].sel     =   sel;
        idt[num].always0 =     0;
        idt[num].flags   = flags;
    }

/*
Setup einer IDT mit Zeiger
*/
void idt_install()
    {
        // Setzt den IDT zeiger nach oben
        idt_register.limit = (sizeof (struct idt_entry) * 256)-1;
        idt_register.base  = (unsigned int) &idt;

        k_memset(&idt, 0, sizeof(struct idt_entry) * 256); // lösche den IDT eintrag
        idt_load(); // das IDTR zeigt nun auf die IDT
    }
