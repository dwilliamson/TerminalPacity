#include <stdio.h>
#include <process.h>
#include <dos.h>
#include <malloc.h>
#include <direct.h>
#include "dosgui.h"

#define BUFSIZE 50 /* This must be less than [filesize / 40] */

char *SetupString = { " Universal Setup - Version 1.0                Copyright (c) 1997 Don Williamson " };
DGIWindow tpacity;

byte far *pacmenu[] = { "Install Terminal Pacity", "Install Terminal Pacity to a selected Drive",
                        "View Playing Info      ", "Read what the game is all about          ",
                        "Exit                   ", "Leave the Install Program" };
byte far *insmenu[] = { "Path %39                                     ", "Type the location you wish to install Terminal Pacity to" };
FILE *fpop, *fpwr;
byte drv1, drv2;
word drvnum;

void main(void)
{
   byte mywindow, newindow, hiwindow, inkey = 32, menures, inwindow;
   byte infowin,  prwin, exitval = 0;
   byte *currdir, *newdir, inb, far *filestore;
   dword tflen, mxe, count, mxy;
   float interval;
   word x;

   currdir = malloc(200);
   getcwd(currdir, 200);
   DGI_Setup((DGIWin_ptr)&tpacity);

   inwindow = DGI_CreateWindow((DGIWin_ptr)&tpacity, 15, 10, 50, 7,
                               WT_FOOTHEAD, 1, " Game Location", "ESC = Exit      ENTER = Accept",
                               1, insmenu, 3, 3);
   mywindow = DGI_CreateWindow((DGIWin_ptr)&tpacity, 15, 8, 50, 11,
                               WT_FOOTHEAD, 0, "                    Main Menu", "ESC = Exit        ENTER = Accept",
                               3, pacmenu, 12, 4);
   newindow = DGI_CreateWindow((DGIWin_ptr)&tpacity, 25, 2, 30, 5,
                               WT_HEADONLY, 0, "   Terminal Pacity Install",
                               NULL, NULL, NULL, NULL, NULL);
   hiwindow = DGI_CreateWindow((DGIWin_ptr)&tpacity, 10, 20, 60, 3,
                               WT_SINGLE, 0, NULL, NULL, NULL, NULL, NULL, NULL);

   prwin = DGI_CreateWindow((DGIWin_ptr)&tpacity, 15, 8, 50, 9, WT_FOOTHEAD, 1,
                            " Copying Archive: Progress", " ...Please Wait...", 0, NULL, NULL, NULL);

/*-------------------------------------------*/
   infowin = DGI_CreateWindow((DGIWin_ptr)&tpacity, 3, 2, 74, 21,
                               WT_SINGLE, 1, NULL, NULL, NULL, NULL, NULL, NULL);

   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 1, 32, "Once the game is installed, you can start it straight away by typing");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 2, 32, "PACMAN. There are also a few command-line options...");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 4, 32, "   -nosb               Don't detect SoundCard");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 5, 32, "   -l <level>          Load level");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 7, 32, "It is now possible to cheat in Terminal Pacity. There are, so far");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 8, 32, "only two. They are...");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 10, 32, "   ALI     Gain a life");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 11, 32, "   ALO     Advance a level");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 13, 32, "Terminal Pacity has the ability to load and save games provided");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 14, 32, "Pac is on the ground (not mid-jump). To save your game (it saves");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 15, 32, "your current position on the level with you score and other stuff),");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 16, 32, "press F2 in the game, use the arrow keys to select a slot. Then, to");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 17, 32, "load your game, press F3. Note that external levels CAN also have");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 18, 32, "games saved on them.");
   DGI_WriteString((DGIWin_ptr)&tpacity, infowin, 2, 19, 32, "Terminal Pacity / PacEdit - Copyright (c) 1997 Don Williamson");

/*-------------------------------------------*/

   DGI_WriteString((DGIWin_ptr)&tpacity, hiwindow, 61, 1, 32,
                   "Welcome to Terminal Pacity!!!");
   DGI_CreateProgBar((DGIWin_ptr)&tpacity, prwin, 5, 5, 40);
   DGI_WriteString((DGIWin_ptr)&tpacity, newindow, 12, 3, 120, "DEMO");
   DGI_UpdateScreen((DGIWin_ptr)&tpacity);

   doag:
   if (kbhit()) if((inkey = getch()) == 0) getch();
   DGI_VBlank();
   DGI_DisplayWindow((DGIWin_ptr)&tpacity, hiwindow);
   tpacity.StrX[hiwindow][0]--;
   delay(60);
   if (inkey == ' ' && tpacity.StrX[hiwindow][0] > 15) goto doag;
   tpacity.StrX[hiwindow][0] = 15;
   DGI_DisplayWindow((DGIWin_ptr)&tpacity, hiwindow);

   tpacity.Menu[inwindow].StrInfo[0][0] = 'C';
   tpacity.Menu[inwindow].StrInfo[0][1] = ':';
   tpacity.Menu[inwindow].StrInfo[0][2] = '\\';
   tpacity.Menu[inwindow].StrInfo[0][3] = 'P';
   tpacity.Menu[inwindow].StrInfo[0][4] = 'A';
   tpacity.Menu[inwindow].StrInfo[0][5] = 'C';
   tpacity.Menu[inwindow].StrInfo[0][6] = 'I';
   tpacity.Menu[inwindow].StrInfo[0][7] = 'T';
   tpacity.Menu[inwindow].StrInfo[0][8] = 'Y';
   tpacity.Menu[inwindow].StrLen[0] = 9;
   tpacity.Menu[inwindow].StrInfo[0][9] = '\0';

   doag2:;
   menures = DGI_MenuStart((DGIWin_ptr)&tpacity, mywindow);
   if (menures == 1)
   {
      DGI_HideWindow((DGIWin_ptr)&tpacity, mywindow);
      DGI_ShowWindow((DGIWin_ptr)&tpacity, inwindow);
      DGI_UpdateScreen((DGIWin_ptr)&tpacity);
      menures = DGI_MenuStart((DGIWin_ptr)&tpacity, inwindow);
      if (menures == 0) goto doag2;
      if (menures == 1)
      {
         mxe = tpacity.Menu[inwindow].StrInfo[0][0];
         if (tpacity.Menu[inwindow].StrInfo[0][tpacity.Menu[inwindow].StrLen[0] - 1] == '\\')
            tpacity.Menu[inwindow].StrInfo[0][tpacity.Menu[inwindow].StrLen[0] - 1] = '\0';
         drv1 = toupper(mxe) - 64;
         drv2 = toupper(currdir) - 64;
         if (chdir(tpacity.Menu[inwindow].StrInfo[0]) == -1)
         {
            mkdir(tpacity.Menu[inwindow].StrInfo[0]);
            chdir(tpacity.Menu[inwindow].StrInfo[0]);
         }
         DGI_HideWindow((DGIWin_ptr)&tpacity, inwindow);
         DGI_ShowWindow((DGIWin_ptr)&tpacity, prwin);
         DGI_UpdateScreen((DGIWin_ptr)&tpacity);
         if (currdir[3] == '\0') strcat(currdir, "PACSETUP.EXE");
         else strcat(currdir, "\\PACSETUP.EXE");
         fpop = fopen(currdir, "rb");
         _dos_setdrive(drv1, &drvnum);
         fpwr = fopen("EXTRACT.EXE", "wb");
         fseek(fpop, 0L, SEEK_END);
         tflen = ftell(fpop);
         fseek(fpop, 0L, SEEK_SET);

         _dos_setdrive(drv2, &drvnum);
         for (mxe = 0; mxe < tflen; mxe++)
         {
            if (fgetc(fpop) == 167)
            {
               mxe++;
               if (fgetc(fpop) == 167)
               {
                  mxe++;
                  if (fgetc(fpop) == 167)
                  {
                     tflen -= mxe;
                     goto medone;
                  }
               }
            }
         }
         if (mxe == tflen)
         {
            exitval = 5;
            goto daplace;
         }
         medone:;

         count = 0;
         interval = tflen / 40;
         filestore = _fmalloc(BUFSIZE + 1000);
         for (mxe = 0; mxe < tflen; mxe += BUFSIZE)
         {
            DGI_VBlank();
            DGI_DisplayWindow((DGIWin_ptr)&tpacity, prwin);
            if (mxe > count * interval)
            {
               count++;
               tpacity.PFill[prwin]++;
            }
            _dos_setdrive(drv2, &drvnum);
            for (mxy = 0; mxy < BUFSIZE; mxy++)
               filestore[mxy] = fgetc(fpop);
            _dos_setdrive(drv1, &drvnum);
            for (mxy = 0; mxy < BUFSIZE; mxy++)
               fputc(255 - filestore[mxy], fpwr);
         }
         tpacity.PFill[prwin] = 40;
         DGI_DisplayWindow((DGIWin_ptr)&tpacity, prwin);
         fclose(fpop);
         fclose(fpwr);
         _ffree(filestore);
      }
      DGI_HideWindow((DGIWin_ptr)&tpacity, prwin);
      DGI_ShowWindow((DGIWin_ptr)&tpacity, mywindow);
      DGI_UpdateScreen((DGIWin_ptr)&tpacity);
   }
   if (menures == 2)
   {
      DGI_HideWindow((DGIWin_ptr)&tpacity, mywindow);
      DGI_ShowWindow((DGIWin_ptr)&tpacity, infowin);
      DGI_UpdateScreen((DGIWin_ptr)&tpacity);
      inkey = getch();
      if (inkey == 0) getch();
      DGI_HideWindow((DGIWin_ptr)&tpacity, infowin);
      DGI_ShowWindow((DGIWin_ptr)&tpacity, mywindow);
      DGI_UpdateScreen((DGIWin_ptr)&tpacity);
      goto doag2;
   }
   if (menures == 3)
   {
      exitval = 6;
   }
   daplace:;
   DGI_RemoveWindow((DGIWin_ptr)&tpacity, mywindow);
   DGI_RemoveWindow((DGIWin_ptr)&tpacity, newindow);
   DGI_RemoveWindow((DGIWin_ptr)&tpacity, hiwindow);
   DGI_RemoveWindow((DGIWin_ptr)&tpacity, inwindow);
   DGI_SetTextMode();
   DGI_RestoreColours();

   free(currdir);
   if (exitval == 0)
   {
      spawnl(P_WAIT, "extract.exe", 0);
      spawnl(P_WAIT, "setup.bat", 0);
   }
   if (exitval == 5)
      printf("This Install program has been used before...");
   exit(exitval);
}
