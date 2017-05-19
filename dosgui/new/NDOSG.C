#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include "ndosgui.h"

#define CHAR2INT(x)     (x - 48)
#define INCREASE        1
#define DECREASE        -1
#define free_mem(x)     if (x) _ffree(x)

void    vblank(void);
void    copystring(byte far *, byte far *);
void   *re_alloc(void *, dword);
bool    nisan(byte);
void    InitialiseFirstWindow(void);
bool    UpdateWindowNumber(signed char);
void    CleanupWindowMem(void);
int     strlen(void *);
bool    searchstring(byte far *, byte);

extern char *SetupString;
byte far *vidbuf = (byte far *)0xB8000000L;
byte numslots = 0;
DGIWindowSet *internal_set;
bool mem_error;

void vblank(void)
{
   while (inp(0x3da) & 0x08)
      ;
   while (!(inp(0x3da) & 0x08))
      ;
}

bool searchstring(byte *string, byte ch)
{
   word x;

   for (x = 0; string[x] != 0; x++)
      if (string[x] == ch) return(1);
   return(0);
}

void copystring(byte far *str_dest, byte far *str_src)
{
   word x = 0;

   while (str_src[x] != 0)
   {
      str_dest[x] = str_src[x];
      x++;
   }
   str_dest[x] = 0;
}

int strlen(byte far *tstr)
{
   int x;
   for (x = 0; tstr[x] != 0; x++);
   return (x);
}

void *re_alloc(void *reptr, dword size)
{
   byte *tptr;

   tptr = reptr;
   reptr = _frealloc(reptr, size);
   if (!reptr) { mem_error = 1; return (tptr); }
   else return(reptr);
}

bool nisan(byte inchar)
{
   byte temp[67] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789:\\~|";
   byte x;

   for (x = 0; x < 66; x++) if (temp[x] == inchar) return(1);
   return(0);
}

void DGI_DisplayBanner(void)
{
   byte x;

   for (x = 0; x < 160; x += 2)
   {
      vidbuf[x + 0] = SetupString[x >> 1];
      vidbuf[x + 1] = 112;
      vidbuf[x + 3840] = 32;
      vidbuf[x + 3841] = 120;
   }
}

bool DGI_Setup(void)
{
   if (!(internal_set = _fmalloc(sizeof(DGIWindowSet)))) return (0);
   InitialiseFirstWindow();
   mem_error = 0;

   DGI_SetTextMode();
   DGI_HideCursor();
   DGI_ChangeColours();
   DGI_FillBackground();
   DGI_DisplayBanner();

   return (1);
}

void DGI_Close(void)
{
   byte x;

   DGI_SetTextMode();
   DGI_RestoreColours();
   for (x = numslots; x > 0; x--)
      if (internal_set->Used[x - 1]) DGI_DeleteWindow(x - 1);
   free_mem(internal_set);
}

byte DGI_CreateWindow(byte wxpos, byte wypos, byte wwidth, byte wheight,
                      byte wtype, byte whide, byte *wtitle, byte *winfo,
                      byte wmel, byte far **wmenu, byte wmx, byte wmy)
{
   byte x, y, mx;

   for (x = 1; x < numslots; x++)
      if (internal_set->Used[x] == 0) break;
   if (x == numslots) UpdateWindowNumber(INCREASE);

   internal_set->XPos[x]        = wxpos;
   internal_set->YPos[x]        = wypos;
   internal_set->Width[x]       = wwidth;
   internal_set->Height[x]      = wheight;
   internal_set->Type[x]        = wtype;
   internal_set->Used[x]        = 1;
   internal_set->Hidden[x]      = whide;

   internal_set->Title[x]       = _fmalloc(strlen(wtitle) + 1);
   internal_set->InfoString[x]  = _fmalloc(strlen(winfo) + 1);
   copystring(internal_set->Title[x], wtitle);
   copystring(internal_set->InfoString[x], winfo);

   internal_set->NumStrings[x] = 0;

   if (wtype == WT_FOOTHEAD)
   {
      internal_set->Menu[x].NumElem     = 0;
      internal_set->Menu[x].XPos        = wmx;
      internal_set->Menu[x].YPos        = wmy;
      internal_set->Menu[x].CurrSelect  = 0;
      internal_set->Menu[x].NumElem     = wmel;
      internal_set->Menu[x].Element     = _fmalloc(wmel * sizeof(byte far *));
      internal_set->Menu[x].StatusInfo  = _fmalloc(wmel * sizeof(byte far *));
      internal_set->Menu[x].StrInfo     = _fmalloc(wmel * sizeof(byte far *));
      internal_set->Menu[x].IsStr       = _fmalloc(wmel * sizeof(bool));
      internal_set->Menu[x].StrLen      = _fmalloc(wmel * sizeof(byte));
      internal_set->Menu[x].StrTot      = _fmalloc(wmel * sizeof(byte));
      for (y = 0; y < wmel; y++)
      {
         internal_set->Menu[x].Element[y] = _fmalloc(strlen(wmenu[(y << 1)]) + 1);
         internal_set->Menu[x].StatusInfo[y] = _fmalloc(strlen(wmenu[(y << 1) + 1]) + 1);
         if (searchstring(wmenu[(y << 1)], '%%'))
         {
            internal_set->Menu[x].StrInfo[y] = _fmalloc(80);
            for (mx = 0; mx < 80; mx++) internal_set->Menu[x].StrInfo[y] = 0;
         }
         copystring(internal_set->Menu[x].Element[y], wmenu[(y << 1)]);
         copystring(internal_set->Menu[x].StatusInfo[y], wmenu[(y << 1) + 1]);
         internal_set->Menu[x].IsStr[y] = 0;
         internal_set->Menu[x].StrLen[y] = 0;
      }
   }

   return (x);
}

void InitialiseFirstWindow(void)
{
   internal_set->XPos       = _fmalloc(sizeof(byte));
   internal_set->YPos       = _fmalloc(sizeof(byte));
   internal_set->Width      = _fmalloc(sizeof(byte));
   internal_set->Height     = _fmalloc(sizeof(byte));
   internal_set->Type       = _fmalloc(sizeof(byte));
   internal_set->Used       = _fmalloc(sizeof(bool));
   internal_set->Hidden     = _fmalloc(sizeof(bool));
   internal_set->Title      = _fmalloc(sizeof(byte far *));
   internal_set->InfoString = _fmalloc(sizeof(byte far *));

   internal_set->NumStrings   = _fmalloc(sizeof(byte));
   internal_set->WindowString = _fmalloc(sizeof(byte far *));
   internal_set->StrX         = _fmalloc(sizeof(byte far *));
   internal_set->StrY         = _fmalloc(sizeof(byte far *));
   internal_set->StrCol       = _fmalloc(sizeof(byte far *));

   internal_set->Menu       = _fmalloc(sizeof(DGIWindowSet));

   internal_set->PXPos      = _fmalloc(sizeof(byte));
   internal_set->PYPos      = _fmalloc(sizeof(byte));
   internal_set->PLen       = _fmalloc(sizeof(byte));
   internal_set->PFill      = _fmalloc(sizeof(byte));

   internal_set->Used[0] = 1;
   internal_set->NumStrings[0] = 0;
   internal_set->Menu[0].NumElem = 0;
   numslots = 1;
}

bool UpdateWindowNumber(signed char action)
{
   byte x;

   if (action == DECREASE && numslots == 0) return (0);
   if (action == INCREASE && numslots == 255) return (0);
   x = numslots + action;

   if (x == 0)
   {
      CleanupWindowMem();
      return (1);
   }

   internal_set->XPos       = re_alloc(internal_set->XPos, x * sizeof(byte));
   internal_set->YPos       = re_alloc(internal_set->YPos, x * sizeof(byte));
   internal_set->Width      = re_alloc(internal_set->Width, x * sizeof(byte));
   internal_set->Height     = re_alloc(internal_set->Height, x * sizeof(byte));
   internal_set->Type       = re_alloc(internal_set->Type, x * sizeof(byte));
   internal_set->Used       = re_alloc(internal_set->Used, x * sizeof(bool));
   internal_set->Hidden     = re_alloc(internal_set->Hidden, x * sizeof(bool));
   internal_set->Title      = re_alloc(internal_set->Title, x * sizeof(byte far *));
   internal_set->InfoString = re_alloc(internal_set->InfoString, x * sizeof(byte far *));

   internal_set->NumStrings   = re_alloc(internal_set->NumStrings, x * sizeof(byte));
   internal_set->WindowString = re_alloc(internal_set->WindowString, x * sizeof(byte far *));
   internal_set->StrX         = re_alloc(internal_set->StrX, x * sizeof(byte far *));
   internal_set->StrY         = re_alloc(internal_set->StrY, x * sizeof(byte far *));
   internal_set->StrCol       = re_alloc(internal_set->StrCol, x * sizeof(byte far *));

   internal_set->Menu       = re_alloc(internal_set->Menu, x * sizeof(DGIWindowSet));

   internal_set->PXPos      = re_alloc(internal_set->PXPos, x * sizeof(byte));
   internal_set->PYPos      = re_alloc(internal_set->PYPos, x * sizeof(byte));
   internal_set->PLen       = re_alloc(internal_set->PLen, x * sizeof(byte));
   internal_set->PFill      = re_alloc(internal_set->PFill, x * sizeof(byte));

   if (mem_error) return (0);
   mem_error = 0;
   numslots += action;
   return (1);
}

void CleanupWindowMem(void)
{
   free_mem(internal_set->XPos);
   free_mem(internal_set->YPos);
   free_mem(internal_set->Width);
   free_mem(internal_set->Height);
   free_mem(internal_set->Type);
   free_mem(internal_set->Used);
   free_mem(internal_set->Hidden);
   free_mem(internal_set->Title);
   free_mem(internal_set->InfoString);
   free_mem(internal_set->NumStrings);
   free_mem(internal_set->WindowString);
   free_mem(internal_set->StrX);
   free_mem(internal_set->StrY);
   free_mem(internal_set->StrCol);
   free_mem(internal_set->Menu);
   free_mem(internal_set->PXPos);
   free_mem(internal_set->PYPos);
   free_mem(internal_set->PLen);
   free_mem(internal_set->PFill);
   numslots = 0;
}

void DGI_DeleteWindow(byte whandle)
{
   byte x;

   internal_set->Used[whandle] = 0;

   free_mem(internal_set->Title[whandle]);
   free_mem(internal_set->InfoString[whandle]);
   for (x = 0; x < internal_set->NumStrings[whandle]; x++)
      free_mem(internal_set->WindowString[whandle][x]);
   free_mem(internal_set->WindowString[whandle]);
   free_mem(internal_set->StrX[whandle]);
   free_mem(internal_set->StrY[whandle]);
   free_mem(internal_set->StrCol[whandle]);
   if (internal_set->Type[whandle] == WT_FOOTHEAD)
   {
      for (x = 0; x < internal_set->Menu[whandle].NumElem; x++)
      {
         free_mem(internal_set->Menu[whandle].Element[x]);
         free_mem(internal_set->Menu[whandle].StatusInfo[x]);
         free_mem(internal_set->Menu[whandle].StrInfo[x]);
      }
      free_mem(internal_set->Menu[whandle].Element);
      free_mem(internal_set->Menu[whandle].StatusInfo);
      free_mem(internal_set->Menu[whandle].IsStr);
      free_mem(internal_set->Menu[whandle].StrLen);
      free_mem(internal_set->Menu[whandle].StrTot);
      free_mem(internal_set->Menu[whandle].StrInfo);
   }

   if (whandle == numslots - 1 || whandle == 0) UpdateWindowNumber(DECREASE);
}

void DGI_CreateProgBar(byte whandle, byte wxpos, byte wypos, byte wlen)
{
   internal_set->PXPos[whandle] = wxpos;
   internal_set->PYPos[whandle] = wypos;
   internal_set->PLen[whandle]  = wlen;
   internal_set->PFill[whandle] = 0;
}
