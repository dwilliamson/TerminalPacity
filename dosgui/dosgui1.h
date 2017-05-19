void DGI_SetTextMode(void);
void DGI_HideCursor(void);
void DGI_FillBackground(void);
void DGI_DisplayBanner(void);

/* Window Types for DGI_CreateWindow() */
#define WT_FOOTHEAD 1
#define WT_HEADONLY 2
#define WT_SINGLE   3

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

typedef struct DGIMenu_strct
{
   byte NumElem;                /* Number of menu options */
   byte XPos, YPos;             /* Offset (X, Y) of menu from Window corner */
   byte far *Element[20];       /* Array holding menu options */
   byte far *StatusInfo[20];    /* Array holding option information */
   byte CurrSelect;             /* Currently selected option */

   bool IsStr[20];              /* Is the element an Input String */
   byte StrLen[20];             /* Array holding length of input strings */
   byte StrTot[20];             /* Total characters allowed */
   byte far *StrInfo[40];       /* Array holding input strings */

} DGIMenu, *DGIMenu_ptr;

typedef struct DGIWin_strct
{
   byte XPos[20], YPos[20];
   byte Width[20], Height[20];
   byte Type[20];
   bool Used[20], Hidden[20];
   byte far *Title[20];
   byte far *InfoString[20];

   /* Only used by Windows Beta & Gamma */
   byte NumStrings[20];
   byte far *WindowString[20][40];
   byte StrX[20][40], StrY[20][40];

   byte StrCol[20][40];         /* Colour of string */

   /* Only used by Window Alpha */
   DGIMenu Menu[20];            /* Menu structure for window */

   byte PXPos[20], PYPos[20];   /* Progress Bar offset (X, Y) */
   byte PLen[20];               /* Length of progress bar */
   byte PFill[20];              /* Complete in progress bar */

} DGIWindow, *DGIWin_ptr;


