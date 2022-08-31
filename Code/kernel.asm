;--------------------------------------------------------------;
;   Name:               Eigenes OS in Asm/C/C++     >WinBex<   ; 
;   Autor:              Adrian Stoop                           ;
;   Datei:              kernel.asm                             ;
;   OS_Version:         V.0.01.00                              ;
;   Datum:              18.01.2016                             ;
;--------------------------------------------------------------;
;                                                              ;
;                                                              ;
;--------------------------------------------------------------;
;   Commands:                                                  ;
;               -  "hi"           = "Hallo Welt"               ;
;               -  "start_pm"        =  Springe in den PM      ;
;--------------------------------------------------------------;
;                                                              ;
;                                                              ;
;--------------------------------------------------------------;
;   Initialisierung:                                           ;
;--------------------------------------------------------------;
  ;org 0x8000                                                  ; Code startadresse
  [BITS 16]                                                    ; 16 Bit Code
  [global RealMode]
  [extern _main]                                               ; im C file
;--------------------------------------------------------------;
;   Initialisierung Real Mode:                                 ;
;--------------------------------------------------------------;
RealMode:
  xor ax, ax                                                   ; Xor = 0 in akku
  mov ds, ax                                                   ; Data segment reg. auf 0 gesetzt
  mov es, ax                                                   ; Extra Segment reg. auf 0 gesetzt
  mov ss, ax                                                   ; Stack segment reg. auf 0 gesetzt
  mov sp, ax                                                   ; Stack Point auf 0 gesetzt

  mov si, willkommen_1                                         ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf. (Willkommensnachricht 1)
  mov si, willkommen_2                                         ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf. (Willkommensnachricht 2)
  mov si, willkommen_3                                         ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf. (Willkommensnachricht 3)

  add sp, -0x40                                                ; Mache Platz frei für den input Buffer (64 chars)
;--------------------------------------------------------------;
;   Endlosschleife:                                            ;
;--------------------------------------------------------------;
loop:                                                          ; Endlosschlaufe
  mov si, befl_prompt                                          ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf. ($>)

  mov di, sp                                                   ; lade buffer grösse (nach $>) in Destination Index(di)(Ziel-Operanden reg.)
  call get_string                                              ; ruft Subroutine auf. (holle befehl)
  jcxz loop                                                    ; ist 0: ignorieren >> loop (schwarze linie)
;--------------------------------------------------------------;
;   Befehle überprüfen:                                        ;
;--------------------------------------------------------------;
  mov si, sp                                                   ; lade buffer(nach $>) in Source Index(si) register (Sprungpunkt). 
  mov di, befl_hi                                              ; verschibe "hi" ins (di)
  call strcmp                                                  ; ruft Subroutine auf. (vergleicht ob es dieser befl ist)
  je .hallowelt                                                ; sind sie gleich? springe zu befehl. sonst geh weiter.

  mov si, sp                                                   ; lade buffer(nach $>) in Source Index(si) register (Sprungpunkt).
  mov di, befl_pmode                                           ; verschibe "start_pm" ins (di)
  call strcmp                                                  ; ruft Subroutine auf. (vergleicht ob es dieser befl ist)
  je .pmode                                                    ; sind sie gleich? springe zu befehl. sonst geh weiter. 
;--------------------------------------------------------------;
;   Kein Befehl übereinstimmend:                               ;
;--------------------------------------------------------------;
  mov si, befl_falsch                                          ; kein befehl stimmt >> falscher befehl. schiebe "befl_falsch" in (si)
  call print_string                                            ; ruft Subroutine auf. (befehl nicht erkannt)
  jmp loop                                                     ; Springe zurück in die Endlosschleiffe
;--------------------------------------------------------------;
;   Befehle ausführen:                                         ;
;--------------------------------------------------------------;
.hallowelt:                                                    ; "Hallo Welt" Sprungpunkt
  mov si, msg_hallowelt                                        ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf.

  jmp loop                                                     ; zurück in die Endlosschleiffe für weitere befehle

.pmode:                                                        ; "Protected Mode" Sprungpunkt
  mov si, msg_pmode_1                                          ; läd Index in das Source Index(si) register (Sprungpunkt)
  call print_string                                            ; ruft Subroutine auf.

  cli                                                          ; lösche interrupts

  lgdt [gdtr]                                                  ; lade GDT von GDTR (in file "gtd.inc")

  in  al, 0x92                                                 ; switch A20 gate über fast A20 port 92
  cmp al, 0xff                                                 ; ist es auf liest 0xFF, nichts hat sich an diesem Port umgesetzt
  je .no_fast_A20
    
  or  al, 2                                                    ; set A20_Gate_Bit (bit 1)
  and al, ~1                                                   ; clear INIT_NOW bit 
  out 0x92, al
  jmp .A20_done
    
.no_fast_A20:                                                  ; kein shortcut -> benutzte langsame kbc...
  call empty_8042   
    
  mov al, 0xD1                                                 ; kbc befehl: schreibe zum ausgangs port
  out 0x64, al
  call empty_8042
    
  mov al, 0xDF                                                 ; schreibe das zum kbc ausgang port, enables A20
  out 0x60, al
  call empty_8042

.A20_done:
  mov eax, cr0                                                 ; wechsle zum Protected Mode
  or  eax, 1                                                   ; setze bit vom CR0 register auf 0
  mov cr0, eax       

  jmp 0x8:ProtectedMode                                        ; Springe in den PM

;--------------------------------------------------------------;
;   Nachrichten:                                               ;
;--------------------------------------------------------------;
willkommen_1 db 'Willkommen zu meinem OS, Winbex.', 13, 10, 0  ;
willkommen_2 db 'WinBex V. 0.01.00', 13, 10, 0                 ;
willkommen_3 db 'Stand: 18.01.2016', 13, 10, 0                 ;
befl_falsch db 'Befehl nicht erkannt.', 13, 10, 0              ;
befl_prompt db '<$>:', 0                                       ;
befl_hi db 'hi', 0                                             ;
befl_pmode db 'start_pm', 0                                    ;
msg_hallowelt db 'Hallo Welt!', 13, 10, 0                      ;
msg_pmode_1 db 'Wechsle in den Protected Mode.', 13, 10, 0
msg_pmode_2 db 'OS benutzt jetzt den Protected Mode.', 13, 10, 0
;--------------------------------------------------------------;
;                                                              ;
;   Subroutinen:                                               ;
;                                                              ;
;--------------------------------------------------------------;
empty_8042:
  in al, 0x64
  cmp al, 0xff                                                 ; keine echte kbc?
  je .done
    
  test al, 1                                                   ; ist etwas im Input Buffer?
  jz .no_output
  in al, 0x60                                                  ; Ja: lese Buffer
  jmp empty_8042                                               ; und probiere nochmal
    
.no_output:
  test al, 2                                                   ; Input Buffer lehr?
  jnz empty_8042                                               ; Nein: kann nichts senden bis Buffer lehr ist.
.done:
ret
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
;   Hohle befehl von Konsole:                                  ;
;--------------------------------------------------------------;
get_string:                                                    ; Sprungpunkt
  xor cx, cx                                                   ; Logische XOR von cx und sich selber (reset auf 0)

.loop:                                                         ; Sprungpunkt
  xor ax, ax                                                   ; resete Akku auf 0 (XOR)
  int 0x16                                                     ; warte auf Tastatur

  cmp al, 8                                                    ; wurde backspace gedrückt?
  je .backspace                                                ; Ja, rufe subroutine auf

  cmp al, 13                                                   ; wurde Enter gedrückt?
  je .done                                                     ; Ja, fertig

  cmp cl, 63                                                   ; sind schon 63 chars eingegeben?(max. buffergrösse)
  je .loop                                                     ; Ja, zurück in schleife nur noch Enter/Backspace möglich

  mov ah, 0x0E                                                 ; lade 0x0E in das high Byte vom AKKU
  int 0x10                                                     ; gib den Charakter aus (Buchstabe/Zahl/Zeichen)

  stosb                                                        ; Speichere Charakter in Buffer
  inc cx                                                       ; erhohe Counter (cx)
  jmp .loop                                                    ; Springe zum loop

.backspace:                                                    ; backspace gedrückt
  jcxz .loop                                                   ; 0? (anfang des string) wenn ja >> ingoriere tasteneingabe

  dec di                                                       ; vermindere (di) um eins
  mov byte [di], 0                                             ; lösche character
  dec cx                                                       ; vermindere counter um eins

  mov ah, 0x0E                                                 ; lade 0x0E in den AKKU
  int 0x10                                                     ; backspace auf dem Bildschirm (Konsole print)

  mov al, ' '                                                  ; lade " " in low Byte vom AKKU
  int 0x10                                                     ; überschreibe Charater

  mov al, 8                                                    ; lade 8 in das low Byte vom AKKU 
  int 0x10                                                     ; backspace zurück (Char wurde überschriben und curser zurück)

  jmp .loop                                                    ; Springe zurück (Backspace fertig)

.done:                                                         ; Sprungpunkt
  mov byte [di], 0                                             ; null terminator (Array fertig mit 0 signalisieren)

  mov ax, 0x0E0D                                               ; lade 0x0E0D in den AKKU
  int 0x10                                                     ; gib den Charakter aus (Buchstabe/Zahl/Zeichen)
  mov al, 0x0A                                                 ; lade 0x0A in das low Byte vom AKKU
  int 0x10                                                     ; gib den Charakter aus (Buchstabe/Zahl/Zeichen) (leer = newline)

  ret                                                          ; Kehre aus Prozedur zurück
;--------------------------------------------------------------;
;   vergleiche String: (befehlsauswahl)                        ;
;--------------------------------------------------------------;
strcmp:                                                        ; Sprungpunkt
.loop:
  mov al, [si]                                                 ; nim ein byte von SI (eingegebener befehl)
  cmp al, [di]                                                 ; sind die beiden byte gleich?
  jne .done                                                    ; Nein, nicht der befehl >> zurück

  or al, al                                                    ; Sind beide byte von vorher 0?
  jz .done                                                     ; Ja, fertig >> kommand gefunden

  inc di                                                       ; verringere DI
  inc si                                                       ; verringere SI
  jmp .loop                                                    ; Springe zur Schleiffe

.done:  
  ret 
;--------------------------------------------------------------;
;--------------------------------------------------------------;
;   Protected Mode                                             ;
;--------------------------------------------------------------;
;--------------------------------------------------------------;

[Bits 32]

ProtectedMode:
    mov    ax, 0x10
    mov    ds, ax                                              ; data descriptor --> data, stack and extra segment
    mov    ss, ax             
    mov    es, ax
    xor    eax, eax                                            ; reset Akku (32bit)
    mov    fs, ax
    mov    gs, ax
    mov    esp, 0x200000                                       ; setzte stack unter 2 MB limit

    mov ah,  0x01

    call _main                                                 ; C-Kernel laden
    jmp $
;--------------------------------------------------------------;
;   Includes                                                   ;
;--------------------------------------------------------------;
%include "gdt.inc"
;--------------------------------------------------------------;