/*-------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              util.c                                 ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;-------------------------------------------------------------*/
#include <limits.h>
#include "funktionen.h"

inline void sti() { asm volatile ( "sti" ); } // erlaube/aktiviere interrupts
inline void cli() { asm volatile ( "cli" ); } // verbiete/deaktiviere interrupts

/*
holle einen Wert von Port
*/
inline UINT inportb(UINT port)
{
  UINT ret_val;
  asm volatile ("inb %w1,%b0" : "=a"(ret_val) : "d"(port));
  return ret_val;
}
/*
gibt einen Wert an einen Port weiter
*/
inline void outportb(UINT port, UINT val)
{
    asm volatile ("outb %b0,%w1" : : "a"(val), "d"(port));
}
/*
Kopiert den Werte von einem Byte von der speicherStelle , auf den der zeiger zeigt.
*/
void* k_memcpy(void* dest, const void* src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}
/*
Setzt die ersten num Byte der Speicherblock durch ptr, wies auf den angegebenen Wert (als ein unsigned char interpretiert) .
*/
void* k_memset(void *dest, char val, size_t count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}
/*
Setzt die ersten num word(2Byte) der Speicherblock durch ptr, wies auf den angegebenen Wert (als ein unsigned char interpretiert) .
*/
USHORT* k_memsetw(USHORT* dest, USHORT val, size_t count)
{
    USHORT *temp = (USHORT*)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}
/*
gibt stringlänge zurück
*/
size_t k_strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

/*
reboot funktion
*/
void reboot()
{
  int temp;
    do //lösche den tastatur buffer
    {
       temp = inportb( 0x64 );
       if( temp & 1 )
         inportb( 0x60 );
    }
  while ( temp & 2 );

    // Reboot
    outportb(0x64, 0xFE);
}
/*
wandelt int zu string uf.
*/
void k_itoa(int value, char* valuestring)
{
  int min_flag;
  char swap, *p;
  min_flag = 0;

  if (0 > value)
  {
    *valuestring++ = '-';
    value = -INT_MAX > value ? min_flag = INT_MAX : -value;
  }

  p = valuestring;

  do
  {
    *p++ = (char)(value % 10) + '0';
    value /= 10;
  } while (value);

  if (min_flag != 0)
  {
    ++*valuestring;
  }
  *p-- = '\0';

  while (p > valuestring)
  {
    swap = *valuestring;
    *valuestring++ = *p;
    *p-- = swap;
  }
}
/*
wandelt int zu string um in Hex
*/
void k_i2hex(UINT val, UCHAR* dest, int len)
{
  UCHAR* cp;
  char x;
  UINT n;
  n = val;
  cp = &dest[len];
  while (cp > dest)
  {
    x = n & 0xF;
    n >>= 4;
    *--cp = x + ((x > 9) ? 'A' - 10 : '0');
  }
    dest[len]  ='h';
    dest[len+1]='\0';
}
/*
wandelt einen float(mit kommastellen) in einen string um
*/
void float2string(float value, int decimal, char* valuestring) 
{
   int neg = 0;    char tempstr[20];
   int i = 0;   int j = 0;   int c;    long int val1, val2;
   char* tempstring;
   tempstring = valuestring;
   if (value < 0)
     {neg = 1; value = -value;}
   for (j=0; j < decimal; ++j)
     {value = value * 10;}
   val1 = (value * 2);
   val2 = (val1 / 2) + (val1 % 2);
   while (val2 !=0)
   {
     if ((decimal > 0) && (i == decimal))
     {
       tempstr[i] = (char)(0x2E);
       ++i;
     }
     else
     {
       c = (val2 % 10);
       tempstr[i] = (char) (c + 0x30);
       val2 = val2 / 10;
       ++i;
     }
   }
   if (neg)
   {
     *tempstring = '-';
      ++tempstring;
   }
   i--;
   for (;i > -1;i--)
   {
     *tempstring = tempstr[i];
     ++tempstring;
   }
   *tempstring = '\0';
}

