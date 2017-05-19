#define WT_FOOTHEAD 1
#define WT_HEADONLY 2
#define WT_SINGLE   3

typedef unsigned char  bool;
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;

typedef struct DGIMenu_strct
{
   byte NumElem;                /* Number of menu options */
   byte XPos, YPos;             /* Offset (X, Y) of menu from Window corner */
   byte far **Element;          /* Array holding menu options */
   byte far **StatusInfo;       /* Array holding option information */
   byte CurrSelect;             /* Currently selected option */

   bool far *IsStr;             /* Is the element an Input String */
   byte far *StrLen;            /* Array holding length of input strings */
   byte far *StrTot;            /* Total characters allowed */
   byte far **StrInfo;          /* Array holding input strings */
} DGIMenu;

typedef struct DGIWin_strct
{
   byte far *XPos, far *YPos;
   byte far *Width, far *Height;
   byte far *Type;
   bool far *Used, far *Hidden;
   byte far **Title;
   byte far **InfoString;

   byte far *NumStrings;
   byte far ***WindowString;
   byte far **StrX, far **StrY;
   byte far **StrCol;         /* Colour of string */

   /* Only used by Window Alpha */
   DGIMenu far *Menu;            /* Menu structure for window */

   byte far *PXPos, far *PYPos;      /* Progress Bar offset (X, Y) */
   byte far *PLen;               /* Length of progress bar */
   byte far *PFill;              /* Complete in progress bar */

} DGIWindowSet;
