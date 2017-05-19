#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include "dosgui.h"

extern char *SetupString;


void vblank(void)
{
   while (inp(0x3da) & 0x08)
      ;
   while (!(inp(0x3da) & 0x08))
      ;
}

byte char2int(byte inchar)
{
   byte temp;

   if (inchar == '0') temp = 0;
   if (inchar == '1') temp = 1;
   if (inchar == '2') temp = 2;
   if (inchar == '3') temp = 3;
   if (inchar == '4') temp = 4;
   if (inchar == '5') temp = 5;
   if (inchar == '6') temp = 6;
   if (inchar == '7') temp = 7;
   if (inchar == '8') temp = 8;
   if (inchar == '9') temp = 9;

   return(temp);
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
   unsigned char far *vidbuf = (unsigned char far *)0xB8000000L;
   unsigned char x;

   for (x = 0; x < 160; x += 2)
   {
      vidbuf[x + 0] = SetupString[x >> 1];
      vidbuf[x + 1] = 112;
      vidbuf[x + 3840] = 32;
      vidbuf[x + 3841] = 120;
   }
}

void DGI_CreateProgBar(DGIWin_ptr wmain, byte whandle, byte wxpos, byte wypos, byte wlen)
{
   wmain->PXPos[whandle] = wxpos;
   wmain->PYPos[whandle] = wypos;
   wmain->PLen[whandle]  = wlen;
   wmain->PFill[whandle] = 0;
}

byte DGI_MenuStart(DGIWin_ptr wmain, byte whandle)
{
   word inkey;
   byte ixpos;

   if (wmain->Type[whandle] != 1) return(0);
   DGI_DisplayWindow(wmain, whandle);
   ixpos = wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect];

   tisidml:
   inkey = getch();
   if (nisan(inkey) && wmain->Menu[whandle].IsStr[wmain->Menu[whandle].CurrSelect] && wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect] < wmain->Menu[whandle].StrTot[wmain->Menu[whandle].CurrSelect])
   {
      wmain->Menu[whandle].StrInfo[wmain->Menu[whandle].CurrSelect][wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect]] = inkey;
      wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect]++;
   }
   if (inkey == 8 && wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect] > 0)
   {
      wmain->Menu[whandle].StrInfo[wmain->Menu[whandle].CurrSelect][wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect] - 1] = '\0';
      wmain->Menu[whandle].StrLen[wmain->Menu[whandle].CurrSelect]--;
   }
   if (inkey == 13) return(wmain->Menu[whandle].CurrSelect + 1);
   if (inkey == 27) return(0);
   if (inkey == 0)
   {
      inkey = getch();
      if (inkey == 72 && wmain->Menu[whandle].CurrSelect > 0)
         wmain->Menu[whandle].CurrSelect--;
      if (inkey == 80 && wmain->Menu[whandle].CurrSelect < wmain->Menu[whandle].NumElem - 1)
         wmain->Menu[whandle].CurrSelect++;
      if (wmain->Menu[whandle].Element[wmain->Menu[whandle].CurrSelect][0] == 32 && inkey == 72 && wmain->Menu[whandle].CurrSelect > 0)
         wmain->Menu[whandle].CurrSelect--;
      if (wmain->Menu[whandle].Element[wmain->Menu[whandle].CurrSelect][0] == 32 && inkey == 80 && wmain->Menu[whandle].CurrSelect < wmain->Menu[whandle].NumElem - 1)
         wmain->Menu[whandle].CurrSelect++;
   }
   DGI_VBlank();
   DGI_DisplayWindow(wmain, whandle);
   goto tisidml;
   return(0);
}

void DGI_DisplayWindow(DGIWin_ptr wmain, byte whandle)
{
   byte far *vidbuf = (byte far *)0xB8000000L;
   byte boxdo = 0, bstart = 0, blen = 0, bref = 0;
   dword vidstrt, vidend;
   word x, y;
   dword mx, my;

   if (wmain->Type[whandle] == WT_FOOTHEAD)
   {
      vidstrt = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1);
      vidend  = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->Width[whandle] << 1);
      for (mx = vidstrt; mx < vidend; mx += 2)
         for (my = 0; my < (wmain->Height[whandle] * 160); my += 160)
            vidbuf[mx + my + 1] = 27; /* 23 */
      vidbuf[vidstrt]    = 218;
      vidbuf[vidstrt + (wmain->Width[whandle] << 1) - 2] = 191;
      vidstrt += 2;
      vidend  -= 2;
      for (mx = vidstrt; mx < vidend; mx += 2)
      {
         vidbuf[mx]     = 196;
         vidbuf[mx + 160] = 32;
         vidbuf[mx + 320] = 196;
         for (my = 480; my < ((wmain->Height[whandle] - 6) * 160) + 480; my += 160)
            vidbuf[mx + my] = 32;
         vidbuf[mx + my + 0]   = 196;
         vidbuf[mx + my + 160] = 32;
         vidbuf[mx + my + 320] = 196;
      }
      vidstrt -= 2;
      vidend  += 2;
      for (mx = vidstrt + 160; mx < vidstrt + ((wmain->Height[whandle] - 1) * 160); mx += 160)
      {
         vidbuf[mx] = 179;
         vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 179;
      }
      vidbuf[mx] = 192;
      vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 217;
      for (x = 0; x < (wmain->Width[whandle] - 2); x++)
      {
        if (wmain->Title[whandle][x] == 0) break;
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + 162 + (x << 1)] = wmain->Title[whandle][x];
      }
      for (x = 0; x < (wmain->Width[whandle] - 2); x++)
      {
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + 162 + (x << 1) + 1] = 113;
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (160 * (wmain->Height[whandle] - 1)) - 157 + (x << 1)] = 20;
      }
      for (x = 0; x < (wmain->Width[whandle] - 2); x++)
      {
        if (wmain->InfoString[whandle][x] == 0) break;
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (160 * (wmain->Height[whandle] - 1)) - 156 + (x << 1)] = wmain->InfoString[whandle][x];
      }
      vidbuf[vidstrt + 320] = 195;
      vidbuf[vidstrt + 320 + ((wmain->Width[whandle] - 1) << 1)] = 180;
      vidbuf[vidstrt + (160 * (wmain->Height[whandle] - 3))] = 195;
      vidbuf[vidstrt + (160 * (wmain->Height[whandle] - 3)) + ((wmain->Width[whandle] - 1) << 1)] = 180;
      if (wmain->PLen[whandle])
      {
         for (x = 0; x < wmain->PLen[whandle]; x++)
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->PYPos[whandle] * 160) + (wmain->PXPos[whandle] << 1) + (x << 1)] = 176;
         for (x = 0; x < wmain->PFill[whandle]; x++)
         {
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->PYPos[whandle] * 160) + (wmain->PXPos[whandle] << 1) + (x << 1)] = 178;
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->PYPos[whandle] * 160) + (wmain->PXPos[whandle] << 1) + (x << 1) + 1] = 68;
         }
      }
      for (x = 0; x < (wmain->NumStrings[whandle]); x++)
      {
         for (mx = wmain->StrX[whandle][x]; mx < (wmain->Width[whandle] - 2); mx++)
         {
            if (wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]] == 0) break;
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->StrY[whandle][x]  * 160) + (mx << 1)] = wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]];
         }
      }
      y = 0;
      if (wmain->Menu[whandle].NumElem > 0)
      {
         for (x = 1; x < 80; x++)
         {
            if (wmain->Menu[whandle].StatusInfo[wmain->Menu[whandle].CurrSelect][x - 1] == 0) y = 1;
            if (y == 0) vidbuf[3840 + (x << 1)] = wmain->Menu[whandle].StatusInfo[wmain->Menu[whandle].CurrSelect][x - 1];
            else vidbuf[3840 + (x << 1)] = 32;
         }
      }
      for (x = 0; x < (wmain->Menu[whandle].NumElem); x++)
      {
         for (y = 0; y < (wmain->Width[whandle] - 2); y++)
         {
            if (wmain->Menu[whandle].Element[x][y] == 0) break;
            if (wmain->Menu[whandle].Element[x][y] == '%')
            {
               boxdo  = 1;
               bstart = y;
               blen   = (char2int(wmain->Menu[whandle].Element[x][y + 1]) * 10) + char2int(wmain->Menu[whandle].Element[x][y + 2]);
               bref   = wmain->Menu[whandle].CurrSelect;
               wmain->Menu[whandle].IsStr[x]  = 1;
               wmain->Menu[whandle].StrTot[x] = blen;
            }
            if (!boxdo)
            {
               vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + (x * 160)] = wmain->Menu[whandle].Element[x][y];
               if (wmain->Menu[whandle].CurrSelect == x)
                  vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + 1 + (x * 160)] = 113;
               else vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + 1 + (x * 160)] = 23;
            }
            if (boxdo)
            {
               if (y - bstart < wmain->Menu[whandle].StrLen[x])
                  vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + (x * 160)] = wmain->Menu[whandle].StrInfo[x][y - bstart];
               else
                  vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + (x * 160)] = 32;
               if (wmain->Menu[whandle].CurrSelect == x)
               {
                  vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + (x * 160) + 1] = 11;
               }
               else
               {
                  vidbuf[(wmain->YPos[whandle] * 160) + ((wmain->XPos[whandle] + wmain->Menu[whandle].XPos) << 1) + (160 * (wmain->Menu[whandle].YPos)) + (y << 1) + 1 + (x * 160)] = 3;
               }
               if (y == bstart + blen)
               {
                  boxdo = 0;
                  y--;
               }
            }
         }
      }
   }
   if (wmain->Type[whandle] == WT_HEADONLY)
   {
      vidstrt = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1);
      vidend  = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1)+ (wmain->Width[whandle] << 1);
      for (mx = vidstrt; mx < vidend; mx += 2)
         for (my = 0; my < (wmain->Height[whandle] * 160); my += 160)
            vidbuf[mx + my + 1] = 120;
      vidbuf[vidstrt]    = 218;
      vidbuf[vidstrt + (wmain->Width[whandle] << 1) - 2] = 191;
      vidstrt += 2;
      vidend  -= 2;
      for (mx = vidstrt; mx < vidend; mx += 2)
      {
         vidbuf[mx]     = 196;
         vidbuf[mx + 160] = 32;
         vidbuf[mx + 320] = 196;
         for (my = 480; my < ((wmain->Height[whandle] - 4) * 160) + 480; my += 160)
            vidbuf[mx + my] = 32;
         vidbuf[mx + my + 0] = 196;
      }
      vidstrt -= 2;
      vidend  += 2;
      for (mx = vidstrt + 160; mx < vidstrt + ((wmain->Height[whandle] - 1) * 160); mx += 160)
      {
         vidbuf[mx] = 179;
         vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 179;
      }
      vidbuf[mx] = 192;
      vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 217;
      for (x = 0; x < (wmain->Width[whandle] - 2); x++)
      {
        if (wmain->Title[whandle][x] == 0) break;
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + 162 + (x << 1)] = wmain->Title[whandle][x];
      }
      for (x = 0; x < (wmain->Width[whandle] - 2); x++)
        vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + 162 + (x << 1) + 1] = 120;
      vidbuf[vidstrt + 320] = 195;
      vidbuf[vidstrt + 320 + ((wmain->Width[whandle] - 1) << 1)] = 180;
      for (x = 0; x < (wmain->NumStrings[whandle]); x++)
      {
         for (mx = wmain->StrX[whandle][x]; mx < (wmain->Width[whandle] - 2); mx++)
         {
            if (wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]] == 0) break;
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->StrY[whandle][x]  * 160) + (mx << 1)] = wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]];
         }
      }
   }
   if (wmain->Type[whandle] == WT_SINGLE)
   {
      vidstrt = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1);
      vidend  = (wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1)+ (wmain->Width[whandle] << 1);
      for (mx = vidstrt; mx < vidend; mx += 2)
         for (my = 0; my < (wmain->Height[whandle] * 160); my += 160)
            vidbuf[mx + my + 1] = 46;
      vidbuf[vidstrt]    = 218;
      vidbuf[vidstrt + (wmain->Width[whandle] << 1) - 2] = 191;
      vidstrt += 2;
      vidend  -= 2;
      for (mx = vidstrt; mx < vidend; mx += 2)
      {
         vidbuf[mx]     = 196;
         for (my = 160; my < ((wmain->Height[whandle] - 2) * 160) + 160; my += 160)
            vidbuf[mx + my] = 32;
         vidbuf[mx + my + 0] = 196;
      }
      vidstrt -= 2;
      vidend  += 2;
      for (mx = vidstrt + 160; mx < vidstrt + ((wmain->Height[whandle] - 1) * 160); mx += 160)
      {
         vidbuf[mx] = 179;
         vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 179;
      }
      vidbuf[mx] = 192;
      vidbuf[mx + (wmain->Width[whandle] << 1) - 2] = 217;
      for (x = 0; x < (wmain->NumStrings[whandle]); x++)
      {
         for (mx = wmain->StrX[whandle][x]; mx < (wmain->Width[whandle] - 1); mx++)
         {
            if (wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]] == 0) break;
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->StrY[whandle][x]  * 160) + (mx << 1)] = wmain->WindowString[whandle][x][mx - wmain->StrX[whandle][x]];
            vidbuf[(wmain->YPos[whandle] * 160) + (wmain->XPos[whandle] << 1) + (wmain->StrY[whandle][x]  * 160) + (mx << 1) + 1] = wmain->StrCol[whandle][x];
         }
      }
   }
}

void DGI_HideWindow(DGIWin_ptr wmain, byte whandle)
{
   wmain->Hidden[whandle] = 1;
}

void DGI_ShowWindow(DGIWin_ptr wmain, byte whandle)
{
   wmain->Hidden[whandle] = 0;
}

void DGI_UpdateScreen(DGIWin_ptr wmain)
{
   byte x;

//   DGI_VBlank();
   vblank();
   DGI_FillBackground();
   for (x = 0; x < 20; x++)
   {
      if (wmain->Used[x] && !wmain->Hidden[x])
         DGI_DisplayWindow(wmain, x);
   }
}

byte DGI_CreateWindow(DGIWin_ptr whandle, byte wxpos, byte wypos, byte wwidth,
                      byte wheight, byte wtype, byte whide, byte *wtitle,
                      byte *winfo, byte wmel, byte far *wmenu[], byte wmx,
                      byte wmy)
{
   byte x, y, mx;


   for (x = 0; x < 20; x++)
      whandle->PLen[x] = 0;
   for (x = 0; x < 20; x++)
      if (!whandle->Used[x]) break;
   if (x == 20) return(0xFF);

   whandle->XPos[x]       = wxpos;
   whandle->YPos[x]       = wypos;
   whandle->Width[x]      = wwidth;
   whandle->Height[x]     = wheight;
   whandle->Type[x]       = wtype;
   whandle->Used[x]       = 1;
   whandle->Hidden[x]     = whide;
   whandle->Title[x]      = wtitle;
   whandle->InfoString[x] = winfo;

   if (wtype == WT_FOOTHEAD)
   {
      whandle->Menu[x].XPos       = wmx;
      whandle->Menu[x].YPos       = wmy;
      whandle->Menu[x].CurrSelect = 0;
      whandle->Menu[x].NumElem    = wmel;
      for (y = 0; y < wmel; y++)
      {
         whandle->Menu[x].Element[y]    = wmenu[(y << 1)];
         whandle->Menu[x].StatusInfo[y] = wmenu[(y << 1) + 1];
         whandle->Menu[x].StrInfo[y]    = _fmalloc(80);
         for (mx = 0; mx < 80; mx++)
            whandle->Menu[x].StrInfo[y][mx] = '\0';
         whandle->Menu[x].IsStr[y]      = 0;
         whandle->Menu[x].StrLen[y]     = 0;
      }
   }

   return(x);
}

void DGI_RemoveWindow(DGIWin_ptr wmain, byte whandle)
{
   byte x;

   wmain->XPos[whandle]   = 0;
   wmain->YPos[whandle]   = 0;
   wmain->Width[whandle]  = 0;
   wmain->Height[whandle] = 0;
   wmain->Type[whandle]   = 0;
   wmain->Used[whandle]   = 0;
   for (x = 0; x <40; x++)
      _ffree(wmain->Menu[whandle].StrInfo[x]);
}

byte DGI_WriteString(DGIWin_ptr wmain, byte whandle, byte wxpos, byte wypos,
                     byte wcol, byte *wstring)
{
   byte x;

   x = wmain->NumStrings[whandle];

   wmain->WindowString[whandle][x] = wstring;
   wmain->StrX[whandle][x]         = wxpos;
   wmain->StrY[whandle][x]         = wypos;
   wmain->StrCol[whandle][x]       = wcol;
   wmain->NumStrings[whandle]++;

   return(x);
}

void DGI_Setup(DGIWin_ptr whandle)
{
   byte x;

   for (x = 0; x < 20; x++)
   {
      whandle->Used[x]            = 0;
      whandle->Hidden[x]          = 0;
      whandle->NumStrings[x]      = 0;
      whandle->Menu[x].NumElem    = 0;
      whandle->Menu[x].CurrSelect = 0;
   }
   DGI_VBlank();
   DGI_SetTextMode();
   DGI_HideCursor();
   DGI_ChangeColours();
   DGI_FillBackground();
   DGI_DisplayBanner();
}
