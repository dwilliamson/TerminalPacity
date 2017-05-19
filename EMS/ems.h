/* ===---===--- Section: TYPE-DEFINITIONS ---===---=== */

typedef unsigned char byte;                     /* Just cos ASM rules! */
typedef unsigned int  word;
typedef unsigned long dword;

typedef struct EMM_Memory_Store /* Just in case you're organised! :) */
{
   word handle;                                 /* EMM handle number */
   word pages;                                  /* Number of allocated pages */
} EMMDataStruct, *EMMDataPtr;

/* ===---===--- Section: DEFINITIONS ---===---=== */

#define BUILD_PTR(x, y) (char *)(((dword)x << 16) + y)

/* ===---===--- Section: Global Variables ---===---=== */

extern byte   EMMVersionMajor;
extern byte   EMMVersionMinor;
extern word   EMMFreePages;
extern word   EMMTotalPages;

/* ===---===--- Section: Prototypes ---===---=== */

/*
Function explanations (the ones you will need to use):

EMM_Check_Install   - Check for the existance of an EMM
EMM_Get_Version     - Get the current EMM version number
EMM_Get_Pages       - Get number of free and total pages in the EMM system
EMM_Alloc           - Setup handle and allocate memory pages
EMM_Map_Memory      - Map logical page into physical page window
EMM_Get_Page_Frame  - Get page frame base address
EMM_Realloc         - Reallocate pages
EMM_Num_Pages       - Get page count for handle
EMM_Free            - Release handle and memory pages

*/

byte EMM_Check_Install(void);
word EMM_GVA(void);
byte EMM_Get_Version(void);
void EMM_Print_Error(void);
byte EMM_Get_Pages(void);
byte EMM_Alloc(word pages, word *emmh_ptr);
byte EMM_Map_Memory(word, byte, word);
byte EMM_Get_Page_Frame(byte **pf_ptr);
byte EMM_Free(word emmhand);
byte EMM_Realloc(word, word);
word EMM_Num_Pages(word);     

#pragma aux EMM_GVA = value [ax];
#pragma aux EMM_Get_Pages = modify [ax bx dx] value [ah];
#pragma aux EMM_Map_Memory = parm [dx][al][bx] modify [ax] value [ah];
#pragma aux EMM_Realloc = parm [dx][bx] modify [ax] value[ah];
#pragma aux EMM_Num_Pages = parm [dx] modify [ax] value [bx];
