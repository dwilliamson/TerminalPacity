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
   byte far *XPos, far *YPos;
   byte far *Width, far *Height;
   byte far *Type;
   bool far *Used, far *Hidden;
   byte far **Title;
   byte far **InfoString;

   /* Only used by Windows Beta & Gamma */
   byte far *NumStrings;
   byte far **WindowString[40];
   byte far *StrX[40], far *StrY[40];

   byte far *StrCol[40];         /* Colour of string */

   /* Only used by Window Alpha */
   DGIMenu far *Menu;            /* Menu structure for window */

   byte far *PXPos, far *PYPos;      /* Progress Bar offset (X, Y) */
   byte far *PLen;               /* Length of progress bar */
   byte far *PFill;              /* Complete in progress bar */

} DGIWindow, *DGIWin_ptr;


