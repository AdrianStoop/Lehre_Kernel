/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              video.c                                ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include "funktionen.h"
#include <stdarg.h>
/*
varablen für den cursor und video speicherort
*/
UCHAR  csr_x  = 0;
UCHAR  csr_y  = 0;
UCHAR  saved_csr_x  = 0;
UCHAR  saved_csr_y  = 0;
UCHAR  attrib = 0x0F;
USHORT* vidmem = (USHORT*) 0xb8000;
/*
löscht den Bildschirm und setzt cursor nach 0,0
*/
void k_clear_screen()
{
    k_memsetw (vidmem, 0x20 | (attrib << 8), 80 * 25);
    csr_x = 0; csr_y = 0; update_cursor();
}
/*
setze Textfarbe high nibble für hindergrund und low nibble für vordergrund
*/
void settextcolor(UCHAR forecolor, UCHAR backcolor)
{
    attrib = (backcolor << 4) | (forecolor & 0x0F);
}
/*
bewegt den Cursor nach ganz rechts, ist er schon rechts springt er auf die nächste zeile
*/
void move_cursor_right()
{
    ++csr_x;
    if(csr_x>=80)
    {
      ++csr_y;
      csr_x=0;
    }
}
/*
bewegt den Cursor nach ganz links, ist er schon links springt er auf die vorherige zeile
*/
void move_cursor_left()
{
    if(csr_x)
        --csr_x;
    if(!csr_x && csr_y>0)
    {
        csr_x=79;
        --csr_y;
    }
}
/*
bewegt curor nach ganz links
*/
void move_cursor_home()
{
    csr_x = 0; update_cursor();
}
/*
bewegt cursor nach ganz rechts
*/
void move_cursor_end()
{
    csr_x = 79; update_cursor();
}
/*
setzt cursor an pos x,y
*/
void set_cursor(UCHAR x, UCHAR y)
{
    csr_x = x; csr_y = y; update_cursor();
}
/*
cursor Position wird an die tastatur weitergeschickt
*/
void update_cursor()
{
    USHORT position = csr_y * 80 + csr_x;
    // cursor HIGH port to vga INDEX register
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (UCHAR)((position>>8)&0xFF));
    // cursor LOW port to vga INDEX register
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, (UCHAR)(position&0xFF));
};
/*
gibt einen Buchstabe an stelle aus(keine strings!)
werden sondertasten benutzt (z.b. tab) wird das hier eingefüght
*/
void putch(UCHAR c)
{
    USHORT* pos;
    UINT att = attrib << 8;

    if(c == 0x08) // ist charakter ein backspace, geht der Cursor ein zurück und überschreibt die stelle mit ' '.
    {
        if(csr_x)
        {
            --csr_x;
            putch(' ');
            --csr_x;
        }
        if(!csr_x && csr_y>0)
        {
            csr_x=79;
            --csr_y;
            putch(' ');
            csr_x=79;
            --csr_y;
        }
    }
    else if(c == 0x09) // ist ein tab: increment csr_x mit 8(springt 10 stellen vor)
    {
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    else if(c == '\r') // cr: cursor geht zurück auf 0 in der zeile
    {
        csr_x = 0;
    }
    else if(c == '\n') // newline: wie cr + eine zeile nach runter
    {
        csr_x = 0; ++csr_y;
    }
    /* ist es ein Buchstabe, wird er hier ins video register geladen mit den farbatributten*/
    else if(c >= ' ')
    {
        pos = vidmem + (csr_y * 80 + csr_x);
        *pos = c | att; // Character und attributes: farbe
        ++csr_x;
    }

    if(csr_x >= 80) // nach jeder ausgabe wird geprüft ob der Cursor rechts angekommen ist und beginnt eine neue linie
    {
        csr_x = 0;
        ++csr_y;
    }

    /* scrolle runter und setzte den cursor neu */
    scroll();
    update_cursor();
}
/*
schlaufrutine für stringsausgabe mit putch
*/
void puts(UCHAR* text)
{
    for(; *text; putch(*text), ++text);
}
/*
wenn der Cursor unten angekommen ist wird der ganze videospeicher verschoben und "weiter oben" neu gesetzt
*/
void scroll()
{
    UINT blank, temp;
    blank = 0x20 | (attrib << 8);
    if(csr_y >= 25)
    {
        temp = csr_y - 25 + 1;
        k_memcpy (vidmem, vidmem + temp * 80, (25 - temp) * 80 * 2);
        k_memsetw (vidmem + (25 - temp) * 80, blank, 80);
        csr_y = 25 - 1;
    }
}
/*
gibt text an stelle x,y aus mit farb atributen und linie
*/
void k_printf(UCHAR* message, UINT line, UCHAR attribute)
{
    // Top 4 bytes: background, bottom 4 bytes: foreground color
    settextcolor(attribute & 0x0F, attribute >> 4);
    csr_x = 0; csr_y = line;
    update_cursor();
    puts(message);
};

/* k_printf funktion die formate an nimmt. unterstützt: %u, %d, %x/%X, %s, %c */
void printformat (char *args, ...)
{
    va_list ap;
    va_start (ap, args);
    int index = 0, d;
    UINT u;
    char c, *s;
    char buffer[100];

    while (args[index])
    {
        switch (args[index])
        {
        case '%':
            ++index;
            switch (args[index])
            {
            case 'u':
                u = va_arg (ap, UINT);
                k_itoa(u, buffer);
                puts(buffer);
                break;
            case 'd':
            case 'i':
                d = va_arg (ap, int);
                k_itoa(d, buffer);
                puts(buffer);
                break;
            case 'X':
            case 'x':
                d = va_arg (ap, int);
                k_i2hex(d, buffer,8);
                puts(buffer);
                break;
            case 's':
                s = va_arg (ap, char*);
                puts(s);
                break;
            case 'c':
                c = (char) va_arg (ap, int);
                putch(c);
                break;
            default:
                putch('%');
                putch('%');
                break;
            }
            break;

        default:
            putch(args[index]); //wenn kein \xyz dan wird der charakter einfach ausgegeben.
            break;
        }
        ++index;
    }
}
/*
speichert cursor positionen
*/
void save_cursor()
{
    cli();
    saved_csr_x  = csr_x;
    saved_csr_y  = csr_y;
    sti();
}
/*
ladet cursor Position
*/
void restore_cursor()
{
    cli();
    csr_x  = saved_csr_x;
    csr_y  = saved_csr_y;
    sti();
}
