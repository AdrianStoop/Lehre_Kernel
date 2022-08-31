@echo off
del WinBex.bin

nasm -O32 -f bin boot.asm -o boot.bin
nasm -O32 -f aout kernel.asm -o kernel.o
nasm -O32 -f aout isr.asm -o isr.o 

pause

gcc  -Wall -O  -c ckernel.c -o ckernel.o -O1   
gcc  -Wall -O  -c video.c -o video.o -O1
gcc  -Wall -O  -c util.c -o util.o -O1
gcc  -Wall -O  -c gdt.c -o gdt.o
gcc  -Wall -O  -c idt.c -o idt.o
gcc  -Wall -O  -c isrs.c -o isrs.o	
gcc  -Wall -O  -c irq.c -o irq.o
gcc  -Wall -O  -c timer.c -o timer.o 	
gcc  -Wall -O  -c keyboard.c -o keyboard.o

pause

ld -T kernel.ld kernel.o isr.o ckernel.o video.o gdt.o idt.o isrs.o irq.o util.o timer.o keyboard.o -o ckernel.bin --verbose
	
pause


copy /b boot.bin + ckernel.bin WinBex

pause

del *.o
del *.bin
rename WinBex WinBex.bin
exit
