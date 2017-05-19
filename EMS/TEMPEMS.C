/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
*             Expanded Memory Manager Library for Watcom C/C++               *
*                                 by                                         *
*                               Alioth                                       *
*                                                                            *
*                    Copyright (c) 1997 Don Williamson                       *
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 Right, before you start using this code, understand this; I can in no way
 be held responsible for the use of this code or indeed the problems that
 may arise from the use of it, be it direct OR inconsequential. The code is
 provided AS IS, without any guarantee whatsoever.
 This code may be used in any Public Domain Software without royalty. However,
 if you wish to use this code in ay Commercial Software, including Shareware,
 please give me credit - it's not necessary, but you lot are the only ones
 who can appreciate what I do - it helps!
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
*/

#include <stdio.h>
#include <dos.h>
#include <string.h>

#include "ems1.h"

/* ===---===--- Section: Functions ---===---=== */

byte EMM_Check_Install(void)
{
   union  REGS input_regs, output_regs;
   struct SREGS segment_regs;
   byte *EMM_Device_Name = "EMMXXXX0";        /* EMM present signature */
   byte *EMM_Device_Real;

   input_regs.h.ah = 0x35;
   input_regs.h.al = 0x67;
   intdosx(&input_regs, &output_regs, &segment_regs);

   EMM_Device_Real = BUILD_PTR(segment_regs.es, 0x0A);

   if (memcmp (EMM_Device_Name, EMM_Device_Real, 8) == 0) return (1);
   else return (0);
}

#pragma aux EMM_GVA = \
   "mov  ah, 46h" \
   "int  67h" \
   value [ax];

byte EMM_Get_Version(void)
{
   word error, value;

   value = EMM_GVA();
   if ((error = value >> 8))
   {
      EMMError = value;
      return(0);
   }
   EMMVersionMinor = value & 15;
   EMMVersionMajor = (value & 240) >> 4;
   return(1);
}

void EMM_Print_Error(void)
{
   byte x;

   for (x = 0; x < 34; x++)
   {
      if (EMMEMap[x] == EMMError)
         printf("\nERROR %xh: %s", EMMEMap[x], EMMEString[x]);
   }
}

#pragma aux EMM_Get_Pages = \
   "mov  ah, 42h" \
   "int  67h" \
   "mov  [EMMFreePages], bx" \
   "mov  [EMMTotalPages], dx" \
   "mov  [EMMError], ah" \
   modify [ax bx dx] \
   value [ah];

byte EMM_Alloc(word pages, word *emmh_ptr)
{
   union REGS inregs, outregs;

   inregs.h.ah = 0x43;
   inregs.x.bx = pages;
   int86(0x67, &inregs, &outregs);
   if (outregs.h.ah == 0)
   {
      *emmh_ptr = outregs.x.dx;
      return(1);
   }
   else
   {
      EMMError = outregs.h.ah;
      return(0);
   }
}

#pragma aux EMM_Map_Memory = \
   "mov  ah, 44h" \
   "int  67h" \
   "mov  [EMMError], ah" \
   "cmp  ah, 0" \
   "je   exit" \
   "mov  ah, 1" \
   "exit:" \
   parm [dx][al][bx] \
   modify [ax] \
   value [ah];

byte EMM_Get_Page_Frame(byte **pf_ptr)
{
   union REGS inregs, outregs;

   inregs.h.ah = 0x41;
   int86(0x67, &inregs, &outregs);
   if (outregs.h.ah != 0)
   {
      EMMError = outregs.h.ah;
      return(0);
   }
   else
   {
      *pf_ptr = BUILD_PTR(outregs.x.bx, 0);
      return(1);
   }
}

byte EMM_Free(word emmhand)
{
   union REGS inregs, outregs;

   inregs.h.ah = 0x45;
   inregs.x.dx = emmhand;
   int86(0x67, &inregs, &outregs);
   if (outregs.h.ah == 0) return(1);
   else
   {
      EMMError = outregs.h.ah;
      return(0);
   }
}

#pragma aux EMM_Realloc = \
   "mov  ah, 51h" \
   "int  67h" \
   "mov  [EMMError], ah" \
   "cmp  ah, 0" \
   "je   exit" \
   "mov  ah, 1" \
   "exit:" \
   parm [dx][bx] \
   modify [ax] \
   value [ah];

#pragma aux EMM_Num_Pages = \
   "mov  ah, 4Ch" \
   "int  67h" \
   "mov  [EMMError], ah" \
   parm [dx] \
   modify [ax] \
   value [bx];

/*
                      =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                       QuickLib EMM Library by Alioth
                      -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

You can map ANY of your 16k pages to the physical page 0 - therefore making
LOTS of memory available... how???...

   allocate your memory.
   map the page you need to the physical page 0.
   get the frame address (physical page 0), where your logical page is mapped.
   access it like it was any normal variable.....!!!!!!

Functions:

------------------------------------------------------------------------------
   EMM_Check_Install();
------------------------------------------------------------------------------

   IN: none.

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   Checks for the existance of an Expanded Memory Manager. If one is detected,
   a true state is returned.


------------------------------------------------------------------------------
   EMM_Get_Version();
------------------------------------------------------------------------------

   IN: none

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   Retrieves the Expanded Memory Manager's version number in Binary Coded
   Decimal format. The major version number (x.) is stored in the global
   variable "EMMVersionMajor", and the minor version number (.x) is stored
   in the global variable "EMMVersionMinor". If a true state was returned,
   the function was successful.


------------------------------------------------------------------------------
   EMM_Get_Pages();
------------------------------------------------------------------------------

   IN: none

   OUT: Error code

   DESCRIPTION:
   Requests amount of pages info. The amount of total pages provided by the
   Expanded Memory Manager is stored in the global variable "EMMTotalPages",
   and the amount of free pages is stored in the global variable -->
   "EMMFreePages". If the function returns a 0, then the operation was
   successful; if not, then the error code is returned.


------------------------------------------------------------------------------
   EMM_Alloc();
------------------------------------------------------------------------------

   IN: word    - Number of pages to allocate
       pointer - Address of the datas handle

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   Allocate a requested amount of 16k pages of memory and assign the group
   of pages a handle. If the state 1 is returned, the operation was
   successful and your memory area now has handle by which you can access it
   with.


------------------------------------------------------------------------------
   EMM_Map_Memory();
------------------------------------------------------------------------------

   IN: word - Data handle
       byte - Physical page number to map to
       word - Logical page number to map from

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   You can map previously allocated memory from the requested logical page,
   to any of the four physical pages that belong to the page frame. Once
   this is done, accessing the memory is merely a case of accessing one of
   the four physical pages (just like normal!). Upon return, if a state 1 is
   received, the operation was successful.


------------------------------------------------------------------------------
   EMM_Multiple_Map();
------------------------------------------------------------------------------

   IN: word - Data handle
       byte - Physical page number to map to (start)
       word - Logical page number to start mapping from
       word - Number of logical pages to map

    OUT: boolean - TRUE | FALSE

    DESCRIPTION:
    Instead of manually mapping individual pages if your memory is greater
    than one page, you can map all your pages from a beginning logical page
    to a beginning physical page. The number of pages to map must not exceed
    the total number of physical pages in the system. Upon return, if a state
    1 is received, the operation was successful.


------------------------------------------------------------------------------
   EMM_Get_Page_Frame();
------------------------------------------------------------------------------

   IN: pointer - Address of variable you want to assign to the frame's address

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   This function points a user-defined variable to the address of the page
   frame, physical page 0. This allows you to access whatever memory is
   located there. As usual, if the state returned is 1, the operation was
   successful.


------------------------------------------------------------------------------
   EMM_Realloc();
------------------------------------------------------------------------------

   IN: word - EMM handle
       word - Number of pages to allocate

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   Reallocates an already allocated EMM handle data area. If the state 1 is
   returned, the operation was successful.


------------------------------------------------------------------------------
   EMM_Num_Pages();
------------------------------------------------------------------------------

   IN: word - EMM handle

   OUT: word - Number of allocated pages

   DESCRIPTION:
   This function will return the total amount of 16k pages that have been
   allocated to a requested EMM handle.


------------------------------------------------------------------------------
   EMM_Free();
------------------------------------------------------------------------------

   IN: word - datas handle

   OUT: boolean - TRUE | FALSE

   DESCRIPTION:
   The final word in EMS manipulation - free that memory!!! :0)

*/
