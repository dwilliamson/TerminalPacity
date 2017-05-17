/* File: ASMINC.C */

#pragma aux ASM_Video320 = \
   "mov  ah, 00h" \
   "mov  al, 013h" \
   "int  10h";

#pragma aux ASM_VideoTEXT = \
   "mov  ah, 00h" \
   "mov  al, 03h" \
   "int  10h";

