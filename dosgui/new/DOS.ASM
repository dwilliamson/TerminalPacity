     TITLE dos.asm

   public DGI_SetTextMode_
   public DGI_HideCursor_
   public DGI_ShowCursor_
   public DGI_FillBackground_
   public DGI_ChangeColours_
   public DGI_RestoreColours_
   public DGI_VBlank_

.286
_DATA   SEGMENT PUBLIC 'DATA'


_DATA   ends

_TEXT   SEGMENT PUBLIC 'CODE'


DGI_SetTextMode_ PROC

   mov  al, 03h                         ; BIOS mode 03h (text mode)
   xor  ah, ah                          ; Clear video memory upon entry
   int  10h                             ; BIOS video interrupt
   ret

DGI_SetTextMode_ ENDP


DGI_HideCursor_ PROC

   push ax
   push dx
   mov  dx, 3D4h                        ; Set bit 5 of port 3D4h, port index
   mov  ax, 100Ah                       ;  0Ah to disable the cursor
   out  dx, ax
   mov  al, 0Bh                         ; Bit 5
   out  dx, ax
   pop  dx
   pop  ax
   ret

DGI_HideCursor_ ENDP


DGI_ShowCursor_ PROC

   push ax
   push dx
   mov  dx, 3D4h                        ; Set bit 5 of port 3D4h, port index
   mov  ax, 100Ah                       ;  0Ah to disable the cursor
   out  dx, ax
   mov  al, 00h                         ; Bit 5
   out  dx, ax
   pop  dx
   pop  ax
   ret

DGI_ShowCursor_ ENDP


DGI_FillBackground_ PROC

   push ax
   push es
   push di
   push cx
   push ds
   push si

   mov  ax, 0B800h
   mov  es, ax

   mov  di, 160
   mov  cx, 1840
   mov  al, 177
   mov  ah, 23
   rep  stosw
   pop  si
   pop  ds
   pop  cx
   pop  di
   pop  es
   pop  ax
   ret

DGI_FillBackground_ ENDP


DGI_ChangeColours_ PROC

    mov  dx, 3C8h
    mov  al, 1
    out  dx, al

    inc  dx
    out  dx, al
    mov  al, 5
    out  dx, al
    mov  al, 16
    out  dx, al
    ret

DGI_ChangeColours_ ENDP

DGI_RestoreColours_ PROC

    mov     dx, 3C8h
    mov     al, 1
    out     dx, al

    inc     dx
    xor     al, al
    out     dx, al
    out     dx, al
    mov     al, 42
    out     dx, al
    ret

DGI_RestoreColours_ ENDP


DGI_VBlank_ PROC

   push dx
   push ax
   mov  dx, 3DAh

@wait1:
   in   al, dx
   and  al, 08h
   jnz  @wait1

@wait2:
   in   al, dx
   and  al, 08h
   jz   @wait2
   pop  ax
   pop  dx

DGI_VBlank_ ENDP


_TEXT   ends

end
