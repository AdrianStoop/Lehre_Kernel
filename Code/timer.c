/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              timer.c                                ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"

ULONG const FREQ  = 100; // 100 "ticks" pro sekunde
ULONG timer_ticks =   0;
ULONG eticks;

/*test*/
int teiler = 0;
int systime_sek = 0;
int systime_min = 0;
int systime_hour = 0;
/*\teset*/

void timer_handler(struct regs* r)
{
    ++timer_ticks;
    if (eticks)
        --eticks;

    /*TEST*/
        ++teiler;
        if(teiler == 100)
          {
            teiler = 0;
            ++systime_sek;
            if(systime_sek == 60){systime_sek = 0; systime_min++;}
            if(systime_min == 60){systime_min = 0; systime_hour++;}
            set_cursor(40,11);
            settextcolor(0x2, 0x0);
            printformat("%s%d%s%d%s%d%s","System Runtime:", systime_hour, "h", systime_min, "min", systime_sek, "sek");
            settextcolor(0xF,0x0);
          }
    /*\TEST*/
}

void timer_wait (ULONG ticks)
{
    timer_uninstall();
    eticks = ticks;
    timer_install();

    // warte....
    while (eticks)
    {
        update_cursor();
    }
}

void sleepSeconds (ULONG seconds)
{
    timer_wait(FREQ * seconds);
}

void sleepMilliSeconds (ULONG ms)
{
    timer_wait(FREQ * ms/1000UL);
}


static void systemTimer_setFrequency( ULONG freq )
{
   ULONG divisor = 1193180 / freq; //divisor mus 2Byte lang sein

   // Sende das Befehls Byte
   outportb(0x43, 0x36);

   // Sende den Divisor
   outportb(0x40, (UCHAR)(  divisor     & 0xFF )); // low  byte
   outportb(0x40, (UCHAR)( (divisor>>8) & 0xFF )); // high byte
}

void timer_install()
{
    /*setzt timer_handler auf IRQ0*/
    irq_install_handler(0, timer_handler);
    systemTimer_setFrequency( FREQ ); // FREQ in Hz
}

void timer_uninstall()
{
    /* lösche timer_handler auf IRQ0 */
    irq_uninstall_handler(0);
}
