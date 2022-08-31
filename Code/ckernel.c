/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              ckernel.c                              ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"
int main()
{
    /*
    Da man aus dem asm-Kernel kommt und der Bildschirm noch benutzt war, 
    muss man zuerstden Bildschirm löschen
    */
    k_clear_screen();


    /*
    Initialisiere GDT, IDT, ISRS, IRQ, timer und keyboard.
    Genauere Bschreibung in den einzelnen Funktionen
    */
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install();
    keyboard_install();

    /*
    Das OS ist jetzt Initialisiert und kann ab jetzt auf Interrupt reagieren:
    sti = SeT Interrupts,  (Interrupt erlauben/zulassen)
    cli = CLear Interrupts (Interrupt nicht erlauben/zulassen)
    */
    sti();

    /*
    Ende Vorbereitung
    */


    /*
    setzte startposizion des Cursor (stadartwerte sind 0,0 und alte position auch 0,0)
    */
    set_cursor(0,0);

    /*
    Lade Titelbild auf den Bildschirm
    */
    
    printformat("    #**********************************************************************#   \n");
    printformat("    #  __             __   __              __         ______   __      __  #   \n");
    printformat("    # |  |           |  | |__|            |  |      .'  ____' |  |    |  | #   \n");
    printformat("    # |  |           |  |  __   __        |  |      |  |____   |  |  |  |  #   \n");
    printformat("    # |  |           |  | |  | |  |_____  |  |___   |   ___.'   |  ''  |   #   \n");
    printformat("    #  |  |    .    |  |  |  | |   __   ) |   __ '. |  |        |  --  |   #   \n");
    printformat("    #   |  |_.-^-._|  |   |  | |  |  |  | |  |__| | |  |____   |  |  |  |  #   \n");
    printformat("    #    |____---____|    |__| |__|  |__| |______.' '._____.' |__|    |__| #   \n");
    printformat("    #**********************************************************************#   \n");
    printformat("    # V.0.01.00 # Kernel in Asm und C #  stand: 18.01.2016  # adrian stoop #   \n");
    printformat("    #**********************************************************************#   \n\n");
    printformat("    #**********************************************************************#   \n");
    

    /*
    setzt Cursor neu, weil sonst eingegebene Tasten das Titelbild überschreiben.
    courser muss ebenfalls gespeichert werden, da die Interrupts Schläge 
    sich auf den letzten gespeicherten Cursor ort wieder ausgeben.
    */

int farbe = 0x00;
    
    set_cursor(0,14);
    save_cursor();

    while(1)
        {
            
            while(1)
                {
                    k_printf("Hoi", 17, farbe);
                    sleepMilliSeconds(1000);
                    if(farbe == 0xFF)
                        {
                            farbe = 0x00;
                        }
                    else
                        {
                            ++farbe;
                        }

                }

            

        }
    /*
    int systime_sek = 0;
    int systime_min = 0;
    int systime_hour = 0;
    while(TRUE)
    {
        sleepMilliSeconds(1000);
        systime_sek++;
        if(systime_sek == 60){systime_sek = 0; systime_min++;}
        if(systime_min == 60){systime_min = 0; systime_hour++;}
        set_cursor(40,11);
        settextcolor(0x2, 0x0);
        printformat("%s%d%s%d%s%d%s","System Runtime:", systime_hour, "h", systime_min, "min", systime_sek, "sek");
        settextcolor(0xF,0x0);
    };
    */
    
    
            

    return 0;
};



        