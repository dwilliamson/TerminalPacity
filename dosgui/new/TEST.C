#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include "ndosgui.h"

extern byte numslots;
extern DGIWindowSet *internal_set;

char *SetupString = { " Universal Setup - Version 1.0                Copyright (c) 1997 Don Williamson " };

byte far *pacmenu[] = { "Install Terminal Pacity", "Install Terminal Pacity to a selected Drive",
                        "View Playing Info      ", "Read what the game is all about          ",
                        "Exit                   ", "Leave the Install Program" };
byte far *insmenu[] = { "Path %39                                     ", "Type the location you wish to install Terminal Pacity to",
                        "NewO %39                                     ", "Erm..." };

void main(void)
{
   byte mywin, x;

   DGI_Setup();
   for (x = 0; x < 3; x++)
      mywin = DGI_CreateWindow(0, 0, 0, 0, WT_FOOTHEAD, 0, "new", "errr", 1, pacmenu, 0, 0);
   getch();
   DGI_Close();
   printf("%d\n", numslots);
}
