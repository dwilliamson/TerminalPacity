//#pragma off(unreferenced)

/*
Filename: PACMAN.C
Name:     Terminal PACity
Author:   Don Williamson

State  Meaning
--------------
  0    Falling
  1    Vertical Equilibrium
  2    Jumping

Copyright (c) 1997 Don Williamson
*/

/***** INCLUDES ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <bios.h>
#include <string.h>
#include <malloc.h>

#include "pacman.h"
#include "title.h"
#include "sound4.c"
#include "pacfunc.c"

#define GAME_END 8

/***** GLOBALS *************************************************************/

unsigned char *level_store, *monster_store, *liquid_bounds, *food_store, *anim_store;
unsigned char *monpos_store, *key_store, *temp_level, state = 0, awlives = 3;
unsigned int pscx = 0, pscy = 0, psbx = 0, psby = 0;
unsigned int pecx = 9, pecy = 9, pebx = 19, peby = 10;
unsigned int num_monsters = 0, screen_x, screen_y, num_objs = 0, num_foods = 0;
unsigned int num_mons = 0, num_keys = 0, clevel = 0, keys = 0;
unsigned int pac_pos = 0, fingame = 0;
long score = 0;
unsigned char scr_trk = 1;
char pcxfile[13] = "He He He", str_score[10], str_key[7], str_life[7], SB_Enable;
sprite t, e, r, m, i, n, a, l;
sprite pacman, ground, spikes, liquid, food, enemy, key_stuff;
int lives = 3;
float temp_y;
char str_level[3];
emsample splat, hitroof, hurt, ding, collect, keysnd, opendoor, finale;
unsigned char *level_names[9] = { "LEVEL1  .TPE",
                                  "LEVEL2  .TPE",
                                  "LEVEL3  .TPE",
                                  "LEVEL4  .TPE",
                                  "LEVEL5  .TPE",
                                  "LEVEL6  .TPE",
                                  "LEVEL7  .TPE",
                                  "LEVEL8  .TPE",
                                  "LEVEL9  .TPE" };
unsigned char *title_names[6] = { "New Game",
                                  "Options",
                                  "Credits",
                                  "High Scores",
                                  "Restore Game",
                                  "Exit" };
unsigned char *opscr_names[3] = { "Lives:",
                                  "Fire Anim:",
                                  "Sound:" };

byte DetectWindows(void);
byte WinMaj, WinMin, WinInst = 0;

/***** FUNCTIONS ***********************************************************/

#pragma aux DetectWindows = \
   "mov  ax, 1600h" \
   "int  2Fh" \
   "mov  WinInst, al" \
   "mov  WinMaj, al" \
   "mov  WinMin, ah" \
   "mov  bl, 0" \
   "cmp  al, 0h" \
   "je   exitag" \
   "mov  bl, 1" \
   "exitag:" \
   value [bl] \
   modify[ax bl];


void Get_Internal_PCX(int pal_ver)
{
   RGB_color color;

   unsigned char head[135], data;
   int count = 0, index;
   unsigned int width, height, ctr = 0;
   int num_bytes;

   for (index = 0; index < 128; index++)
   {
      head[index] = pcxstore[index];
   }
   ctr = 128;
   width  = (head[8]  + (head[9]  << 8)) + 1;
   height = (head[10] + (head[11] << 8)) + 1;
   inpcx = _fmalloc(width * height);
   while (count <= width * height)
   {
      data = pcxstore[ctr];
      ctr++;
      if (data >= 192)
      {
	 num_bytes = data - 192;
         data = pcxstore[ctr];
         ctr++;
	 while (num_bytes-->0)
	 {
            inpcx[count++] = data;
	 }
      }
      else
      {
         inpcx[count++] = data;
      }
   }
   ctr = 31851 - 768;
   if (pal_ver)
   {
      for (index = 0; index < 256; index ++)
      {
         color.red   = (pcxstore[ctr] >> 2);
         ctr++;
         color.green = (pcxstore[ctr] >> 2);
         ctr++;
         color.blue  = (pcxstore[ctr] >> 2);
         ctr++;
	 Set_Palette_Register(index, (RGB_color_ptr)&color);
      }
   }
   ctr = 31851 - 768;
   for (index = 0; index < 256; index++)
   {
      prev_pal[index * 3]       = pcxstore[ctr] >> 2; ctr++;
      prev_pal[(index * 3) + 1] = pcxstore[ctr] >> 2; ctr++;
      prev_pal[(index * 3) + 2] = pcxstore[ctr] >> 2; ctr++;
   }
}

void Get_Batch_PCX(char *batch_name, int pcx_number, int pal_ver)
{
   RGB_color color;

   FILE *fp;
   unsigned char head[135], data;
   unsigned char far *temp_pic;
   unsigned int count = 0, index;
   unsigned int width, height, ctr = 0, pcx_length;
   int num_bytes, x, num_pcx;
   unsigned long pcx_offset;

   fp = fopen(batch_name, "rb");
   fseek(fp, 5, SEEK_SET);
   num_pcx = fgetc(fp);
   if (pcx_number == 0)
   {
      pcx_offset = 6;
      fseek(fp, (((num_pcx - 0) * 4) * -1) + 0, SEEK_END);
      pcx_length = (((long)fgetc(fp) << 24) + ((long)fgetc(fp) << 16) + ((long)fgetc(fp) << 8) + (long)fgetc(fp)) - pcx_offset;
   }
   else
   {
       fseek(fp, (((num_pcx - 0) * 4) * -1) + ((pcx_number - 1) * 4), SEEK_END);
       pcx_offset = ((long)fgetc(fp) << 24) + ((long)fgetc(fp) << 16) + ((long)fgetc(fp) << 8) + (long)fgetc(fp);
       pcx_length = (((long)fgetc(fp) << 24) + ((long)fgetc(fp) << 16) + ((long)fgetc(fp) << 8) + (long)fgetc(fp)) - pcx_offset;
   }
   temp_pic = _fmalloc(pcx_length);
   fseek(fp, pcx_offset, SEEK_SET);
   for (x = 0; x < pcx_length; x++)
      temp_pic[x] = ~fgetc(fp);
   for (index = 0; index < 128; index++)
   {
      head[index] = temp_pic[index];
   }
   ctr = 128;
   width  = (head[8]  + (head[9]  << 8)) + 1;
   height = (head[10] + (head[11] << 8)) + 1;
   sprites = _fmalloc(width * height);
   while (count <= width * height)
   {
      data = temp_pic[ctr];
      ctr++;
      if (data >= 192)
      {
	 num_bytes = data - 192;
         data = temp_pic[ctr];
         ctr++;
	 while (num_bytes-->0)
	 {
            sprites[count++] = data;
	 }
      }
      else
      {
         sprites[count++] = data;
      }
   }
   ctr = pcx_length - 768;
   if (pal_ver)
   {
      for (index = 0; index < 256; index ++)
      {
         color.red   = (temp_pic[ctr] >> 2); ctr++;
         color.green = (temp_pic[ctr] >> 2); ctr++;
         color.blue  = (temp_pic[ctr] >> 2); ctr++;
	 Set_Palette_Register(index, (RGB_color_ptr)&color);
      }
   }
   ctr = pcx_length - 768;
   for (index = 0; index < 256; index++)
   {
      prev_pal[index * 3]       = temp_pic[ctr] >> 2; ctr++;
      prev_pal[(index * 3) + 1] = temp_pic[ctr] >> 2; ctr++;
      prev_pal[(index * 3) + 2] = temp_pic[ctr] >> 2; ctr++;
   }
   _ffree(temp_pic);
   fclose(fp);
}

void Fade_Out(void)
{
   RGB_color color;
   int ctr, x;

   for (ctr = 0; ctr < 63; ctr++)
   {
      for (x = 0; x < 256; x++)
      {
	 Get_Palette_Register(x, (RGB_color_ptr)&color);

         if (color.red   >= 1)
            color.red--;
         if (color.green >= 1)
            color.green--;
         if (color.blue  >= 1)
            color.blue--;
	 
	 Set_Palette_Register(x, (RGB_color_ptr)&color);
         delay(0.5);
      }
   }
}

void Fade_In(void)
{
   RGB_color color;
   int ctr, x;

   for (x = 0; x < 63; x++)
   {
      for (ctr = 0; ctr < 256; ctr++)
      {
         Get_Palette_Register(ctr, (RGB_color_ptr)&color);

         if (color.red < prev_pal[ctr * 3])
            color.red++;
         if (color.green < prev_pal[(ctr * 3) + 1])
            color.green++;
         if (color.blue < prev_pal[(ctr * 3) + 2])
            color.blue++;

         Set_Palette_Register(ctr, (RGB_color_ptr)&color);
      }
      delay(2);
   }
}

void Black_Palette(void)
{
   RGB_color color;
   int ctr;

   for (ctr = 0; ctr < 256; ctr++)
   {
      color.red   = 0;
      color.green = 0;
      color.blue  = 0;
      Set_Palette_Register(ctr, (RGB_color_ptr)&color);
   }
}

void Read_Palette(void)
{
   RGB_color color;
   int x;

   for (x = 0; x < 256; x++)
   {
      color.red   = palette[x * 3];
      color.green = palette[(x * 3) + 1];
      color.blue  = palette[(x * 3) + 2];
      Set_Palette_Register(x, (RGB_color_ptr)&color);
   }
}

void Fire_Demo(void)
{
   unsigned int x, y, a, tempx1, tempx2, tempy1, tempy2;

   for (y = 0; y < 100; y++)
   {
      for (x = 0; x < 80; x++)
         fire_buffer[x][y] = 0;
   }
   delay(1000);
   Blit_String(164, 19, 5, "Terminal Pacity", 1);
   Blit_String(165, 20, 4, "Terminal Pacity", 1);
   Blit_String(229, 29, 5, "by", 1);
   Blit_String(230, 30, 4, "by", 1);
   Blit_String(169, 39, 5, "Don Williamson", 1);
   Blit_String(170, 40, 4, "Don Williamson", 1);
   Blit_String(179, 59, 5, "Thanks to...", 1);
   Blit_String(180, 60, 4, "Thanks to...", 1);
   Blit_String(164, 79, 5, "Draeden", 1);
   Blit_String(165, 80, 4, "Draeden", 1);
   Blit_String(164, 89, 5, "Mark Feldman", 1);
   Blit_String(165, 90, 4, "Mark Feldman", 1);
   Blit_String(164, 99, 5, "Chris Land", 1);
   Blit_String(165, 100, 4, "Chris Land", 1);
   Blit_String(164, 109, 5, "Alex Libman", 1);
   Blit_String(165, 110, 4, "Alex Libman", 1);
   Blit_String(164, 119, 5, "Oleg Rekutin", 1);
   Blit_String(165, 120, 4, "Oleg Rekutin", 1);
   Blit_String(164, 129, 5, "Aaron Smith", 1);
   Blit_String(165, 130, 4, "Aaron Smith", 1);
   while (!kbhit())
   {
      vblank();
      for (x = 0; x < 80; x++)
      {
         for (y = 0; y < 75; y++)
         {
            fire_buffer[x][y] = 0;
         }
         fire_buffer[x][99] = 0;
         fire_buffer[x][98]  = 0;
      }
      for (x = 0; x < 60; x++)
      {
         a = rand()%80;
         fire_buffer[a][99] = 253;
         fire_buffer[a][98]  = 253;
      }
      for (y = 98; y != 0; y--)
      {
         for (x = 79; x != 0; x--)     
         {
            fire_buffer[x][y] = (fire_buffer[x][y + 1] + fire_buffer[x + 1][y + 1] + fire_buffer[x - 1][y + 1] + fire_buffer[x][y - 1]) >> 2;
            if (fire_buffer[x][y] > 1) fire_buffer[x][y]-=2;
         }
      }
      for (y = 100; y < 196; y+=2)
      {
         for (x = 0; x < 160; x+=2)
         {
            tempy1 = (y << 6) + (y << 8);
            tempy2 = ((y + 1) << 6) + ((y + 1) << 8);
            tempx1 = x;
            tempx2 = x + 1;
            if (sprites[tempy1 + tempx1] == 255)
               video_buffer[tempy1 + tempx1] = fire_buffer[x >> 1][y >> 1];
            if (sprites[tempy1 + tempx1] == 0)
               video_buffer[tempy1 + tempx1] = (fire_buffer[x >> 1][y >> 1]) >> 1;
            if (sprites[tempy1 + tempx2] == 255)
               video_buffer[tempy1 + tempx2] = fire_buffer[x >> 1][y >> 1];
            else video_buffer[tempy1 + tempx2] = (fire_buffer[x >> 1][y >> 1]) >> 1;
            if (sprites[tempy2 + tempx1] == 255)
               video_buffer[tempy2 + tempx1] = fire_buffer[x >> 1][y >> 1];
            else video_buffer[tempy2 + tempx1] = (fire_buffer[x >> 1][y >> 1]) >> 1;
            if (sprites[tempy2 + tempx2] == 255)
               video_buffer[tempy2 + tempx2] = fire_buffer[x >> 1][y >> 1];
            else video_buffer[tempy2 + tempx2] = (fire_buffer[x >> 1][y >> 1]) >> 1;
         }
      }
   }
   getch();
}

void End_Sequence(void)
{
   delay(1000);
   Fade_Out();
   Black_Palette();
   Get_Batch_PCX("PACMAN.PPE", 8, 0);
   _fmemcpy(video_buffer, sprites, 64000);
   _ffree(sprites);
   Fade_In();
   delay(2000);
   Fade_Out();
   Get_Batch_PCX("PACMAN.PPE", 9, 0);
   _fmemcpy(video_buffer, sprites, 64000);
   _ffree(sprites);
   Fade_In();
   delay(2000);
   Fade_Out();
   Get_Batch_PCX("PACMAN.PPE", 10, 0);
   _fmemcpy(video_buffer, sprites, 64000);
   _ffree(sprites);
   Fade_In();
   delay(2000);
   Fade_Out();
   Get_Batch_PCX("PACMAN.PPE", 7, 0);
   _fmemcpy(video_buffer, sprites, 64000);
   _ffree(sprites);
   Fade_In();
   delay(4000);
   Fade_Out();
   _fmemset(video_buffer, 0, 64000);
   Get_Batch_PCX("PACMAN.PPE", 6, 0);
   Read_Palette();
   Delete_Keyboard();
   Fire_Demo();
   _ffree(sprites);
   High_Score(1);
   Install_Keyboard();
}

void Display_Pac(int x, int y)
{
   int mx, my;

   for (my = y; my < (y + 11); my++)
   {
      for (mx = x; mx < (x + 11); mx++)
      {
         video_buffer[(my << 6) + (my << 8) + mx] = title_col_table[pac_title[((my - y) * 11) + (mx - x)]];
      }
   }
}

void Remove_Pac(int x, int y)
{
   int mx, my;

   for (my = y; my < (y + 11); my++)
   {
      for (mx = x; mx < (x + 11); mx++)
      {
         video_buffer[(my << 6) + (my << 8) + mx] = 0;
      }
   }
}

void Intermittance(void)
{
   int x, y;

   _fmemset(video_buffer, 0, 56640);
   sprintf(str_level, "%i...", clevel + 1);
   Get_Batch_PCX("PACMAN.PPE", 5, 0);
   for (y = 0; y < 56; y++)
   {
      for (x = 0; x < 54; x++)
      {
         video_buffer[pixtab[y + 112] + x + 133] = sprites[(y * 54) + x];
      }
   }
   _ffree(sprites);
   Blit_String(60, 36, 2, "Now Entering Level", 0);
   Blit_String(212, 36, 2, str_level, 0);
   if (SB_Enable)
   {
      SBPlayEMS((emsample_ptr)&finale, 39420);
   }
   delay(3000);
}

void Draw_Inventory(void)
{
   int x, y, tempx, tempy, tempc;

   for (y = 179; y < 198; y++)
   {
      for (x = 1; x < 319; x++)
      {
         video_buffer[(y << 6) + (y << 8) + x] = 155;
      }
   }
   Bline(0, 178, 319, 178, 193);
   Bline(0, 178, 0, 198, 193);
   Bline(0, 198, 319, 198, 94);
   Bline(319, 178, 319, 198, 94);
   Plot_Pixel_Fast(319, 178, 155);
   Plot_Pixel_Fast(0, 198, 155);
   for (y = 182; y < 194; y++)
   {
      for (x = 18; x < 64; x++)
      {
         video_buffer[(y << 6) + (y << 8) + x] = 0;
      }
   }
   for (x = 257; x < 274; x++)
   {
      for (y = 181; y < 196; y++)
      {
         video_buffer[(y << 8) + (y << 6) + x] = 0;
      }
   }
   for (x = 167; x < 184; x++)
   {
      for (y = 181; y < 196; y++)
      {
         video_buffer[(y << 8) + (y << 6) + x] = 0;
      }
   }
   Blit_String(20, 185, 2, "Score", 1);
   Bline(17, 182, 63, 182, 94);
   Bline(17, 182, 17, 193, 94);
   Bline(18, 194, 64, 194, 193);
   Bline(64, 194, 64, 183, 193);
   Bline(256, 180, 273, 180, 94);
   Bline(256, 180, 256, 195, 94);
   Bline(257, 196, 274, 196, 193);
   Bline(274, 196, 274, 181, 193);
   Bline(166, 180, 183, 180, 94);
   Bline(166, 180, 166, 195, 94);
   Bline(167, 196, 184, 196, 193);
   Bline(184, 196, 184, 181, 193);
   Bline(232, 198, 232, 178, 94);
   Bline(233, 198, 233, 178, 193);
   Plot_Pixel_Fast(232, 178, 155);
   Plot_Pixel_Fast(233, 198, 155);
   Bline(147, 198, 147, 178, 94);
   Bline(148, 198, 148, 178, 193);
   Plot_Pixel_Fast(147, 178, 155);
   Plot_Pixel_Fast(148, 198, 155);
   tempx = pacman.x;
   tempy = pacman.y;
   tempc = pacman.curr_frame;
   pacman.x = 257;
   pacman.y = 181;
   pacman.curr_frame = 0;
   key_stuff.x = 167;
   key_stuff.y = 181;
   key_stuff.curr_frame = 0;
   Draw_Sprite((sprite_ptr)&pacman);
   Draw_Sprite((sprite_ptr)&key_stuff);
   pacman.x = tempx;
   pacman.y = tempy;
   pacman.curr_frame = tempc;
}

void Kill_Sprite(void)
{
   int x, ctr;
   RGB_color color;
   unsigned char palstore[768];

   vblank();
   Draw_Sprite((sprite_ptr)&pacman);
   for (x = 0; x < 256; x++)
   {
      Get_Palette_Register(x, (RGB_color_ptr)&color);
      palstore[(x << 1) + x]       = color.red;
      palstore[((x << 1) + x) + 1] = color.green;
      palstore[((x << 1) + x) + 2] = color.blue;
   }
   for (ctr = 0; ctr < 63; ctr++)
   {
      for (x = 0; x < 256; x++)
      {
	 Get_Palette_Register(x, (RGB_color_ptr)&color);

	 if (color.red   <= 62)
	    color.red++;
         if (color.green >= 1)
            color.green--;
         if (color.blue  >= 1)
            color.blue--;
	 
	 Set_Palette_Register(x, (RGB_color_ptr)&color);
      }
   }
   vblank();
   for (ctr = 0; ctr < 63; ctr++)
   {
      for (x = 0; x < 256; x++)
      {
	 Get_Palette_Register(x, (RGB_color_ptr)&color);

         if (color.red   >= 1)
            color.red--;
	 
	 Set_Palette_Register(x, (RGB_color_ptr)&color);
      }
   }
   for (x = 0; x < 256; x++)
   {
      color.red   = 0;
      color.green = 0;
      color.blue  = 0;
      Set_Palette_Register(x, (RGB_color_ptr)&color);
   }
   _fmemset(video_buffer, 0, 64000);
   _fmemcpy(level_store, temp_level, 22000);
   lives--;
   if (lives == -1)
   {
      Title_Screen();
      goto FuncEnd;
   }
   screen_x = pscx;
   screen_y = pscy;
   pacman.x = psbx << 4;
   pacman.y = psby << 4;
   temp_y = pacman.y;
   keys = 0;
   Draw_Screen(screen_x, screen_y);
   Draw_Sprite((sprite_ptr)&pacman);
   Screen_Logic();
   vblank();
   for (ctr = 0; ctr < 63; ctr++)
   {
      for (x = 0; x < 256; x++)
      {
	 Get_Palette_Register(x, (RGB_color_ptr)&color);

         if (color.red   != palstore[x * 3])
	    color.red++;
         if (color.green != palstore[(x * 3) + 1])
            color.green++;
         if (color.blue  != palstore[(x * 3) + 2])
            color.blue++;
	 
	 Set_Palette_Register(x, (RGB_color_ptr)&color);
      }
   }
   FuncEnd:;
}

void Move_Enemies(void)
{
   int x, mod, mx, cx, dx, dy, cy;
   unsigned int offset;

   for (x = 0; x < num_mons; x++)
   {
      mod = (x << 3) + x;
      if (monpos_store[mod] == 0)
      {
         Power_Erase_16((monpos_store[mod + 1] << 8) + monpos_store[mod + 2], monpos_store[mod + 3]);
         monpos_store[mod + 3] -= alpha_table[alpha];
      }
      if (monpos_store[mod] == 1)
      {
         mx = (monpos_store[mod + 1] << 8) + monpos_store[mod + 2];
         cx = (monpos_store[mod + 4] << 8) + monpos_store[mod + 5];
         dx = (monpos_store[mod + 6] << 8) + monpos_store[mod + 7];
         Power_Erase_16(mx, monpos_store[mod + 3]);
         if (monpos_store[mod + 8] == 1)
            mx++;
         else
            mx--;
         if (mx >= dx)
            monpos_store[mod + 8] = 0;
         if (mx <= cx)
            monpos_store[mod + 8] = 1;
         monpos_store[mod + 1] = (mx & 65280) >> 8;
         monpos_store[mod + 2] = mx & 255;
      }
      if (monpos_store[mod] == 3)
      {
         mx = (monpos_store[mod + 1] << 8) + monpos_store[mod + 2];
         dx = pacman.x - mx;
         dy = pacman.y - monpos_store[mod + 3];
         cx = abs(dx);
         cy = abs(dy);
         if (dx > 0 && cx > cy)
         {
//            offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx + 16;
            offset = pixtab[monpos_store[mod + 3] + 40] + mx + 16;
            if (!video_buffer[offset] && !video_buffer[offset + 4800])
            {
               mx++;
               monpos_store[mod + 1] = (mx & 65280) >> 8;
               monpos_store[mod + 2] = mx & 255;
            }
         }
         if (dx < 0 && cx > cy)
         {
//            offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx - 1;
            offset = pixtab[monpos_store[mod + 3] + 40] + mx - 1;
            if (!video_buffer[offset] && !video_buffer[offset + 4800])
            {
               mx--;
               monpos_store[mod + 1] = (mx & 65280) >> 8;
               monpos_store[mod + 2] = mx & 255;
            }
         }
         if (dy > 0 && cy > cx)
         {
//            offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx + 5120;
            offset = pixtab[monpos_store[mod + 3] + 40] + mx + 5120;
            if (!video_buffer[offset] && !video_buffer[offset + 15])
               monpos_store[mod + 3]++;
         }
         if (dy < 0 && cy > cx)
         {
//            offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx - 320;
            offset = pixtab[monpos_store[mod + 3] + 40] + mx - 320;
            if (!video_buffer[offset] && !video_buffer[offset + 15])
               monpos_store[mod + 3]--;
         }
         if (cx == cy)
         {
            if (dy > 0 && dx > 0)
            {
//               offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx + 5136;
               offset = pixtab[monpos_store[mod + 3] + 40] + mx + 5136;
               if (!video_buffer[offset] && !video_buffer[offset - 4800] && !video_buffer[offset - 15])
               {
                  mx++;
                  monpos_store[mod + 3]++;
                  monpos_store[mod + 1] = (mx & 65280) >> 8;
                  monpos_store[mod + 2] = mx & 255;
               }
            }
            if (dy > 0 && dx < 0)
            {
//               offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx + 5119;
               offset = pixtab[monpos_store[mod + 3] + 40] + mx + 5119;
               if (!video_buffer[offset] && !video_buffer[offset - 4800] && !video_buffer[offset + 15])
               {
                  mx--;
                  monpos_store[mod + 3]++;
                  monpos_store[mod + 1] = (mx & 65280) >> 8;
                  monpos_store[mod + 2] = mx & 255;
               }
            }
            if (dy < 0 && dx > 0)
            {
//               offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx - 304;
               offset = pixtab[monpos_store[mod + 3] + 40] + mx - 304;
               if (!video_buffer[offset] && !video_buffer[offset - 15] && !video_buffer[offset + 4800])
               {
                  mx++;
                  monpos_store[mod + 3]--;
                  monpos_store[mod + 1] = (mx & 65280) >> 8;
                  monpos_store[mod + 2] = mx & 255;
               }
            }
            if (dy < 0 && dx < 0)
            {
//               offset = (monpos_store[mod + 3] << 6) + (monpos_store[mod + 3] << 8) + mx - 321;
               offset = pixtab[monpos_store[mod + 3] + 40] + mx - 321;
               if (!video_buffer[offset] && !video_buffer[offset + 15] && !video_buffer[offset + 4800])
               {
                  mx--;
                  monpos_store[mod + 3]--;
                  monpos_store[mod + 1] = (mx & 65280) >> 8;
                  monpos_store[mod + 2] = mx & 255;
               }
            }
         }
      }
                 
   }
   alpha++;
   if (alpha == 80)
      alpha = 0;
}

void Wipe_Enemies(void)
{
   int x, mod;

   for (x = 0; x < num_mons; x++)
   {
      mod = (x << 3) + x;
      Power_Erase_16((monpos_store[mod + 1] << 8) + monpos_store[mod + 2], monpos_store[mod + 3]);
   }
}

void Update_Enemies(void)
{
   int x, mod;

   for (x = 0; x < num_mons; x++)
   {
      mod = (x << 3) + x;
      enemy.curr_frame = ((monpos_store[mod] << 1) + monpos_store[mod]) + (sprite_ctr / 6);
      enemy.x = (monpos_store[mod + 1] << 8) + monpos_store[mod + 2];
      enemy.y = monpos_store[mod + 3];
      Draw_Sprite((sprite_ptr)&enemy);
   }
   if (fire_anim)
   {
      for (x = 0; x < fire_track; x+=2)
      {
         Draw_Sprite_Quick((sprite_ptr)&liquid, anim_store[x] << 4, anim_store[x + 1] << 4, anim_pos);
      }
   }
}

void Update_Score(void)
{
   int mx, my;

   if (score >= ((long)scr_trk * 1000) && score < 64000)
   {
      scr_trk++;
      lives++;
      if (SB_Enable)
      {
         SBPlayEMS((emsample_ptr)&ding, ding.length);
         spi[0] = 0;
         spi[1] = 1;
      }
   }
   for (my = 183; my < 193; my++)
   {
      for (mx = 68; mx < 130; mx++)
      {
         video_buffer[(my << 6) + (my << 8) + mx] = 0;
      }
   }
   for (my = 183; my < 193; my++)
   {
      for (mx = 188; mx < 212; mx++)
      {
         video_buffer[(my << 6) + (my << 8) + mx] = 0;
         video_buffer[(my << 6) + (my << 8) + mx + 90] = 0;
      }
   }
   sprintf(str_score, "%lu", score);
   sprintf(str_key, "%u", keys);
   sprintf(str_life, "%i", lives);
   Blit_String(70, 185, 2, str_score, 1);
   Blit_String(190, 185, 2, str_key, 1);
   Blit_String(280, 185, 2, str_life, 1);
   Bline(67, 183, 130, 183, 94);
   Bline(67, 183, 67, 193, 94);
   Bline(67, 193, 130, 193, 193);
   Bline(130, 193, 130, 183, 193);
   Plot_Pixel_Fast(67, 193, 155);
   Plot_Pixel_Fast(130, 183, 155);
   Bline(187, 183, 212, 183, 94);
   Bline(187, 183, 187, 193, 94);
   Bline(187, 193, 212, 193, 193);
   Bline(212, 193, 212, 183, 193);
   Plot_Pixel_Fast(212, 183, 155);
   Plot_Pixel_Fast(187, 193, 155);

   Bline(277, 183, 302, 183, 94);
   Bline(277, 183, 277, 193, 94);
   Bline(277, 193, 302, 193, 193);
   Bline(302, 193, 302, 183, 193);
   Plot_Pixel_Fast(302, 183, 155);
   Plot_Pixel_Fast(277, 193, 155);
}

void Power_Erase_16(int x, int y)
{
   int mx, my;

   for (my = y; my < y + 16; my++)
   {
      for (mx = x; mx < x + 16; mx++)
      {
         video_buffer[pixtab[my + 40] + mx] = 0;
      }
   }
}

char Check_Collision(void)
{
   int x, bx, by, lx, ly, dx, y, offset, temp, dead = 0;

   if (screen_x == pecx && screen_y == pecy)
   {
      bx = pebx << 4;
      by = peby << 4;
      lx = bx + 15;
      ly = by + 15;
      if (pacman.x >= bx && pacman.y >= by && pacman.x < lx && pacman.y < ly)
      {
         clevel++;
         if (clevel < GAME_END)
         {
            Intermittance();
            _fmemset(video_buffer, 0, 64000);
            Load_Level(level_names[clevel]);
            dead = 1;
            goto KillA;
         }
         else
         {
            dead    = 1;
            fingame = 1;
            End_Sequence();
            goto KillA;
         }
      }
      if (pacman.x >= bx && (pacman.y + 15) >= by && pacman.x < lx && (pacman.y + 15) < ly)
      {
         clevel++;
         if (clevel < GAME_END)
         {
            Intermittance();
            _fmemset(video_buffer, 0, 64000);
            Load_Level(level_names[clevel]);
            dead = 1;
            goto KillA;
         }
         else
         {
            dead    = 1;
            fingame = 1;
            End_Sequence();
            goto KillA;
         }
      }
      if ((pacman.x + 15) >= bx && pacman.y >= by && (pacman.x + 15) < lx && pacman.y < ly)
      {
         clevel++;
         if (clevel < GAME_END)
         {
            Intermittance();
            _fmemset(video_buffer, 0, 64000);
            Load_Level(level_names[clevel]);
            dead = 1;
            goto KillA;
         }
         else
         {
            dead    = 1;
            fingame = 1;
            End_Sequence();
            goto KillA;
         }
      }
      if ((pacman.x + 15) >= bx && (pacman.y + 15) >= by && (pacman.x + 15) < lx && (pacman.y + 15) < ly)
      {
         clevel++;
         if (clevel < GAME_END)
         {
            Intermittance();
            _fmemset(video_buffer, 0, 64000);
            Load_Level(level_names[clevel]);
            dead = 1;
            goto KillA;
         }
         else
         {
            dead    = 1;
            fingame = 1;
            End_Sequence();
            goto KillA;
         }
      }
   }
   for (x = 0; x < num_objs; x++)
   {
      y = (x << 1) + (x << 2);
      bx = (liquid_bounds[y] << 8) + liquid_bounds[y + 1];
      by = liquid_bounds[y + 2];
      lx = (liquid_bounds[y + 3] << 8) + liquid_bounds[y + 4];
      ly = liquid_bounds[y + 5];
      if (pacman.x >= bx && pacman.y >= by && pacman.x < lx && pacman.y < ly)
      {
         offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
         temp = offset + (by + (by >> 2) + (bx >> 4));
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&splat, splat.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if (pacman.x >= bx && (pacman.y + 15) >= by && pacman.x < lx && (pacman.y + 15) < ly)
      {
         offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
         temp = offset + (by + (by >> 2) + (bx >> 4));
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&splat, splat.length);
         Kill_Sprite();         
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 15) >= bx && pacman.y >= by && (pacman.x + 15) < lx && pacman.y < ly)
      {
         offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
         temp = offset + (by + (by >> 2) + (bx >> 4));
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&splat, splat.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 15) >= bx && (pacman.y + 15) >= by && (pacman.x + 15) < lx && (pacman.y + 15) < ly)
      {
         offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
         temp = offset + (by + (by >> 2) + (bx >> 4));
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&splat, splat.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
   }
   for (x = 0; x < num_mons; x++)
   {
      y = (x << 3) + x;
      bx = ((monpos_store[y + 1] << 8) + monpos_store[y + 2]) + 2;
      by = monpos_store[y + 3] + 2;
      lx = bx + 11;
      ly = by + 11;
      if (pacman.x >= bx && pacman.y >= by && pacman.x < lx && pacman.y < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if (pacman.x >= bx && (pacman.y + 15) >= by && pacman.x < lx && (pacman.y + 15) < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 15) >= bx && pacman.y >= by && (pacman.x + 15) < lx && pacman.y < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 15) >= bx && (pacman.y + 15) >= by && (pacman.x + 15) < lx && (pacman.y + 15) < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 15) >= bx && (pacman.y + 7) >= by && (pacman.x + 15) < lx && (pacman.y + 7) < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 7) >= bx && (pacman.y + 15) >= by && (pacman.x + 7) < lx && (pacman.y + 15) < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if (pacman.x >= bx && (pacman.y + 7) >= by && pacman.x < lx && (pacman.y + 7) < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
      if ((pacman.x + 7) >= bx && pacman.y >= by && (pacman.x + 7) < lx && pacman.y < ly)
      {
         if (SB_Enable)
            SBPlayEMS((emsample_ptr)&hurt, hurt.length);
         Kill_Sprite();
         dead = 1;
         goto KillA;
      }
   }
   for (x = 0; x < num_foods; x++)
   {
      y = (x << 2) + x;
      bx = (food_store[y + 1] << 8) + food_store[y + 2];
      by = food_store[y + 3];
      lx = bx + 15;
      ly = by + 15;
      dx = food_store[y];
      if (food_store[y + 4])
      {
         if (pacman.x >= bx && pacman.y >= by && pacman.x < lx && pacman.y < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            food_store[y + 4] = 0;
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            score += (dx * dx);
            if (SB_Enable && !spi[1])
            {
               SBPlayEMS((emsample_ptr)&collect, collect.length);
               spi[0] = 1;
            }
            Update_Score();
         }
         if (pacman.x >= bx && (pacman.y + 15) >= by && pacman.x < lx && (pacman.y + 15) < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            food_store[y + 4] = 0;
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            score += (dx * dx);
            if (SB_Enable && !spi[1])
            {
               SBPlayEMS((emsample_ptr)&collect, collect.length);
               spi[0] = 1;
            }
            Update_Score();
         }
         if ((pacman.x + 15) >= bx && pacman.y >= by && (pacman.x + 15) < lx && pacman.y < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            food_store[y + 4] = 0;
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            score += (dx * dx);
            if (SB_Enable && !spi[1])
            {
               SBPlayEMS((emsample_ptr)&collect, collect.length);
               spi[0] = 1;
            }
            Update_Score();
         }
         if ((pacman.x + 15) >= bx && (pacman.y + 15) >= by && (pacman.x + 15) < lx && (pacman.y + 15) < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            food_store[y + 4] = 0;
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            score += (dx * dx);
            if (SB_Enable && !spi[1])
            {
               SBPlayEMS((emsample_ptr)&collect, collect.length);
               spi[0] = 1;
            }
            Update_Score();
         }
      }
   }
   for (x = 0; x < num_keys; x++)
   {
      y = x << 2;
      bx = (key_store[y] << 8) + key_store[y + 1];
      by = key_store[y + 2];
      lx = bx + 15;
      ly = by + 15;
      if (key_store[y + 3])
      {
         if (pacman.x >= bx && pacman.y >= by && pacman.x < lx && pacman.y < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            keys++;
            key_store[y + 3] = 0;
            if (SB_Enable)
               SBPlayEMS((emsample_ptr)&keysnd, keysnd.length);
            Update_Score();
         }
         if (pacman.x >= bx && (pacman.y + 15) >= by && pacman.x < lx && (pacman.y + 15) < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            keys++;
            key_store[y + 3] = 0;
            if (SB_Enable)
               SBPlayEMS((emsample_ptr)&keysnd, keysnd.length);
            Update_Score();
         }
         if ((pacman.x + 15) >= bx && pacman.y >= by && (pacman.x + 15) < lx && pacman.y < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            keys++;
            key_store[y + 3] = 0;
            if (SB_Enable)
               SBPlayEMS((emsample_ptr)&keysnd, keysnd.length);
            Update_Score();
         }
         if ((pacman.x + 15) >= bx && (pacman.y + 15) >= by && (pacman.x + 15) < lx && (pacman.y + 15) < ly)
         {
            Power_Erase_16(bx, by);
            Draw_Sprite((sprite_ptr)&pacman);
            offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
            temp = offset + (by + (by >> 2) + (bx >> 4));
            level_store[temp] = 63;
            keys++;
            key_store[y + 3] = 0;
            if (SB_Enable)
               SBPlayEMS((emsample_ptr)&keysnd, keysnd.length);
            Update_Score();
         }
      }
   }
   bx = pacman.x % 16;
   by = pacman.y % 16;
   bx = pacman.x - bx;
   by = pacman.y - by;
   offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
   temp = offset + (by + (by >> 2) + (bx >> 4));
   if (level_store[temp + 1] == 53 && pacman.curr_frame < 8 && keys > 0 && level_store[(temp + 1) - 20] == 54)
   {
      lx = bx + 16;
      ly = by - 16;
      lx = lx % 16;
      ly = ly % 16;
      lx = (bx + 16) - lx;
      ly = (by - 16) - ly;
      Power_Erase_16(lx, ly + 16);
      Power_Erase_16(lx, ly);
      keys--;
      level_store[temp + 1] = 63;
      level_store[(temp + 1) - 20] = 63;
      Update_Score();
      if (SB_Enable)
         SBPlayEMS((emsample_ptr)&opendoor, opendoor.length);
   }
   if (level_store[temp - 1] == 53 && pacman.curr_frame > 7 && keys > 0 && level_store[(temp - 1) - 20] == 54)
   {
      lx = bx - 16;
      ly = by - 16;
      lx = lx % 16;
      ly = ly % 16;
      lx = (bx - 16) - lx;
      ly = (by - 16) - ly;
      Power_Erase_16(lx, ly + 16);
      Power_Erase_16(lx, ly);
      keys--;
      level_store[temp - 1] = 63;
      level_store[(temp - 1) - 20] = 63;
      Update_Score();
      if (SB_Enable)
         SBPlayEMS((emsample_ptr)&opendoor, opendoor.length);
   }
   KillA:;
   return(dead);
}

void Screen_Logic(void)
{
   unsigned int x, y, offset, temp, bx = 0, by = 0, lx = 0, ly = 0, ctr, tctr = 0, multemp;
   unsigned int offtemp, mx;

   offset = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
   num_objs   = 0;
   num_foods  = 0;
   num_mons   = 0;
   num_keys   = 0;
   fire_track = 0;
   for (y = 0; y < 11; y++)
   {
      for (x = 0; x < 20; x++)
      {
         temp = offset + (((y << 4) + (y << 2)) + x);         
         if (level_store[temp] > 54 && level_store[temp] < 61)
         {
            bx = x << 4;
            by = (y << 4) + 3;
            lx = ((x + 1) << 4) - 1;
            ly = ((y + 1) << 4) - 1;
            ctr = 1;
            if (level_store[temp] == 60)
            {
               anim_store[fire_track]     = x;
               anim_store[fire_track + 1] = y;
               fire_track+=2;
            }
            while ((level_store[temp + ctr] > 54 && level_store[temp + ctr] < 61) && (x + ctr) < 20)
            {
               if (level_store[temp + ctr] == 60)
               {
                  anim_store[fire_track]     = (lx >> 4) + 1;
                  anim_store[fire_track + 1] = y;
                  fire_track+=2;
               }
               lx += 16;
               ctr++;
            }
            if (level_store[temp + ctr - 1] == 60)
            {
               anim_store[fire_track]     = lx >> 4;
               anim_store[fire_track + 1] = y;
               fire_track+=2;
            }
            multemp = (tctr << 1) + (tctr << 2);
            liquid_bounds[multemp]       = (bx & 65280) >> 8;
            liquid_bounds[(multemp) + 1] = bx & 255;
            liquid_bounds[(multemp) + 2] = by;
            liquid_bounds[(multemp) + 3] = (lx & 65280) >> 8;
            liquid_bounds[(multemp) + 4] = lx & 255;
            liquid_bounds[(multemp) + 5] = ly;
            tctr++;
            x += ctr;
            num_objs++;
         }
         if (level_store[temp] >= 47 && level_store[temp] < 52)
         {
            multemp = (num_foods << 2) + num_foods;
            food_store[multemp]     = level_store[temp] - 46;
            food_store[multemp + 1] = ((x << 4) & 65280) >> 8;
            food_store[multemp + 2] = (x << 4) & 255;
            food_store[multemp + 3] = y << 4;
            food_store[multemp + 4] = 1;
            num_foods++;
         }
         if (level_store[temp] == 52)
         {
            multemp = num_keys << 2;
            key_store[multemp]     = ((x << 4) & 65280) >> 8;
            key_store[multemp + 1] = (x << 4) & 255;
            key_store[multemp + 2] = y << 4;
            key_store[multemp + 3] = 1;
            num_keys++;
         }
         if (level_store[temp] >= 64 && level_store[temp] < 68)
         {
            multemp = (num_mons << 3) + num_mons;
            monpos_store[multemp]     = level_store[temp] - 64;
            monpos_store[multemp + 1] = ((x << 4) & 65280) >> 8;
            monpos_store[multemp + 2] = (x << 4) & 255;
            monpos_store[multemp + 3] = y << 4;
            if (level_store[temp] == 65)
            {
               for (mx = 0; mx < num_monsters << 2; mx += 4)
               {
                  offtemp = monster_store[mx];
                  offtemp += (monster_store[mx + 1] << 8);
                  if (offtemp == temp)
                  {
                     monpos_store[multemp + 4] = (((x << 4) - (monster_store[mx + 2] << 4)) & 65280) >> 8;
                     monpos_store[multemp + 5] = ((x << 4) - (monster_store[mx + 2] << 4)) & 255;
                     monpos_store[multemp + 6] = (((x << 4) + (monster_store[mx + 3] << 4)) & 65280) >> 8;
                     monpos_store[multemp + 7] = ((x << 4) + (monster_store[mx + 3] << 4)) & 255;
                  }
               }
            }
            else
            {
               monpos_store[multemp + 4] = 0;
               monpos_store[multemp + 5] = 0;
               monpos_store[multemp + 6] = 0;
               monpos_store[multemp + 7] = 0;
            }
            monpos_store[multemp + 8] = 1;
            num_mons++;
         }
      }
   }
}

void Title_Screen(void)
{
   Delete_Keyboard();
   _dos_setvect(0x1C, Old_Time_Isr);
   Change_Timer(0xFFFF);

   _ffree(level_store);
   _ffree(monster_store);
   _ffree(liquid_bounds);
   _ffree(food_store);
   _ffree(monpos_store);
   _ffree(key_store);
   _ffree(temp_level);
   _ffree(anim_store);
   if (lives == -1)
   {
      High_Score(1);
      lives = awlives;
   }
   Display_Menu();
   Initialise_Globals();
   Install_Keyboard();
   Old_Time_Isr = _dos_getvect(0x1C);
   _dos_setvect(0x1C, Timer);
   Change_Timer(0x4DAE);
   _fmemset(video_buffer, 0, 64000);
   num_monsters = num_keys = clevel = 0;
   scr_trk = 1;
   score = keys = 0;
   lives = awlives;
   Load_Level(level_names[clevel]);
}

void Leave(void)
{
   Delete_Keyboard();
   _dos_setvect(0x1C, Old_Time_Isr);
   Change_Timer(0xFFFF);
   Set_Video_Mode(0x03);
   printf("--------------------------------------------------------------------------------");
   printf("              Terminal Pacity - Copyright (c) 1997 Don Williamson\n");
   printf("--------------------------------------------------------------------------------");
   printf("\nMemory deallocating...");
   printf("\nReleasing Keyboard...");
   printf("\nResetting Internal Timer...");
   Free_Memory();
   if (SB_Reset) Release_Blaster();
   printf("\nDone.\n");
   exit(1);
}

void Initialise_Globals(void)
{
   level_store    = _fmalloc(22000);
   monster_store  = _fmalloc(20000);
   liquid_bounds  = _fmalloc(2000);
   food_store     = _fmalloc(2000);
   monpos_store   = _fmalloc(4000);
   key_store      = _fmalloc(2000);
   temp_level     = _fmalloc(22000);
   anim_store     = _fmalloc(440);
   screen_x = 0;
   screen_y = 0;
}

void Free_Memory(void)
{
   _ffree(level_store);
   _ffree(monster_store);
   _ffree(liquid_bounds);
   _ffree(food_store);
   _ffree(monpos_store);
   _ffree(key_store);
   _ffree(temp_level);
   _ffree(anim_store);
   Sprite_Delete((sprite_ptr)&pacman);
   Sprite_Delete((sprite_ptr)&ground);
   Sprite_Delete((sprite_ptr)&spikes);
   Sprite_Delete((sprite_ptr)&liquid);
   Sprite_Delete((sprite_ptr)&food);
   Sprite_Delete((sprite_ptr)&enemy);
   Sprite_Delete((sprite_ptr)&key_stuff);
   if (SB_Reset)
   {
//      _ffree(splat.data);
//      _ffree(hitroof.data);
//      _ffree(hurt.data);
//      _ffree(ding.data);
//      _ffree(collect.data);
//      _ffree(keysnd.data);
      EMM_Free(splat.handle);
      EMM_Free(hitroof.handle);
      EMM_Free(hurt.handle);
      EMM_Free(ding.handle);
      EMM_Free(collect.handle);
      EMM_Free(keysnd.handle);
      EMM_Free(opendoor.handle);
      EMM_Free(finale.handle);
   }
}

void Report_Error(char *message)
{
   Delete_Keyboard();
   _dos_setvect(0x1C, Old_Time_Isr);
   Change_Timer(0xFFFF);
   Free_Memory();
   Set_Video_Mode(0x03);
   printf("%s", message);
   exit(1);
}

void Initialise_Sprites(void)
{
   Sprite_Init((sprite_ptr)&pacman, 100, 100, 0, 0, 0, 0, 16, 16, 16);
   Sprite_Init((sprite_ptr)&ground, 100, 100, 0, 0, 0, 0, 16, 16, 63);
   Sprite_Init((sprite_ptr)&spikes, 100, 100, 0, 0, 0, 0, 16, 16, 4);
   Sprite_Init((sprite_ptr)&liquid, 100, 100, 0, 0, 0, 0, 16, 16, 7);
   Sprite_Init((sprite_ptr)&food, 100, 100, 0, 0, 0, 0, 16, 16, 5);
   Sprite_Init((sprite_ptr)&enemy, 100, 100, 0, 0, 0, 0, 16, 16, 13);
   Sprite_Init((sprite_ptr)&key_stuff, 100, 100, 0, 0, 0, 0, 16, 16, 3);
}

void Get_Sprite_Info(void)
{
   int x;

   for (x = 0; x < 16; x++)
      Grab_Bitmap((sprite_ptr)&pacman, x, x, 3);
   for (x = 0; x < 18; x++)
      Grab_Bitmap((sprite_ptr)&ground, x, x, 0);
   for (x = 0; x < 18; x++)
      Grab_Bitmap((sprite_ptr)&ground, x + 18, x, 1);
   for (x = 0; x < 11; x++)
      Grab_Bitmap((sprite_ptr)&ground, x + 36, x, 2);
   Grab_Bitmap((sprite_ptr)&ground, 63, 12, 2);
   Grab_Bitmap((sprite_ptr)&liquid, 0, 12, 4);
   Grab_Bitmap((sprite_ptr)&liquid, 1, 13, 4);

   Grab_Bitmap((sprite_ptr)&liquid, 2, 12, 7); // Animated Fire
   Grab_Bitmap((sprite_ptr)&liquid, 3, 13, 7);
   Grab_Bitmap((sprite_ptr)&liquid, 4, 14, 7);
   Grab_Bitmap((sprite_ptr)&liquid, 5, 15, 7);
   Grab_Bitmap((sprite_ptr)&liquid, 6, 16, 7);

   Grab_Bitmap((sprite_ptr)&spikes, 0, 8, 4);
   Grab_Bitmap((sprite_ptr)&spikes, 1, 9, 4);
   Grab_Bitmap((sprite_ptr)&spikes, 2, 10, 4);
   Grab_Bitmap((sprite_ptr)&spikes, 3, 11, 4);
   for (x = 0; x < 5; x++)
      Grab_Bitmap((sprite_ptr)&food, x, x, 4);
   for (x = 0; x < 12; x++)
      Grab_Bitmap((sprite_ptr)&enemy, x, x, 7);
   for (x = 5; x < 8; x++)
      Grab_Bitmap((sprite_ptr)&key_stuff, x - 5, x, 4);
   Grab_Bitmap((sprite_ptr)&enemy, 12, 11, 2);
}

void Draw_Screen(int scrnx, int scrny)
{
   int x, y, offset, px, temp;

   offset = (((screen_y << 3) + (screen_y << 1)) + scrnx) * 220;
   for (y = 0; y < 11; y++)
   {
      for (x = 0; x < 20; x++)
      {
         px = ((y << 4) + (y << 2)) + x;
         ground.x  = liquid.x = spikes.x = food.x = x << 4;
         ground.y  = liquid.y = spikes.y = food.y = y << 4;
         enemy.x   = key_stuff.x = x << 4;
         enemy.y   = key_stuff.y = y << 4;
         temp = level_store[offset + px];
         if (temp < 47)
         {
            ground.curr_frame = temp;
            Draw_Sprite((sprite_ptr)&ground);
         }
         if (temp >= 59 && temp < 61)
         {
            if (fire_anim && temp == 60)
            {
               liquid.curr_frame = (temp - 59) + 1;
               Draw_Sprite((sprite_ptr)&liquid);
            }
            else
            {
               liquid.curr_frame = temp - 59;
               Draw_Sprite((sprite_ptr)&liquid);
            }
         }
         if (temp >= 55 && temp < 59)
         {
            spikes.curr_frame = temp - 55;
            Draw_Sprite((sprite_ptr)&spikes);
         }
         if (temp >= 47 && temp < 52)
         {
            food.curr_frame = temp - 47;
            Draw_Sprite((sprite_ptr)&food);
         }
         if (temp >= 52 && temp < 55)
         {
            key_stuff.curr_frame = temp - 52;
            Draw_Sprite((sprite_ptr)&key_stuff);
         }
         if (temp > 63 && temp < 68)
         {
            enemy.curr_frame = ((temp - 64) * 3) + (sprite_ctr / 6);
            Draw_Sprite((sprite_ptr)&enemy);
         }
         if (temp == 69)
         {
            enemy.curr_frame = 12;
            Draw_Sprite((sprite_ptr)&enemy);
         }
      }
   }
   Draw_Inventory();
   Update_Score();
   alpha = 0;
}

void Open_Sprite_PCX(char *pcxname, int pal_ver)
{
   RGB_color color;

   unsigned char head[135], data;
   int count = 0, index; 
   unsigned int width, height;
   int num_bytes;
   FILE *fp;

   fp = fopen(pcxname, "rb");
   for (index = 0; index < 128; index++)
   {
      head[index] = getc(fp);
   }
   width  = (head[8]  + (head[9]  << 8)) + 1;
   height = (head[10] + (head[11] << 8)) + 1;
   sprites = _fmalloc(width * height);
   while (count <= width * height)
   {
      data = (unsigned char)getc(fp);
      if (data >= 192)
      {
	 num_bytes = data - 192;
	 data = (unsigned char)getc(fp);
	 while (num_bytes-->0)
	 {
	    sprites[count++] = data;
	 }
      }
      else
      {
	 sprites[count++] = data;
      }
   }
   if (pal_ver)
   {
      fseek(fp, -768L, SEEK_END);
      for (index = 0; index < 256; index ++)
      {
	 color.red   = (unsigned char)(getc(fp) >> 2);
	 color.green = (unsigned char)(getc(fp) >> 2);
	 color.blue  = (unsigned char)(getc(fp) >> 2);
	 Set_Palette_Register(index, (RGB_color_ptr)&color);
      }
   }
   else
   {
      fseek(fp, -768L, SEEK_END);
      for (index = 0; index < 256; index++)
      {
	 prev_pal[index * 3]       = (unsigned char)(getc(fp) >> 2);
	 prev_pal[(index * 3) + 1] = (unsigned char)(getc(fp) >> 2);
	 prev_pal[(index * 3) + 2] = (unsigned char)(getc(fp) >> 2);
      }
   }
   fclose(fp);
}

void Load_Episode(char *filename)
{
   int x, mod = 0, set = 0;
   unsigned char store[5], temp;
   FILE *fpe;

   if ((fpe = fopen(filename, "rb")) == NULL)
      Report_Error("Error opening Level");
   fread(store, 5, 1, fpe);
   if (store[0] == 'T' && store[1] == 'P' && store[2] == 'E' && store[3] == 'D'
       && store[4] == 'F')
   {
      temp = (fgetc(fpe) & 65280) >> 8;
      num_monsters = temp + (fgetc(fpe) & 255);
      for (x = 0; x < 20000; x++)
         monster_store[x] = NULL;
      for (x = 0; x < 13; x++)
         pcxfile[x] = 32;
      for (x = 0; x < 8; x++)
      {
         temp = fgetc(fpe);
         if (temp == 32 && set == 0)
         {
            mod = x;
            set = 1;
         }
         if (temp != 32)
            pcxfile[x] = temp;
      }
      if (mod == 0)
         mod = x;
      pcxfile[mod] = '.';
      pcxfile[mod + 1] = 'G';
      pcxfile[mod + 2] = 'T';
      pcxfile[mod + 3] = 'F';
      pcxfile[mod + 4] = '\0';
      temp = fgetc(fpe);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      store[0] = fgetc(fpe);
      pscy = store[0] & 15;
      pscx = (store[0] & 240) >> 4;
      temp = fgetc(fpe);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      psbx = fgetc(fpe);
      psby = fgetc(fpe);
      store[0] = fgetc(fpe);
      pecy = store[0] & 15;
      pecx = (store[0] & 240) >> 4;
      pebx = fgetc(fpe);
      peby = fgetc(fpe);
      temp = fgetc(fpe);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      for (x = 0; x < 22000; x++)
         level_store[x] = fgetc(fpe);
      for (x = 0; x < num_monsters << 2; x++)
         monster_store[x] = fgetc(fpe);
   }
   _fmemcpy(temp_level, level_store, 22000);
   fclose(fpe);
}

//////////////////////////////////////////////////////////////////////////////
/// This function draws sprites to the screen while checking for boundaries
/// where the sprite isn't allowed to be drawn. It is higly specialised in
/// that it will only suit (currently) the scrolling of "Terminal" on the
/// intro screen.
//////////////////////////////////////////////////////////////////////////////
void Draw_Sprite_Clipped(sprite_ptr sprite)
{
   char far *work_sprite;
   int work_offset = 0, offset, x_off, x, y, xs, xe,
       clip_width;
   unsigned char data;

   xs = sprite->x;
   xe = xs + 83;

   if ((xs >= 320) || (xs <= -84))
      return;   
   if (xs < 0)
      xs = 0;
   if (xe >= 320)
      xe = 319;

   clip_width  = xe - xs + 1;
   work_offset = 0;
   x_off       = (xs - sprite->x);

   work_sprite = sprite->frames[sprite->curr_frame];
   offset = 640 + xs;

   for (y = 0; y < 70; y++)
   {
      for (x = 0; x < clip_width; x++)
      {
         if(data = work_sprite[work_offset + x + x_off])
            double_buffer[offset + x] = data;
      }
      offset      += 320;
      work_offset += 84;	 
   }
}

void Introduction(void)
{
   unsigned int x;
   int px, py, offset;
   long ctr = 0;
   RGB_color color;

   Fade_Out();
   Fill_Colours();
   Set_Video_Mode(0x13);
//   Open_Sprite_PCX("graphics\\intfont1.pcx", 1);
   Get_Batch_PCX("PACMAN.PPE", 0, 1);
   Sprite_Init((sprite_ptr)&t, 320, 10, 0, 0, 0, 0, 84, 70, 1); 
   Sprite_Init((sprite_ptr)&e, 405, 10, 0, 0, 0, 0, 84, 70, 1);    
   Sprite_Init((sprite_ptr)&r, 490, 10, 0, 0, 0, 0, 84, 70, 1);    
   Sprite_Init((sprite_ptr)&m, 575, 10, 0, 0, 0, 0, 84, 70, 1);    
   Sprite_Init((sprite_ptr)&n, 688, 10, 0, 0, 0, 0, 84, 70, 1);    
   Sprite_Init((sprite_ptr)&a, 775, 10, 0, 0, 0, 0, 84, 70, 1);    
   Grab_Bitmap((sprite_ptr)&t, 0, 0, 0);
   Grab_Bitmap((sprite_ptr)&e, 0, 1, 0);
   Grab_Bitmap((sprite_ptr)&r, 0, 2, 0);
   Grab_Bitmap((sprite_ptr)&m, 0, 0, 1);
   Grab_Bitmap((sprite_ptr)&n, 0, 1, 1);
   Grab_Bitmap((sprite_ptr)&a, 0, 2, 1);
   _ffree(sprites);
   Get_Batch_PCX("PACMAN.PPE", 1, 1);
   Sprite_Init((sprite_ptr)&i, 632, 10, 0, 0, 0, 0, 84, 70, 1);
   Sprite_Init((sprite_ptr)&l, 860, 10, 0, 0, 0, 0, 84, 70, 1);
   Grab_Bitmap((sprite_ptr)&i, 0, 1, 0);
   Grab_Bitmap((sprite_ptr)&l, 0, 0, 0);
   _ffree(sprites);
   double_buffer = _fmalloc(64000);
   for (x = 0; x < 64000; x++)
   {
      video_buffer[x] = 0;
      double_buffer[x] = 0;
   }
   while (!kbhit())
   {
      delay(30);
      vblank();
      _fmemset(double_buffer, 0, 23040);
      Draw_Sprite_Clipped((sprite_ptr)&t);
      Draw_Sprite_Clipped((sprite_ptr)&e);
      Draw_Sprite_Clipped((sprite_ptr)&r);
      Draw_Sprite_Clipped((sprite_ptr)&m);
      Draw_Sprite_Clipped((sprite_ptr)&i);
      Draw_Sprite_Clipped((sprite_ptr)&n);
      Draw_Sprite_Clipped((sprite_ptr)&a);
      Draw_Sprite_Clipped((sprite_ptr)&l);
      _fmemcpy(temp_buffer, double_buffer, 23040);
      t.x-=3;
      e.x-=3;
      r.x-=3;
      m.x-=3;
      i.x-=3;
      n.x-=3;
      a.x-=3;
      l.x-=3;
      if (l.x <= -80)
	 goto Skip_It1;
   }
   getch();
   Skip_It1:
   Sprite_Delete((sprite_ptr)&t);
   Sprite_Delete((sprite_ptr)&e);
   Sprite_Delete((sprite_ptr)&r);
   Sprite_Delete((sprite_ptr)&m);
   Sprite_Delete((sprite_ptr)&n);
   Sprite_Delete((sprite_ptr)&a);
   Sprite_Delete((sprite_ptr)&i);
   Sprite_Delete((sprite_ptr)&l);
   _fmemset(video_buffer, 0, 64000);
   Get_Batch_PCX("PACMAN.PPE", 2, 1);
   while (!kbhit())
   {
      px = (rand()%263) + 32;      
      py = (rand()%40) + 80;
      offset = (py << 8) + (py << 6) + px;
      if (video_buffer[offset != 0])
	 video_buffer[offset] = sprites[offset];
      else 
	 while (video_buffer[offset] != 0)
	    offset++;
	 video_buffer[offset] = sprites[offset];
      ctr++;
      if (ctr >= 64000)
	 goto Skip_It2;
   }	   
   getch();
   Skip_It2:;
   _ffree(sprites);
   Get_Internal_PCX(0);
   for (ctr = 0; ctr < 63; ctr++)
   {
      for (x = 0; x < 256; x++)
      {
	 Get_Palette_Register(x, (RGB_color_ptr)&color);

	 if (color.red   <= 62)
	    color.red++;
	 if (color.green <= 62)
	    color.green++;
	 if (color.blue  <= 62)
	    color.blue++;
	 
	 Set_Palette_Register(x, (RGB_color_ptr)&color);
      }
   }
   delay(250);
   _fmemcpy(video_buffer, inpcx, 64000);
   for (x = 0; x < 63; x++)
   {
      for (ctr = 0; ctr < 256; ctr++)
      {
	 Get_Palette_Register(ctr, (RGB_color_ptr)&color);

	 if (color.red > prev_pal[ctr * 3])
	    color.red--;
	 if (color.green > prev_pal[(ctr * 3) + 1])
	    color.green--;
	 if (color.blue > prev_pal[(ctr * 3) + 2])
	    color.blue--;

	 Set_Palette_Register(ctr, (RGB_color_ptr)&color);
      }
   }
   getch();
   _ffree(inpcx);
   Display_Menu();
   _fmemset(video_buffer, 0, 64000);
   _ffree(double_buffer);
}

void Draw_Menu_Box(void)
{
   unsigned int x, y;

   for (y = 85; y < 185; y++)
   {
      for (x = 50; x < 270; x++)
      {
         video_buffer[(y << 8) + (y << 6) + x] = 0;
      }
   }
   Bline(51, 86, 268, 86, 138);
   Bline(51, 86, 51, 183, 138);
   Bline(51, 183, 268, 183, 138);
   Bline(268, 183, 268, 86, 138);
   Bline(52, 87, 267, 87, 227);
   Bline(52, 87, 52, 182, 227);
   Bline(52, 182, 267, 182, 227);
   Bline(267, 182, 267, 87, 227);
   Bline(53, 88, 268, 88, 138);
   Bline(53, 88, 53, 181, 138);
   Bline(53, 181, 268, 181, 138);
   Bline(268, 181, 268, 88, 138);
   Blit_String(120, 101, 1, title_names[0], 0);
   Blit_String(120, 111, 1, title_names[1], 0);
   Blit_String(120, 121, 1, title_names[2], 0);
   Blit_String(120, 131, 1, title_names[3], 0);
   Blit_String(120, 141, 1, title_names[4], 0);
   Blit_String(120, 151, 1, title_names[5], 0);
}

void Draw_BoxO(void)
{
   unsigned int x, y;

   for (y = 85; y < 185; y++)
   {
      for (x = 50; x < 270; x++)
      {
         video_buffer[(y << 8) + (y << 6) + x] = 0;
      }
   }
   Bline(51, 86, 268, 86, 138);
   Bline(51, 86, 51, 183, 138);
   Bline(51, 183, 268, 183, 138);
   Bline(268, 183, 268, 86, 138);
   Bline(52, 87, 267, 87, 227);
   Bline(52, 87, 52, 182, 227);
   Bline(52, 182, 267, 182, 227);
   Bline(267, 182, 267, 87, 227);
   Bline(53, 88, 268, 88, 138);
   Bline(53, 88, 53, 181, 138);
   Bline(53, 181, 268, 181, 138);
   Bline(268, 181, 268, 88, 138);
}

void Display_Menu(void)
{
   unsigned char inkey;

   _fmemset(video_buffer, 0, 64000);
   Get_Internal_PCX(1);
   _fmemcpy(video_buffer, inpcx, 64000);
   _ffree(sprites);
   Draw_Menu_Box();
   _ffree(inpcx);
   Display_Pac(106, 101 + (pac_pos * 10));
   Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
   while (inkey != 27)
   {
      inkey = getch();
      if (!inkey)
      {
         inkey = getch();
         if (inkey == 72 && pac_pos > 0)
         {
            pac_pos--;
            Remove_Pac(106, 111 + (pac_pos * 10));
            Display_Pac(106, 101 + (pac_pos * 10));
            Blit_String(120, 111 + (pac_pos * 10), 1, title_names[pac_pos + 1], 0);
            Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
         }
         else
         {
            if (inkey == 72 && pac_pos == 0)
            {
               pac_pos = 5;
               Remove_Pac(106, 101);
               Display_Pac(106, 151);
               Blit_String(120, 101, 1, title_names[0], 0);
               Blit_String(120, 151, 0, title_names[5], 0);
            }
         }
         if (inkey == 80 && pac_pos < 5)
         {
            pac_pos++;
            Remove_Pac(106, 91 + (pac_pos * 10));
            Display_Pac(106, 101 + (pac_pos * 10));
            Blit_String(120, 91 + (pac_pos * 10), 1, title_names[pac_pos - 1], 0);
            Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
         }
         else
         {
            if (inkey == 80 && pac_pos == 5)
            {
               pac_pos = 0;
               Remove_Pac(106, 151);
               Display_Pac(106, 101);
               Blit_String(120, 151, 1, title_names[5], 0);
               Blit_String(120, 101, 0, title_names[0], 0);
            }
         }
      }
      if (inkey == 13 && pac_pos == 5)
      {
         Set_Video_Mode(0x03);
         printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
         printf("\n---===---=== Terminal Pacity ===---===---");
         printf("\n--- Copyright (c) 1997 Don Williamson ---");
         printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
         printf("\nMemory deallocating...");
         printf("\nReleasing Keyboard...");
         Free_Memory();
         Change_Timer(0xFFFF);
         if (SB_Reset) Release_Blaster();
         printf("\nDone.\n");
         exit(1);
      }
      if (inkey == 13 && pac_pos == 4)
      {
         level_names[0][0]  = 'L';
         level_names[0][1]  = 'E';
         level_names[0][2]  = 'V';
         level_names[0][3]  = 'E';
         level_names[0][4]  = 'L';
         level_names[0][5]  = '1';
         level_names[0][6]  = ' ';
         level_names[0][7]  = ' ';
         level_names[0][8]  = '.';
         level_names[0][9]  = 'T';
         level_names[0][10]  = 'P';
         level_names[0][11]  = 'E';
         level_names[0][12]  = '\0';
      }
      if (inkey == 13 && pac_pos == 3)
      {
         High_Score(0);
         Get_Internal_PCX(1);
         _fmemcpy(video_buffer, inpcx, 64000);
         Draw_Menu_Box();
         _ffree(inpcx);
         Display_Pac(106, 101 + (pac_pos * 10));
         Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
      }
      if (inkey == 13 && pac_pos == 2)
      {
         _fmemset(video_buffer, 0, 64000);
         Display_Credits();
         Fade_Out();
         Get_Internal_PCX(1);
         _fmemcpy(video_buffer, inpcx, 64000);
         Draw_Menu_Box();
         _ffree(inpcx);
         Display_Pac(106, 101 + (pac_pos * 10));
         Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
      }
      if (inkey == 13 && pac_pos == 1)
      {
         inkey = 0xFF;
         Draw_BoxO();
         pac_pos = 0;
         Display_Pac(106, 101 + (pac_pos * 10));
         Blit_String(120, 101 + (pac_pos * 10), 0, opscr_names[pac_pos], 0);
         Blit_String(120, 111, 1, opscr_names[1], 0);
         Blit_String(120, 121, 1, opscr_names[2], 0);
         sprintf(str_level, "%i", awlives);
         Blit_String(220, 101, 1, str_level, 0);
         if (fire_anim) Blit_String(220, 111, 1, "On ", 0);
         else Blit_String(220, 111, 1, "Off", 0);
         if (SB_Enable) Blit_String(220, 121, 1, "On ", 0);
         else Blit_String(220, 121, 1, "Off", 0);
         while (inkey != 27 && inkey != 13)
         {
            inkey = getch();
            if (inkey == 0)
            {
               inkey = getch();
               if (inkey == 80)
               {
                  if (pac_pos == 0 || pac_pos == 1)
                  {
                     pac_pos++;
                     Remove_Pac(106, 91 + (pac_pos * 10));
                     Display_Pac(106, 101 + (pac_pos * 10));
                     Blit_String(120, 91 + (pac_pos * 10), 1, opscr_names[pac_pos - 1], 0);
                     Blit_String(120, 101 + (pac_pos * 10), 0, opscr_names[pac_pos], 0);
                  }
               }
               if (inkey == 72)
               {
                  if (pac_pos == 2 || pac_pos == 1)
                  {
                     pac_pos--;
                     Remove_Pac(106, 111 + (pac_pos * 10));
                     Display_Pac(106, 101 + (pac_pos * 10));
                     Blit_String(120, 111 + (pac_pos * 10), 1, opscr_names[pac_pos + 1], 0);
                     Blit_String(120, 101 + (pac_pos * 10), 0, opscr_names[pac_pos], 0);
                  }
               }
               if (inkey == 77 && awlives < 5 && pac_pos == 0)
               {
                  awlives++;
                  sprintf(str_level, "%i", awlives);
                  Blit_String(220, 101, 1, str_level, 0);
               }
               if (inkey == 75 && awlives > 1 && pac_pos == 0)
               {
                  awlives--;
                  sprintf(str_level, "%i", awlives);
                  Blit_String(220, 101, 1, str_level, 0);
               }
               if (inkey == 77 && fire_anim == 0 && pac_pos == 1)
               {
                  fire_anim = 1;
                  Blit_String(220, 111, 1, "On ", 0);
               }
               if (inkey == 75 && fire_anim == 1 && pac_pos == 1)
               {
                  fire_anim = 0;
                  Blit_String(220, 111, 1, "Off", 0);
               }
               if (inkey == 77 && SB_Enable == 0 && pac_pos == 2 && SB_Reset)
               {
                  SB_Enable = 1;
                  Blit_String(220, 121, 1, "On ", 0);
               }
               if (inkey == 75 && SB_Enable == 1 && pac_pos == 2)
               {
                  SB_Enable = 0;
                  Blit_String(220, 121, 1, "Off", 0);
               }
            }
         }
         pac_pos = 1;
         inkey   = 0xFF;
         Draw_Menu_Box();
         Display_Pac(106, 101 + (pac_pos * 10));
         Blit_String(120, 101 + (pac_pos * 10), 0, title_names[pac_pos], 0);
      }
      if (inkey == 13 && pac_pos == 0)
         goto MeEnd;
   }
   MeEnd:;
   lives = awlives;
}

void Display_Credits(void)
{
   int x, y, z, place;

   Get_Batch_PCX("PACMAN.PPE", 4, 1);
   _fmemcpy(video_buffer, sprites, 64000);
   for (place = 0; place < 8; place++)
   {
      for (x = 132; x > 60; x--)
      {
         delay(20);
         vblank();
         for (y = 0; y < 3; y++)
         {
            if (kbhit())
            {
               getch();
               goto thend;
            }
            if (x + Credit_Value[y] < 190)
            {
               Blit_String_Credit(170 - ((strlen(Credit_Table[place][y]) << 3) >> 1), x + Credit_Value[y], 240, Credit_Table[place][y], 0);
            }
         }
      }
      delay(200);
      for (y = 120; y > 68; y--)
      {
         for (z = y; z > y - 8; z--)
         {
            if (kbhit())
            {
               getch();
               goto thend;
            }
            for (x = 70; x < 266; x++)
            {
               if (video_buffer[pixtab[z + 40] + x]) video_buffer[pixtab[z + 40] + x] = 248 - (y - z);
            }
         }
         delay(10);
         vblank();
      }
      for (y = 120; y > 60; y--)
      {
         for (x = 70; x < 266; x++)
         {
            video_buffer[pixtab[y + 40] + x] = 0;
         }
      }
   }
   thend:;
   _ffree(sprites);

}

void High_Score(unsigned char mode)
{
   FILE *fp;
   unsigned char strng[10], tempd, hscore_store[10][10], done = 0, ftemp, fchar[2];
   unsigned long high_score[10], value;
   unsigned char far *back_store;
   unsigned int slx = 80, mx, my;
   long x;

   strcpy(strng, "Empty   ");
   Get_Batch_PCX("PACMAN.PPE", 3, 1);
   _fmemcpy(video_buffer, sprites, 64000);
   if ((fp = fopen("PACMAN.SCR", "rb")) == NULL)
   {
      fclose(fp);
      fp = fopen("PACMAN.SCR", "wb");
      fprintf(fp, "TPHSF");
      for (x = 0; x < 10; x++)
      {
         fprintf(fp, "%c", strng[0] + 128);
         fprintf(fp, "%c", strng[1] + 128);
         fprintf(fp, "%c", strng[2] + 128);
         fprintf(fp, "%c", strng[3] + 128);
         fprintf(fp, "%c", strng[4] + 128);
         fprintf(fp, "%c", strng[5] + 128);
         fprintf(fp, "%c", strng[6] + 128);
         fprintf(fp, "%c", strng[7] + 128);
      }
      for (x = 0; x < 40; x++)
         fprintf(fp, "%c", 0);
   }
   fclose(fp);
   fp = fopen("PACMAN.SCR", "rb");
   fseek(fp, 5, SEEK_SET);
   for (x = 0; x < 10; x++)
   {
      fread(strng, 8, 1, fp);
      strng[0] -= 128;
      strng[1] -= 128;
      strng[2] -= 128;
      strng[3] -= 128;
      strng[4] -= 128;
      strng[5] -= 128;
      strng[6] -= 128;
      strng[7] -= 128;
      strng[8] =  '\0';
      strcpy(hscore_store[x], strng);
      if (!mode) Blit_String(80, (x * 15) + 30, 3, strng, 1);
   }
   for (x = 0; x < 10; x++)
   {
      value = ((long)fgetc(fp) << 24) + ((long)fgetc(fp) << 16) + ((long)fgetc(fp) << 8) + (long)fgetc(fp);
      high_score[x] = value;
      if (!mode)
      {
         sprintf(strng, "%ld", (unsigned long)value);
         Blit_String(250, (x * 15) + 30, 3, strng, 1);
      }
   }
   fclose(fp);
   if (score > high_score[9] && mode == 1)
   {
      for (x = 9; x > -1; x--)
         if (score > high_score[x]) tempd = x;
      for (x = 8; x > (tempd - 1); x--)
      {
         sprintf(hscore_store[x + 1], "%s", hscore_store[x]);
         high_score[x + 1] = high_score[x];
      }
      high_score[tempd] = score;
      back_store = _fmalloc(512);
      for (my = 30 + (tempd * 15); my < 38 + (tempd * 15); my++)
      {
         for (mx = 80; mx < 144; mx++)
         {
            back_store[((my - (30 + (tempd * 15))) * 64) + (mx - 80)] = video_buffer[(my << 6) + (my << 8) + mx];
         }
      }
      while(!done)
      {
         ftemp = getch();
         if (isalnum(ftemp) && slx < 144)
         {
            sprintf(fchar, "%c", ftemp);
            fchar[1] = '\0';
            Blit_String(slx, 30 + (tempd * 15), 3, fchar, 1);
            hscore_store[tempd][(slx / 8) - 10] = fchar[0];
            hscore_store[tempd][(slx / 8) - 9] = '\0';
            slx += 8;
         }
         if (ftemp == 8 && slx > 80)
         {
            slx -= 8;
            for (my = 30 + (tempd * 15); my < 38 + (tempd * 15); my++)
            {
               for (mx = slx; mx < slx + 8; mx++)
               {
                  video_buffer[(320 * my) + mx] = back_store[((my - (30 + (tempd * 15))) * 64) + (mx - 80)];
               }
            }
            hscore_store[tempd][(slx / 8) - 10] = '\0';
         }
         if (ftemp == 13)
         {
            for (x = 0; x < 10; x++)
            {
               Blit_String(80, (x * 15) + 30, 3, hscore_store[x], 1);
               sprintf(strng, "%lu", high_score[x]);
               Blit_String(250, (x * 15) + 30, 3, strng, 1);
            }
            fp = fopen("PACMAN.SCR", "wb");
            fprintf(fp, "TPHSF");
            for (x = 0; x < 10; x++)
            {
               tempd = strlen(hscore_store[x]);
               for (mx = 0; mx < tempd; mx++)
               {
                  fprintf(fp, "%c", hscore_store[x][mx] + 128);
               }
               for (mx = tempd; mx < 8; mx++)
               {
                  fprintf(fp, "%c", 160);
               }
            }
            for (x = 0; x < 10; x++)
            {
               fprintf(fp, "%c%c%c%c", (char)(high_score[x] >> 24), (char)((high_score[x] >> 16) & 255), (char)((high_score[x] >> 8) & 255), (char)(high_score[x] & 255));
            }
            fclose(fp);
            done = 1;
         }
      }
      _ffree(back_store);
   }
   else
   {
      for (x = 0; x < 10; x++)
      {
         Blit_String(80, (x * 15) + 30, 3, hscore_store[x], 1);
         sprintf(strng, "%lu", high_score[x]);
         Blit_String(250, (x * 15) + 30, 3, strng, 1);
      }
   }
   getch();
   _ffree(sprites);
}

void Load_Level(char *filename)
{
   Load_Episode(filename);
   Sprite_Delete((sprite_ptr)&pacman);
   Sprite_Delete((sprite_ptr)&ground);
   Sprite_Delete((sprite_ptr)&spikes);
   Sprite_Delete((sprite_ptr)&liquid);
   Sprite_Delete((sprite_ptr)&food);
   Sprite_Delete((sprite_ptr)&enemy);
   Sprite_Delete((sprite_ptr)&key_stuff);
   Open_Sprite_PCX(pcxfile, 1);
   Get_Sprite_Info();
   _ffree(sprites);

   pacman.x = psbx << 4;
   pacman.y = psby << 4;
   temp_y = pacman.y;
   state = 1;
   keys = 0;
   screen_x = pscx;
   screen_y = pscy;
   Draw_Sprite((sprite_ptr)&pacman);
   Draw_Screen(screen_x, screen_y);
   Screen_Logic();
}

void Fill_Colours(void)
{
   col[0][0] = 190;  // Red Title Screen
   col[0][1] = 242;
   col[0][2] = 76;
   col[0][3] = 227;
   col[0][4] = 227;
   col[0][5] = 76;
   col[0][6] = 242;
   col[0][7] = 190;

   col[1][0] = 8;    // Green Title Screen
   col[1][1] = 217;
   col[1][2] = 241;
   col[1][3] = 140;
   col[1][4] = 140;
   col[1][5] = 241;
   col[1][6] = 217;
   col[1][7] = 8;

   col[2][0] = 7;  // Green In-Game
   col[2][1] = 8;
   col[2][2] = 9;
   col[2][3] = 10;
   col[2][4] = 11;
   col[2][5] = 12;
   col[2][6] = 13;
   col[2][7] = 13;

   col[3][0] = 72;  // Green High Score screen
   col[3][1] = 248;
   col[3][2] = 208;
   col[3][3] = 196;
   col[3][4] = 196;
   col[3][5] = 208;
   col[3][6] = 248;
   col[3][7] = 72;

   col[4][0] = 50; // End-Game Fire Text
   col[4][1] = 60;
   col[4][2] = 70;
   col[4][3] = 80;
   col[4][4] = 90;
   col[4][5] = 100;
   col[4][6] = 110;
   col[4][7] = 120;

   col[5][0] = 10; // End-Game Shadow Text
   col[5][1] = 15;
   col[5][2] = 20;
   col[5][3] = 25;
   col[5][4] = 30;
   col[5][5] = 35;
   col[5][6] = 40;
   col[5][7] = 45;
}

void Load_Sounds(void)
{
   EMM_Alloc(1, &splat.handle);
   EMM_Alloc(1, &hitroof.handle);
   EMM_Alloc(1, &collect.handle);
   EMM_Alloc(1, &ding.handle);
   EMM_Alloc(1, &hurt.handle);
   EMM_Alloc(1, &keysnd.handle);
   EMM_Alloc(1, &opendoor.handle);
   EMM_Alloc(3, &finale.handle);
   PCM_Load_EMS((emsample_ptr)&splat, "sfx\\splat1.pcm", 3835);
   PCM_Load_EMS((emsample_ptr)&hitroof, "sfx\\hitroof.pcm", 1596);
   PCM_Load_EMS((emsample_ptr)&collect, "sfx\\collect.pcm", 3336);
   PCM_Load_EMS((emsample_ptr)&ding, "sfx\\ding.pcm", 10479);
   PCM_Load_EMS((emsample_ptr)&keysnd, "sfx\\green.pcm", 13513);
   PCM_Load_EMS((emsample_ptr)&hurt, "sfx\\ow3.pcm", 8088);
   PCM_Load_EMS((emsample_ptr)&opendoor, "sfx\\opendoor.pcm", 6902);
   PCM_Load_EMS((emsample_ptr)&finale, "sfx\\tune.pcm", 39420);
}

void Draw_Circle(unsigned int x ,unsigned int y, unsigned char color)
{
   video_buffer[(y << 6) + (y << 8) + x + 1] = color;
   video_buffer[(y << 6) + (y << 8) + x + 2] = color;
   video_buffer[((y + 1) << 6) + ((y +  1) << 8) + x] = color;
   video_buffer[((y + 1) << 6) + ((y +  1) << 8) + x + 1] = color;
   video_buffer[((y + 1) << 6) + ((y +  1) << 8) + x + 2] = color;
   video_buffer[((y + 1) << 6) + ((y +  1) << 8) + x + 3] = color;
   video_buffer[((y + 2) << 6) + ((y +  2) << 8) + x] = color;
   video_buffer[((y + 2) << 6) + ((y +  2) << 8) + x + 1] = color;
   video_buffer[((y + 2) << 6) + ((y +  2) << 8) + x + 2] = color;
   video_buffer[((y + 2) << 6) + ((y +  2) << 8) + x + 3] = color;
   video_buffer[((y + 3) << 6) + ((y +  3) << 8) + x + 1] = color;
   video_buffer[((y + 3) << 6) + ((y +  3) << 8) + x + 2] = color;
}

void Game_Operation(char save)
{
   FILE *fp;
   unsigned char far *box;
   unsigned int x, y;
   unsigned char string[13] = "\0", pos = 1, inkey, tmp, tmp1;

   box = _fmalloc(14208);
   for (y = 52; y < 148; y++)
   {
      for (x = 86; x < 234; x++)
      {
         box[((y - 52) * 148) + (x - 86)] = video_buffer[(y << 6) + (y << 8) + x];
         video_buffer[(y << 6) + (y << 8) + x] = 155;
      }
   }
   for (y = 68; y < 120; y += 12)
   {
      for (x = 95; x < 223; x++)
      {
         video_buffer[(y << 6) + (y << 8) + x] = 0;
         video_buffer[((y + 1) << 6) + ((y + 1) << 8) + x] = 0;
         video_buffer[((y + 2) << 6) + ((y + 2) << 8) + x] = 0;
         video_buffer[((y + 3) << 6) + ((y + 3) << 8) + x] = 0;
         video_buffer[((y + 4) << 6) + ((y + 4) << 8) + x] = 0;
         video_buffer[((y + 5) << 6) + ((y + 5) << 8) + x] = 0;
         video_buffer[((y + 6) << 6) + ((y + 6) << 8) + x] = 0;
         video_buffer[((y + 7) << 6) + ((y + 7) << 8) + x] = 0;
         video_buffer[((y + 8) << 6) + ((y + 8) << 8) + x] = 0;
         video_buffer[((y + 9) << 6) + ((y + 9) << 8) + x] = 0;
      }
   }
   Bline(86, 52, 233, 52, 193);
   Bline(86, 52, 86, 147, 193);
   Bline(86, 147, 233, 147, 94);
   Bline(233, 52, 233, 147, 94);
   Plot_Pixel_Fast(86, 147, 155);
   Plot_Pixel_Fast(233, 52, 155);
   for (y = 71; y < 129; y += 12)
      Draw_Circle(225, y, 0);
   if ((fp = fopen("SAVG01.PXS", "rb")) == NULL) Blit_String(96, 69, 2, "Unused", 0);
   else Blit_String(96, 69, 2, "Save Game 1", 0);
   fclose(fp);
   if ((fp = fopen("SAVG02.PXS", "rb")) == NULL) Blit_String(96, 81, 2, "Unused", 0);
   else Blit_String(96, 81, 2, "Save Game 2", 0);
   fclose(fp);
   if ((fp = fopen("SAVG03.PXS", "rb")) == NULL) Blit_String(96, 93, 2, "Unused", 0);
   else Blit_String(96, 93, 2, "Save Game 3", 0);
   fclose(fp);
   if ((fp = fopen("SAVG04.PXS", "rb")) == NULL) Blit_String(96, 105, 2, "Unused", 0);
   else Blit_String(96, 105, 2, "Save Game 4", 0);
   fclose(fp);
   if ((fp = fopen("SAVG05.PXS", "rb")) == NULL) Blit_String(96, 117, 2, "Unused", 0);
   else Blit_String(96, 117, 2, "Save Game 5", 0);
   fclose(fp);
   Before:;
   if (save) Blit_String(100, 134, 2, " SAVE Game... ", 1);
   else Blit_String(100, 134, 2, " LOAD Game... ", 1);
   Draw_Circle(225, 59 + (pos * 12), 217);
   inkey = getch();
   if (inkey == 0)
   {
      inkey = getch();
      if (inkey == 80)
      {
         if (pos < 5)
         {
            Draw_Circle(225, 59 + (pos * 12), 0);
            pos++;
         }
         goto Before;
      }
      if (inkey == 72)
      {
         if (pos > 1)
         {
            Draw_Circle(225, 59 + (pos * 12), 0);
             pos--;
         }
         goto Before;
      }
   }
   if (inkey == 13 && save == 1)
   {
      sprintf(string, "savg0%i.PXS", pos);
      fp = fopen(string, "wb");
      fprintf(fp, "TPSG -- Terminal Pacity Save Game%c", 0x1A);
      fprintf(fp, "%c", (clevel << 1) + 1);
      fprintf(fp, "%c", level_names[clevel][0] + 128);
      fprintf(fp, "%c", level_names[clevel][1] + 128);
      fprintf(fp, "%c", level_names[clevel][2] + 128);
      fprintf(fp, "%c", level_names[clevel][3] + 128);
      fprintf(fp, "%c", level_names[clevel][4] + 128);
      fprintf(fp, "%c", level_names[clevel][5] + 128);
      fprintf(fp, "%c", level_names[clevel][6] + 128);
      fprintf(fp, "%c", level_names[clevel][7] + 128);
      fprintf(fp, "%c%c", screen_x, screen_y);
      fprintf(fp, "%c", (pacman.x >> 8));
      fprintf(fp, "%c", (pacman.x & 255));
      fprintf(fp, "%c", pacman.y);
      fprintf(fp, "%c", scr_trk);
      fprintf(fp, "%c%c", (char)((score >> 24) & 255), (char)((score >> 16) & 255));
      fprintf(fp, "%c%c", (char)((score >> 8) & 255), (char)(score & 255));
      fprintf(fp, "%c", lives + 5);
      fprintf(fp, "%c%c", (keys >> 8), (keys & 255));
      fprintf(fp, "%c%c", 0xFF, 0x00); // Signifying beginning of FOOD_STORE.
      for (x = 0; x < 22000; x++)
      {
         if (temp_level[x] >= 47 && temp_level[x] < 52)
         {
            if (level_store[x] != temp_level[x])
            {
               fprintf(fp, "%c%c", (x >> 8), (x & 255));
            }
         }
      }
      fprintf(fp, "%c%c", 0xFF, 0x00); // Signifying beginning of KEY_STORE.
      for (x = 0; x < 22000; x++)
      {
         if (temp_level[x] == 52)
         {
            if (level_store[x] != temp_level[x])
            {
               fprintf(fp, "%c%c", (x >> 8), (x & 255));
            }
         }
      }
      fprintf(fp, "%c%c", 0xFF, 0x00); // Signifying beginning of open doors.
      for (x = 0; x < 22000; x++)
      {
         if (temp_level[x] == 53)
         {
            if (level_store[x] != temp_level[x])
            {
               fprintf(fp, "%c%c", (x >> 8), (x & 255));
            }
         }
      }
      fprintf(fp, "%c%c", 0xFF, 0x00); // Signifying EOF.
      fclose(fp);
   }
   if (inkey == 13 && save == 0)
   {
      sprintf(string, "savg0%i.PXS", pos);
      fp = fopen(string, "rb");
      fread(string, 4, 1, fp);
      if (string[0] == 'T' && string[1] == 'P' && string[2] == 'S' && string[3] == 'G')
      {
         fseek(fp, 34, SEEK_SET);
         tmp = fgetc(fp);
         clevel = (tmp - 1) / 2;
         level_names[clevel][0] = fgetc(fp) - 128;
         level_names[clevel][1] = fgetc(fp) - 128;
         level_names[clevel][2] = fgetc(fp) - 128;
         level_names[clevel][3] = fgetc(fp) - 128;
         level_names[clevel][4] = fgetc(fp) - 128;
         level_names[clevel][5] = fgetc(fp) - 128;
         level_names[clevel][6] = fgetc(fp) - 128;
         level_names[clevel][7] = fgetc(fp) - 128;
         level_names[clevel][8] = '.';
         level_names[clevel][9] = 'T';
         level_names[clevel][10] = 'P';
         level_names[clevel][11] = 'E';
         level_names[clevel][12] = '\0';

         ///
         _fmemset(video_buffer, 0, 56640);
         Load_Episode(level_names[clevel]);
         Sprite_Delete((sprite_ptr)&pacman);
         Sprite_Delete((sprite_ptr)&ground);
         Sprite_Delete((sprite_ptr)&spikes);
         Sprite_Delete((sprite_ptr)&liquid);
         Sprite_Delete((sprite_ptr)&food);
         Sprite_Delete((sprite_ptr)&enemy);
         Sprite_Delete((sprite_ptr)&key_stuff);
         Open_Sprite_PCX(pcxfile, 1);
         Get_Sprite_Info();
         _ffree(sprites);
         ///

         screen_x = fgetc(fp);
         screen_y = fgetc(fp);
         pacman.x = (fgetc(fp) << 8) + fgetc(fp);
         pacman.y = fgetc(fp);
         scr_trk  = fgetc(fp);
         score    = ((long)fgetc(fp) << 24) + ((long)fgetc(fp) << 16);
         score   += ((long)fgetc(fp) << 8) + (long)fgetc(fp);
         lives    = fgetc(fp) - 5;
         keys     = (fgetc(fp) << 8) + fgetc(fp);
         tmp      = fgetc(fp);
         tmp      = fgetc(fp);
         y = 1;
         while (y)
         {
            if ((tmp = fgetc(fp)) == 255)
            {
               y = 0;
               tmp = fgetc(fp);
            }
            else
            {
               tmp1 = fgetc(fp);
               level_store[(tmp << 8) + tmp1] = 63;
            }
         }
         y = 1;
         while (y)
         {
            if ((tmp = fgetc(fp)) == 255)
            {
               y = 0;
               tmp = fgetc(fp);
            }
            else
            {
               tmp1 = fgetc(fp);
               level_store[(tmp << 8) + tmp1] = 63;
            }
         }
         y = 1;
         while (y)
         {
            if ((tmp = fgetc(fp)) == 255)
            {
               y = 0;
               tmp = fgetc(fp);
            }
            else
            {
               tmp1 = fgetc(fp);
               level_store[(tmp << 8) + tmp1] = 63;
               level_store[(tmp << 8) + tmp1 - 20] = 63;
            }
         }
         fclose(fp);
         temp_y = pacman.y;
         state = 1;
         Draw_Sprite((sprite_ptr)&pacman);
         Draw_Screen(screen_x, screen_y);
         Screen_Logic();
         goto Me;
      }
      else fclose(fp);
   }
   for (y = 52; y < 148; y++)
   {
      for (x = 86; x < 234; x++)
      {
         video_buffer[(y << 6) + (y << 8) + x] = box[((y - 52) * 148) + (x - 86)];
      }
   }
   Me:;
   _ffree(box);
}

void Sort_Name(char *title)
{
   char string[13] = "\0";
   char tempe, x;

   tempe = strlen(title);
   strcpy(string, title);
   for (x = tempe - 4; x < 13; x++)
      string[x] = 32;
   string[8]  = '.';
   string[9]  = 'T';
   string[10] = 'P';
   string[11] = 'E';
   string[12] = '\0';
   strcpy(level_names[0], string);
}

void Change_Colour(void)
{
   RGB_color color;

   color.red   = (unsigned char)(3 / 4);
   color.green = (unsigned char)(19 / 4);
   color.blue  = (unsigned char)(95 / 4);
   Set_Palette_Register(1, (RGB_color_ptr)&color);
}

/***** MAIN ****************************************************************/

void main(int argc, char *argv[])
{
   int done = 0, x = 0, offset = 0, blk_offx, blk_offy, scrn_num;
   float disp = 0;

   Set_Video_Mode(0x03);
   Change_Colour();
   ptext(" Terminal Pacity                              Copyright (c) 1997 Don Williamson ", 0, 0, 30);
   printf("\n");
   if (argc > 1)
   {
      for (done = 1; done < argc; done++)
      {
         if (argv[done][0] == '-' && argv[done][1] == 'l')
            x = done + 1;
         if (argv[done][0] == '-' && argv[done][1] == 'n' && argv[done][2] == 'o' && argv[done][3] == 's' && argv[done][4] == 'b')
            offset = 1;
      }
   }
   done = 0;
   if (!offset) Init_Blaster(11025);
   printf("\nInit_Blaster: ");
   offset = 0;
   if (SB_Reset)
   {
      printf("Sound Blaster detected... PORT - %i  DMA - %i   IRQ - %i", DSP_Port, DMA_Channel, IRQ_Jumper);
      SB_Enable = 1;
   }
   else
   {
      printf("SoundBlaster not detected... No Sound.");
      SB_Enable = 0;
   }
   printf("\nInit_Keyboard: Goodbye BIOS! - Success");
   printf("\nInit_Timer: New clock rate - 90Hz");
   if (SB_Reset)
   {
      Load_Sounds();
      EMM_Get_Pages();
      printf("\nLoad_Sounds: 160k of EMS allocated for sound effects   Free - %ik", EMMFreePages << 4);
   }
   if (DetectWindows())
   {
      printf("\nDetect_MSW: Computer in 386 Enhanced Mode  --  ");
      if (WinInst == 1) printf("(version MSW/386 2.x)");
      else printf("(version %i.%i)", WinMaj, WinMin);
   }
   else printf("\nDetect_MSW: MSW not present.");
   printf("\nInit_Tables: Generating look-up tables");
   Generate_Pixel_Table();
   printf("\n\nPress any key...");
   printf("\n ");
   getch();
   Introduction();

   Initialise_Globals();
   Install_Keyboard();
   Change_Timer(0x4DAE);
   Old_Time_Isr = _dos_getvect(0x1C);
   _dos_setvect(0x1C, Timer);

   /// SECTION: Load Sprite Information ///

   Initialise_Sprites();
   if (x)
   {
      Sort_Name(argv[x]);
   }
   Load_Level(level_names[0]);
   while (!done)
   {
      vblank();
      Wipe_Enemies();
      Move_Enemies();
      Update_Enemies();
      if (Check_Collision())
      {
         state = 1;
         disp = 3.15;
      }
//      offset = (pacman.y << 8) + (pacman.y << 6) + pacman.x;
      if (key_table[9] || fingame)
      {
         fingame = 0;
         Title_Screen();
         for (x = 0; x < 10; x++)
            key_table[x] = 0;
      }
      Erase_Sprite((sprite_ptr)&pacman);
      offset = pixtab[pacman.y + 40] + pacman.x;
      if ((video_buffer[offset - 320] == 155 || video_buffer[offset - 305] == 155) &&
	   state == 2)
      {
         state = 0;                              /* Hitting Ceiling */
         disp = 0;
         if (SB_Enable && !spi[0])
            SBPlayEMS((emsample_ptr)&hitroof, hitroof.length);
      }
      if ((video_buffer[offset - 640] == 155 || video_buffer[offset - 625] == 155) &&
           state == 2)
      {
//         Erase_Sprite((sprite_ptr)&pacman);
         pacman.y--;
         temp_y--;
//         Draw_Sprite((sprite_ptr)&pacman);
         state = 0;
         disp = 0;
         if (SB_Enable && !spi[0])
            SBPlayEMS((emsample_ptr)&hitroof, hitroof.length);
      }
      // changed
      if ((video_buffer[offset - 960] == 155 || video_buffer[offset - 945] == 155) &&
           state == 2)
      {
//         Erase_Sprite((sprite_ptr)&pacman);
         pacman.y -= 2;
         temp_y -= 2;
//         Draw_Sprite((sprite_ptr)&pacman);
         state = 0;
         disp = 0;
         if (SB_Enable && !spi[0])
            SBPlayEMS((emsample_ptr)&hitroof, hitroof.length);
      }
      if (key_table[2] && pacman.x < 305 && key_table[3] == 0)
      {
         if (video_buffer[offset + 16] == 0 && video_buffer[offset + 4816] == 0)
	 {
//            if (state == 0 && video_buffer[offset + 4814] != 0)
//               goto Next1;
//            if (state == 2 && video_buffer[offset + 14] != 0)
//               goto Next1;
	    pacman.curr_frame = counter;         /* Right Movement */
//            Erase_Sprite((sprite_ptr)&pacman);
	    pacman.x+= 2;
//            Draw_Sprite((sprite_ptr)&pacman);
	    tctr = 0;
	 }
      }
//      Next1:      
      if (key_table[3] && pacman.x > -2 && key_table[2] == 0)
      {
         if (video_buffer[offset - 1] == 0 && video_buffer[offset + 4799] == 0)
	 {
//            if (state == 0 && video_buffer[offset + 5119] != 0)
//               goto Next2;
//            if (state == 2 && video_buffer[offset - 321] != 0)
//               goto Next2;
	    pacman.curr_frame = counter + 8;     /* Left Movement */
//            Erase_Sprite((sprite_ptr)&pacman);
	    pacman.x-= 2;
//            Draw_Sprite((sprite_ptr)&pacman);
	    tctr = 0;
	 }
      }
//      Next2:
      // changed
      if (((video_buffer[offset + 5120] != 155 && 
            video_buffer[offset + 5135] != 155) &&   
           (video_buffer[offset + 5440] != 155 &&
            video_buffer[offset + 5455] != 155) &&
           (video_buffer[offset + 5760] != 155 &&
            video_buffer[offset + 5775] != 155)) &&
	   (state == 0 || state == 1)) 
      {
//         Erase_Sprite((sprite_ptr)&pacman);      /* Falling */
	 temp_y += disp;
	 pacman.y = (int)temp_y;
//         Draw_Sprite((sprite_ptr)&pacman);
         if (disp < 3) disp += 0.15;
	 state = 0;
      }
      if ((video_buffer[offset + 6080] == 155 && state == 1 && ((pacman.y + 19) % 16) == 0) ||
          (video_buffer[offset + 6095] == 155 && state == 1 && ((pacman.y + 19) % 16) == 0))
      {
         state = 1;                           /* if PAC is 3 pixels above */
         disp = 3;                            /* a block, move him down  */
//         Erase_Sprite((sprite_ptr)&pacman);   /* towards it...           */
         pacman.y += 3;
         temp_y += 3;
//         Draw_Sprite((sprite_ptr)&pacman);
      }
      if ((video_buffer[offset + 5440] == 155 && state == 1 && ((pacman.y + 17) % 16) == 0) ||
          (video_buffer[offset + 5455] == 155 && state == 1 && ((pacman.y + 17) % 16) == 0))
      {
	 state = 1;                           /* if PAC is 1 pixel above */
         disp = 3;                            /* a block, move him down  */
//         Erase_Sprite((sprite_ptr)&pacman);   /* towards it...           */
	 pacman.y++;
         temp_y++;
//         Draw_Sprite((sprite_ptr)&pacman);
      }
      if ((video_buffer[offset + 5760] == 155 && state == 1 && ((pacman.y + 18) % 16) == 0) ||
          (video_buffer[offset + 5775] == 155 && state == 1 && ((pacman.y + 18) % 16) == 0))
      {
         state = 1;                           /* if PAC is 2 pixels above */
         disp = 3;                            /* a block, move him down  */
//         Erase_Sprite((sprite_ptr)&pacman);   /* towards it...           */
         pacman.y += 2;
         temp_y += 2;
//         Draw_Sprite((sprite_ptr)&pacman);
      }

      //changed
      if ((video_buffer[offset + 5120] == 155 && state == 0) ||
          (video_buffer[offset + 5135] == 155 && state == 0))
      {
         state = 1;                              /* Landing from Fall */
         disp = 3.15;
      }
      //changed
      if ((video_buffer[offset + 5440] == 155 && state == 0) ||
          (video_buffer[offset + 5455] == 155 && state == 0))
      {
         state = 1;
         disp = 3.15;
      }
      //changed
      if ((video_buffer[offset + 5760] == 155 && state == 0) ||
          (video_buffer[offset + 5775] == 155 && state == 0))
      {
         state = 1;
         disp = 3.15;
      }
      // NEW
      // changed
      if (key_table[7] == 1 && state == 1 && video_buffer[offset - 319] != 155 &&
          video_buffer[offset - 305] != 155)
      {
            state = 2;                              /* Jumping Start */
      }
      if (state == 2)
      {
//         Erase_Sprite((sprite_ptr)&pacman);      /* Jumping Manipulation */
	 temp_y -= disp;
	 pacman.y = (int)temp_y;
//         Draw_Sprite((sprite_ptr)&pacman);
         if (disp > 0) disp -= 0.15;
	 else state = 0;
      }
      blk_offx = pacman.x - (pacman.x % 16);
      blk_offy = pacman.y - (pacman.y % 16);
      scrn_num  = (((screen_y << 3) + (screen_y << 1)) + screen_x) * 220;
      offset = scrn_num + (blk_offy + (blk_offy >> 2) + (blk_offx >> 4));
      if (level_store[offset] < 47)
      {
         pacman.x = blk_offx + 16;
      }
      blk_offx += 16;
      offset = scrn_num + (blk_offy + (blk_offy >> 2) + (blk_offx >> 4));
      if (level_store[offset] < 47)
      {
         pacman.x = blk_offx - 16;
      }
      Draw_Sprite((sprite_ptr)&pacman);
      if (pacman.x >= 304 && screen_x < 9 && key_table[2])
      {
         memset(video_buffer, 0, 56640);
         pacman.x = 0;
         screen_x++;
         Screen_Logic();
         Draw_Screen(screen_x, screen_y);
         Draw_Sprite((sprite_ptr)&pacman);
      }
      if (pacman.x <= 0 && screen_x > 0 && key_table[3])
      {
         memset(video_buffer, 0, 56640);
         pacman.x = 304;
         screen_x--;
         Screen_Logic();
         Draw_Screen(screen_x, screen_y);
         Draw_Sprite((sprite_ptr)&pacman);
      }
      if (pacman.y >= 159 && screen_y < 9 && state == 0)
      {
         memset(video_buffer, 0, 56960);
//         pacman.y -= 160;
//         temp_y -= 160;
         pacman.y = 0;
         temp_y = 0;
         screen_y++;
         Screen_Logic();
         Draw_Screen(screen_x, screen_y);
         Draw_Sprite((sprite_ptr)&pacman);
      }
      if (pacman.y <= 0 && screen_y > 0 && state == 2)
      {
         memset(video_buffer, 0, 56640);
         pacman.y += 160;
         temp_y += 160;
         screen_y--;
         Screen_Logic();
         Draw_Screen(screen_x, screen_y);
         Draw_Sprite((sprite_ptr)&pacman);
      }
      if (state == 1 && key_table[1])
      {
         while (key_table[1]) { };
         Delete_Keyboard();
         Game_Operation(1);
         Install_Keyboard();
      }
      if (state == 1 && key_table[4])
      {
         while (key_table[4]) { };
         Delete_Keyboard();
         Game_Operation(0);
         Install_Keyboard();
      }
   }
   Free_Memory();

   Delete_Keyboard();
   _dos_setvect(0x1C, Old_Time_Isr);
   Change_Timer(0xFFFF);
   Set_Video_Mode(0x03);
   if (SB_Reset) Release_Blaster();
   exit(1);
}
