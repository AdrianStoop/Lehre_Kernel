/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              keyboard.c                             ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "keyboard.h"
#include "funktionen.h"
/*
Variablen für:
ShiftKeyDown = 0 = shift nicht gedrückt, 1 = shift gedrückt
KeyPressed   = 0 = taste nicht gedrückt, 1 = Taste gedrückt
Scan         = 0 = keien Scancode, 0< = scancode von der Tastatur
*/
UCHAR ShiftKeyDown = 0; 
UCHAR KeyPressed   = 0; 
UCHAR scan         = 0; 


/* warte bis der Buffer lehr ist */
void keyboard_init()
{
    while( inportb(0x64)&1 )
        inportb(0x60);
};

/*
scannt die tastatur und setzt die oben genannten variablen.
*/
UCHAR FetchAndAnalyzeScancode()
{
	if( inportb(0x64)&1 )
	    scan = inportb(0x60);   // 0x60: nim den scancode der Tastatur

    // ACK: toggle bit 7 an port 0x61
    UCHAR port_value = inportb(0x61);
    outportb(0x61,port_value |  0x80); // 0->1
    outportb(0x61,port_value &~ 0x80); // 1->0

	if( scan & 0x80 ) // Taste losgelassen? Checke bit 7 (10000000b = 0x80) vom scancode für das
	{
        KeyPressed = 0;
        scan &= 0x7F; // taste wurde losgelassen, verANDe nur die 7 tieferen bits: 01111111b = 0x7F
        if( scan == KRLEFT_SHIFT || scan == KRRIGHT_SHIFT ) // taste wurde losgelassen, shift auch?
        {
            ShiftKeyDown = 0;	// ja, benutzte nonshift
        }
	}
	else // taste wurde gedrückt
	{
	    KeyPressed = 1;
		if( scan == KRLEFT_SHIFT || scan == KRRIGHT_SHIFT ) // wurde shift gedrükt?
		{
		    ShiftKeyDown = 1; // ja, benutte shiftscode
		}
	}
	return scan;
}

/*
nimm den scancode und gib den ascii code als rückgabewert
zurück. Tastenbelegung ist in keyboard.h
*/
UCHAR k_getch() // Scancode --> ASCII
{
	UCHAR retchar;	                   // speichert das Zeichen aus der Ascii tabelle
	scan = FetchAndAnalyzeScancode();  // scannt den scancode und schauft auf die shifttaste

	if( ShiftKeyDown )                  // ist shift gdrückt?
	    retchar = asciiShift[scan];	   // Shift Codes
	else
		retchar = asciiNonShift[scan]; //  Non-Shift Codes

	if( ( !(scan == KRLEFT_SHIFT || scan == KRRIGHT_SHIFT) ) && ( KeyPressed == 1 ) ) //filtert Shift Key und Key Release
	    return retchar; // gibt ASCII version zurück
	else
	    return 0;
}

/*
wenn ein Interrupt ausgelösst wird vom keyboard, wird diese funktion aufgerüfen, egal was das OS macht!
*/
void keyboard_handler(struct regs* r)
{
   UCHAR KEY;
   KEY = k_getch(); //speichert IRQ tastenschlag
   restore_cursor();  // springt mit dem cursor an die letzte gespeicherte cursor stelle(default isst 0,0)

   int reboot_sek = 5;//test@@@@@@@@@@@

   switch(KEY)
   {
       case KINS: //teset@@@@@@@@@@@@
            while(reboot_sek)
              {
                set_cursor(40,13);
                settextcolor(0x4, 0x0);
                printformat("%s %d %s","System halt, Reboot in:", reboot_sek, "sek");
                settextcolor(0xF,0x0);
                reboot_sek--;
                sleepMilliSeconds(1000);
              }
              reboot();//teset@@@@@@@@@@@@
           break;
       case KDEL:
           move_cursor_right();
           putch('\b');//BACKSPACE
           break;
       case KHOME:
           move_cursor_home();
           break;
       case KEND:
           move_cursor_end();
           break;
       case KPGUP:
           break;
       case KPGDN:
           break;
       case KLEFT:
           move_cursor_left();
           break;
       case KUP:
           break;
       case KDOWN:
           break;
       case KRIGHT:
           move_cursor_right();
           break;
       default:
           printformat("%c",KEY); // print the ASCII character aus
           break;
   }
   save_cursor();//speichert cursor position
}

/*
"installiert" den keyboard handler auf IRQ 1
*/
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
    keyboard_init();
}




