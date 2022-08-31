;--------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              boot.asm                               ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;--------------------------------------------------------------;
;                                                              ;
;--------------------------------------------------------------;
;   Initialisierung:                                           ;
;--------------------------------------------------------------;
    org 0x7C00                                                 ; setzte Startadresse fest von diesem Code

    xor ax, ax                                                 ; Xor = 0 in akku
    mov ds, ax                                                 ; Data segment reg. auf 0 gesetzt
    mov es, ax                                                 ; Extra Segment reg. auf 0 gesetzt
    mov ss, ax                                                 ; Stack segment reg. auf 0 gesetzt
    mov sp, ax                                                 ; Stack Point auf 0 gesetzt
;--------------------------------------------------------------;
;   Start Boot-vorgang:                                        ;
;--------------------------------------------------------------;
    mov [bootdrive], dl                                        ; Bootlaufwerk aus DL speichern (adresse speichern)
    call load_kernel                                           ; ruft Subroutine auf. (Lade Kernel)
 
    jmp 0x0000:0x8000                                          ; Springe auf die Adresse des Kernels (seg. reg. 0x0000 x 16 + 0x8000 = 0x08000)
;--------------------------------------------------------------;
;   Nachrichten:                                               ;
;--------------------------------------------------------------;
    bootdrive db 0                                             ; Boot Laufwerk 
    loadmsg db "Bootloader: Lade Kernel...",13,10,0            ;
;--------------------------------------------------------------;
;                                                              ;
;   Subroutinen:                                               ;
;                                                              ;
;--------------------------------------------------------------;
;   Gib string auf Bildschirm aus:                             ;
;--------------------------------------------------------------;
print_string:                                                  ; Sprungpunkt
  mov ah, 0x0E                                                 ; lade 0x0E in das high Byte vom AKKU (VGA BIOS Funktion)

.loop:
  lodsb                                                        ; nim byte von (si)
  test al, al                                                  ; NUL?
  jz .done                                                     ; ist resultat 0 >> fertig
  int 0x10                                                     ; gib den Charakter aus (Buchstabe/Zahl/Zeichen)
  jmp .loop                                                    ; Springe zurück bis alle Charakter ausgegeben sind.

.done:                                                         ; Sprungpunkt
  ret                                                          ; Kehre aus Prozedur zurück
;--------------------------------------------------------------;
;   Lese Kernel vom Floppy ein:                                ;
;--------------------------------------------------------------;
load_kernel:
    xor ax, ax                                                 ; "reset" vom AKKU (XOR)
    int 0x13                                                   ; Laufwerk zurücksetzten         
    jc load_kernel                                             ; fehler? noch mal (zurückspringen)

    mov bx, 0x8000                                             ; Setzte startadresse vom Kernel
;--------------------------------------------------------------;
;   Parameters für lese Funktion  (intrupt 0x13)               ;
;--------------------------------------------------------------;
    mov dl,[bootdrive]                                         ; wähle Laufwerk aus (boot und kernel sind auf dem gleichem)
    mov al,40                                                  ; lese 40 Sektoren (40*512 = +- 20kByte ist max Code für WinBex.bin, sonst wird nicht ganzes OS nachgeladen)
    mov ch, 0                                                  ; cylinder = 0 
    mov cl, 2                                                  ; sector   = 2
    mov dh, 0                                                  ; head     = 0
    mov ah, 2                                                  ; funktion "lese"   
    int 0x13                                                   ; Bios funktion
    jc load_kernel                                             ; fehler? noch mal (zurückspringen)
;--------------------------------------------------------------;
;   Lade Bestätigung                                           ;
;--------------------------------------------------------------;
    mov si,loadmsg                                             ; läd Index in das Source Index(si) register (Sprungpunkt)
    call print_string                                          ; ruft Subroutine auf. (kernel geladen)
    ret                                                        ; Kehre aus Prozedur zurück

  times 510-($-$$) hlt                                         ; NASM, $ = akt. Prog. zeile, $$ = startaddresse (sprint auf die letzten 2 bytes)
  db 0x55                                                      ; Bootsignatur ausrichten
  db 0xAA                                                      ; Bootsignatur ausrichten
;--------------------------------------------------------------;