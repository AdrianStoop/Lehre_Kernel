/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              gdt.c                                  ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"

#define NUMBER_GDT_GATES 5

/* 
Diese Strukturen definieren ein GDT eintrag.
*/
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
}__attribute__((packed));

/*
GDT-Pointer
*/
struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed));


/* 
hier wird die GDT erstellt mit 3 Einträgen(0 eintrag ist immer 0)
*/
struct gdt_entry gdt[NUMBER_GDT_GATES];

/*
GDT Pointer erstellen
*/
struct gdt_ptr   gdt_register;

/*
lade den gewählten GDT Eintrag
*/
static void gdt_load(){ asm volatile("lgdt %0" : "=m" (gdt_register)); }


/* Setup ein Deskriptor in der Global Descriptor Tabelle */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
    {
        /* Setup die Deskriptor Basis addresse */
        gdt[num].base_low = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high = (base >> 24) & 0xFF;

        /* Setup das Deskriptor Limit */
        gdt[num].limit_low = (limit & 0xFFFF);
        gdt[num].granularity = ((limit >> 16) & 0x0F);

        /* Setup Zugriffsrechte*/
        gdt[num].granularity |= (gran & 0xF0);
        gdt[num].access = access;
    }

/* 
Setup GDT mit 3 Einträgen und lade die GDT mit einem Pointer.
*/
void gdt_install()
    {
        /* Setup den GDT Pointer und limit */
        gdt_register.limit = (sizeof(struct gdt_entry) * NUMBER_GDT_GATES)-1;
        gdt_register.base  = (unsigned int) &gdt;

        /* GDT GATES -  Deskriptor mit Zeiger auf die Lineare Speicher Adresse*/
        gdt_set_gate(0, 0, 0, 0, 0);                // NULL Deskriptor
        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CODE, privilege level 0 für kernel code
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DATA, privilege level 0 für kernel code
        gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // CODE, privilege level 3 für user code
        gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // DATA, privilege level 3 für user code

        gdt_load(); // das GDTR(GDT Register der CPU schaut nun auf die GDT-Tabelle/Speicherort)
    }
