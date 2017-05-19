#include <stdio.h>
#include <dos.h>
#include <string.h>

#include "ems.h"

void main(void)
{
   word emmhandle;
   byte *pf_addr;

   printf("\n\n\n");
   if (!EMM_Check_Install())
   {
      printf("Expanded Memory Manager not detected...");
      exit(0);
   }
   printf("Expanded Memory Manager detected...");
   EMM_Get_Page_Frame(&pf_addr);
   if (!EMM_Get_Version())
   {
      EMM_Print_Error();
      exit(0);
   }
   printf("\nEMM Version: %x.%x", EMMVersionMajor, EMMVersionMinor);
   if (EMM_Get_Pages())
   {
      EMM_Print_Error();
      exit(0);
   }
   printf("\nFree EMS Pages: %u", EMMFreePages);
   printf("\nTotal EMS Pages: %u", EMMTotalPages);
   if (!EMM_Alloc(6, &emmhandle))
   {
      EMM_Print_Error();
      exit(0);
   }
   printf("\nAllocated Pages: %u", (char)EMM_Num_Pages(emmhandle));
   if (EMM_Map_Memory(emmhandle, 0, 0))
   {
      EMM_Print_Error();
      exit(0);
   }
   pf_addr[16000] = 65;
   printf("\n%c", pf_addr[16000]);
   EMM_Realloc(emmhandle, 8);
   printf("\nAllocated Pages: %i", EMM_Num_Pages(emmhandle));
   printf("\n%c", pf_addr[16000]);
   EMM_Free(emmhandle);
   printf("\n%x", FP_SEG(pf_addr));
   printf("\n\n\n");
}

