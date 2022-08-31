/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              irq.c                                  ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"

/* 
eigene ISR zeiger auf einzelne IRQ-Handler anstelle des regulären fault_handler
*/
extern void irq0();  extern void irq1();  extern void irq2();  extern void irq3();
extern void irq4();  extern void irq5();  extern void irq6();  extern void irq7();
extern void irq8();  extern void irq9();  extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14(); extern void irq15();

/* 
Array von Funktionszeigern für den umgang mit benutzerdefinierten IRQ -Handler für einen bestimmten IRQ 
*/
void* irq_routines[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* binde ein IRQ-handler für einen bestimmteaufgabe des OS ein */
void irq_install_handler(int irq, void (*handler)(struct regs* r)) {irq_routines[irq] = handler;}

/* lösche den IRQ-Eintrag */
void irq_uninstall_handler(int irq) {irq_routines[irq] = 0;}

/* 
Remap: IRQ0 zu IRQ15 auf IRQ32 zu IRQ47
*/
void irq_remap()
{
    // starte die Initialisierungssequenz
    outportb(0x20, 0x11); outportb(0xA0, 0x11);

    // Definiere die PIC Zeiger
    outportb(0x21, 0x20); // setze offset vom Master PIC zu 0x20 (32): einträge 32-39
    outportb(0xA1, 0x28); // setze offset vom Slave  PIC zu 0x28 (40): einträge 40-47

    // beende die Initialisierungssequenz
    outportb(0x21, 0x04); outportb(0xA1, 0x02);
    outportb(0x21, 0x01); outportb(0xA1, 0x01);
    outportb(0x21, 0x00); outportb(0xA1, 0x00);
}

/*
Nach dem remap der Interrupt-Controller müssen die entsprechende ISR an die richtigen Einträge in der IDT verknüpft werden. 
*/
void irq_install()
{
    irq_remap();
    idt_set_gate(32, (unsigned) irq0,  0x08, 0x8E);   idt_set_gate(33, (unsigned) irq1,  0x08, 0x8E);
    idt_set_gate(34, (unsigned) irq2,  0x08, 0x8E);   idt_set_gate(35, (unsigned) irq3,  0x08, 0x8E);
    idt_set_gate(36, (unsigned) irq4,  0x08, 0x8E);   idt_set_gate(37, (unsigned) irq5,  0x08, 0x8E);
    idt_set_gate(38, (unsigned) irq6,  0x08, 0x8E);   idt_set_gate(39, (unsigned) irq7,  0x08, 0x8E);
    idt_set_gate(40, (unsigned) irq8,  0x08, 0x8E);   idt_set_gate(41, (unsigned) irq9,  0x08, 0x8E);
    idt_set_gate(42, (unsigned) irq10, 0x08, 0x8E);   idt_set_gate(43, (unsigned) irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned) irq12, 0x08, 0x8E);   idt_set_gate(45, (unsigned) irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned) irq14, 0x08, 0x8E);   idt_set_gate(47, (unsigned) irq15, 0x08, 0x8E);
}


/*  EOI command to the controllers. If you don't send them, any more IRQs cannot be raised */
void irq_handler(struct regs* r)
{
    /* Handler Zeiger */
    void (*handler)(struct regs* r);

    /* findet heraus ob was auf einen IRQ gelegt wurde und führt es aus. */
    handler = irq_routines[r->int_no - 32];
    if (handler) { handler(r); }

    /* ist der IDT eintrag grösser als 40?(IRQ8-15),
    *  dan sende ein EOI an das slave PIC */
    if (r->int_no >= 40) { outportb(0xA0, 0x20); }

    /* in beiden fällen sendet man den EIO auch an das master PIC */
    outportb(0x20, 0x20);
}
