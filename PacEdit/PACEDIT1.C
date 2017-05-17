/*
PacEdit - The Terminal Pacity Level Editor
Program and all source code is Copyright (c) 1997 Don Williamson

State   Meaning
-----   -------
  0     Map mode
  1     Normal mode
  4     Monster type 2 requester
  5     Change monster attributes
  6     Save episode
  7     Quit requester
*/

//#pragma off(unreferenced)

/***** INCLUDES *************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>

#include "pacman.h"
#include "pacfunc.c"

/***** DEFINES **************************************************************/

#define MOUSE_INT               0x33
#define MOUSE_RESET             0x00
#define MOUSE_SHOW              0x01
#define MOUSE_HIDE              0x02
#define MOUSE_BUTT_POS          0x03
#define MOUSE_SET_SENSITIVITY   0x1A
#define MOUSE_MOTION_REL        0x0B
#define MOUSE_LEFT_BUTTON       0x01
#define MOUSE_RIGHT_BUTTON      0x02
#define MOUSE_MIDDLE_BUTTON     0x04

/***** PROTOTYPES ***********************************************************/

extern void   exit(int __status);
unsigned int DetectMouse(void);

/***** GLOBALS **************************************************************/

unsigned char far *mouseback;
unsigned char far *level_store, *monster_store;
unsigned char far *filename_store;
int screen_x = 0, screen_y = 0, file_ctr = 0, filecnt;
int sprite_count = 0, num_levels = 1, dgrid = 1;
unsigned int num_monsters = 0;
unsigned int pscx = 0, pscy = 0, psbx = 0, psby = 0;
int episode = 1, ftemp, slx; 
unsigned int pecx = 9, pecy = 9, pebx = 19, peby = 10;
char pcxfile[13] = "FLAME.GTF", fchar[2], sfile[13] = "TPE001";
sprite enemy;

/***** FUNCTIONS ************************************************************/

#pragma aux DetectMouse = \
   "mov  ax, 0" \
   "int  33h" \
   value [ax] \
   modify [ax];

int Squeeze_Mouse(int command, int *x, int *y, int *buttons)
{
   union REGS inregs, outregs;

   switch(command)
   {
      case (MOUSE_RESET):
      {
         inregs.x.ax = 0x00;
         int86(MOUSE_INT, &inregs, &outregs);
         *buttons = outregs.x.bx;
         return(outregs.x.ax);
      } break;

      case (MOUSE_SHOW):
      {
         inregs.x.ax = 0x01;
         int86(MOUSE_INT, &inregs, &outregs);
         return(1);
      } break;

      case (MOUSE_HIDE):
      {
         inregs.x.ax = 0x02;
         int86(MOUSE_INT, &inregs, &outregs);
         return(1);
      } break;

      case (MOUSE_BUTT_POS):
      {
         inregs.x.ax = 0x03;
         int86(MOUSE_INT, &inregs, &outregs);
         *x       = outregs.x.cx;
         *y       = outregs.x.dx;
         *buttons = outregs.x.bx;
         return(1);
      } break;

      case (MOUSE_MOTION_REL):
      {
         inregs.x.ax = 0x03;
         int86(MOUSE_INT, &inregs, &outregs);
         *x = outregs.x.cx;
         *y = outregs.x.dx;
         return(1);
      } break;

      case (MOUSE_SET_SENSITIVITY):
      {
         inregs.x.bx = *x;
         inregs.x.cx = *y;
         inregs.x.dx = *buttons;

         inregs.x.ax = 0x1a;
         int86(MOUSE_INT, &inregs, &outregs);
         return(1);
      } break;

      default:break;
   }
}

void Bline(int xo, int yo, int x1, int y1, unsigned char color)
{
   int dx, dy, x_inc, y_inc, error = 0, index;
   unsigned char far *vb_start = video_buffer;

   vb_start = vb_start + ((unsigned int)yo<<6) + 
                         ((unsigned int)yo<<8) +
                         (unsigned int)xo;
   dx = x1 - xo;
   dy = y1 - yo;

   if (dx >= 0)
   {
      x_inc = 1;
   }
   else
   {
      x_inc = -1;
      dx    = -dx;
   }

   if (dy >= 0)
   {
      y_inc = 320;
   }
   else
   {
      y_inc = -320;
      dy    = -dy;
   }

   if (dx > dy)
   {
      for (index = 0; index <= dx; index++)
      {
         *vb_start = color;
         error += dy;
         if (error > dx)
         {
            error -= dx;
            vb_start += y_inc;
         }
         vb_start += x_inc;
      }
   }
   else
   {
      for (index = 0; index <= dy; index++)
      {
         *vb_start = color;
         error += dx;
         if (error > 0)
         {
            error -= dy;
            vb_start += x_inc;
         }
         vb_start += y_inc;
      }
   }
}

void Open_Sprite_PCX(char *pcxname, int pal_ver)
{
   RGB_color color;

   unsigned char head[135], data;
   int count = 0, index; 
   unsigned int width, height;
   int num_bytes;
   FILE *fp;

   if ((fp = fopen(pcxname, "rb")) == NULL)
      Report_Error("Error opening Graphics Template");
   for (index = 0; index < 128; index++)
   {
      head[index] = getc(fp);
   }
   width  = (head[8]  + (head[9]  << 8)) + 1;
   height = (head[10] + (head[11] << 8)) + 1;
   if ((sprites = _fmalloc(width * height)) == _NULLOFF)
      exit(1);
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

void Erase_Arrow(sprite_ptr sprite)
{
   int offset, y, x;
   
   offset = 62080 + sprite->x;
   
   for (y = 0; y < 6; y++)
   {
      for (x = 0; x < 16; x++)
         video_buffer[offset + x] = 0;
      offset += 320;
   }
}

void Draw_Sprite_Clipped(sprite_ptr sprite)
{
   char far *work_sprite;
   int work_offset = 0, offset, x_off, x, y, xs, ys, xe, ye;
   int clip_width, clip_height;
   unsigned char data;

   xs = sprite->x;
   ys = sprite->y;

   xe = xs + sprite->width  - 1;
   ye = ys + sprite->height - 1;

   if ((xs >= 320) || (ys >= 200) || (xs <= (0 - sprite->width)) ||
       (ys <= (0 - sprite->height)))
      return;

   if (xs < 0)
      xs = 0;
   else
   if (xe >= 320)
      xe = 319;

   if (ys < 0)
      ys = 0;
   else
   if (ye >= 200)
      ye = 199;

   clip_width  = xe - xs + 1;
   clip_height = ye - ys + 1;
   work_offset = (sprite->y - ys) * sprite->width;
   x_off = (xs - sprite->x);
   work_sprite = sprite->frames[sprite->curr_frame];
   offset = (ys << 8) + (ys << 6) + xs;

   for (y = 0; y < clip_height; y++)
   {
      for (x = 0; x < clip_width; x++)
      {
         if ((data = work_sprite[work_offset + x + x_off]))
            video_buffer[offset + x + x_off] = data;
      }
      offset      += 320;
      work_offset += sprite->width;
   }
}

void Setup_Screen(sprite_ptr button)
{
   int x, y, bx;

   for (x = 0; x < 320; x += 16)
   {
      for (y = 0; y < 168; y += 16)
      {
         Bline(x, y, x + 15, y, 155);
         Bline(x, y, x, y + 15, 155);
      }
   }
   Bline(0, 176, 319, 176, 155);
   Bline(319, 0, 319, 176, 155);
   button->x = 32;
   button->y = 178;
   button->curr_frame = 0;
   Draw_Sprite(button);
   for (bx = 144; bx < 20 << 4; bx += 16)
   {
      button->x = bx;
      button->curr_frame++;
      Draw_Sprite(button);
   }
   button->x = 0;
   button->curr_frame = 12;
   Draw_Sprite(button);
}

void Mouse_Function(int x, int y)
{
   int mx, my;

   for (mx = 0; mx < 16; mx++)
   {
      for (my = 0; my < 16; my++)
      {
         video_buffer[((y + my) << 8) + ((y + my) << 6) + (x + mx)] = mouseback[(my << 4) + mx];
      }
   }
}

void Draw_Grid(void)
{
   int x, y;

   for (x = 0; x < 320; x += 16)
   {
      for (y = 0; y < 168; y += 16)
      {
         Bline(x, y, x + 15, y, 155);
         Bline(x, y, x, y + 15, 155);
      }
   }
   Bline(0, 176, 319, 176, 155);
   Bline(319, 0, 319, 176, 155);
}

void Mouse_Copy(int x, int y)
{
   int mx, my;

   for (mx = 0; mx < 16; mx++)
   {
      for (my = 0; my < 16; my++)
      {
         mouseback[(my << 4) + mx] = video_buffer[((y + my) << 8) + ((y + my) << 6) + (x + mx)];
      }
   }
}

void Mouse_Setup(void)
{
   union REGS inregs, outregs;

   inregs.x.ax = 0x07;
   inregs.x.cx = 0x00;
   inregs.x.dx = 318;
   int86(0x33, &inregs, &outregs);

   inregs.x.ax = 0x08;
   inregs.x.cx = 0x00;
   inregs.x.dx = 190;
   int86(0x33, &inregs, &outregs);

   inregs.x.ax = 0x04;
   inregs.x.cx = 160;
   inregs.x.dx = 100;
   int86(0x33, &inregs, &outregs);
}

void Display_Sprite_Bar(sprite_ptr sprite)
{
   int x, y;

   sprite->y = 178;
   enemy.y   = 178;
   for (x = 48; x < 144; x++)
   {
      for (y = 178; y < 194; y++)
      {
         Plot_Pixel_Fast(x, y, 0);
      }
   }
   for (x = 0; x < 6; x++)
   {
      sprite->curr_frame = sprite_count + x;
      if (sprite->curr_frame < 61)
      {
         sprite->x = 48 + (x << 4);
         Draw_Sprite(sprite);
      }
      else
      {
         enemy.x          = 48 + (x << 4);
         enemy.curr_frame = (sprite_count + x) - 61;
         Draw_Sprite((sprite_ptr)&enemy);
      }
   }
}

void Draw_Screen(sprite_ptr sprite, int scrnx, int scrny)
{
   int x, y, offset, px, temp;

   offset = ((scrny * 10) + scrnx) * 220;
   for (y = 0; y < 11; y++)
   {
      for (x = 0; x < 20; x++)
      {
         px = (y * 20) + x;
         sprite->x = x << 4;
         sprite->y = y << 4;
         enemy.x   = x << 4;
         enemy.y   = y << 4;
         temp = level_store[offset + px];
         if (temp < 64)
         {
            sprite->curr_frame = temp;
            Draw_Sprite(sprite);
         }
         if (temp > 63)
         {
            enemy.curr_frame = temp - 64;
            Draw_Sprite((sprite_ptr)&enemy);
         }
      }
   }
}

void Print_TPE_Files(void)
{
   int x, y, off;
   char temp[12];

   off = (file_ctr * 12);
   for (y = 0; y < 4; y++)
   {
      if (y < filecnt)
      {
         for (x = 0; x < 12; x++)
            temp[x] = filename_store[x + off];
         temp[x] = ' ';
         Blit_String(26, 26 + (y * 10), 2, temp, 1);
         off += 12;
      }
   }
}

void Print_PCX_Files(void)
{
   int x, y, off;
   char temp[12];

   off = (file_ctr * 12);
   for (y = 0; y < 4; y++)
   {
      if (y < filecnt)
      {
         for (x = 0; x < 12; x++)
            temp[x] = filename_store[x + off];
         temp[x] = ' ';
         Blit_String(26, 26 + (y * 10), 2, temp, 1);
         off += 12;
      }
   }
}

void Load_Episode(char *filename)
{
   int x, y, mod = 0, set = 0;
   unsigned char store[5], temp;
   FILE *fp;

   if ((fp = fopen(filename, "rb")) == NULL)
      Report_Error("Error opening Level");
   fread(store, 5, 1, fp);
   if (store[0] == 'T' && store[1] == 'P' && store[2] == 'E' && store[3] == 'D'
       && store[4] == 'F')
   {
      temp = (fgetc(fp) & 65280) >> 8;
      num_monsters = temp + (fgetc(fp) & 255);
      for (x = 0; x < 20000; x++)
         monster_store[x] = NULL;
      for (x = 0; x < 13; x++)
         pcxfile[x] = 32;
      for (x = 0; x < 8; x++)
      {
         temp = fgetc(fp);
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
      temp = fgetc(fp);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      store[0] = fgetc(fp);
      pscy = store[0] & 15;
      pscx = (store[0] & 240) >> 4;
      temp = fgetc(fp);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      psbx = fgetc(fp);
      psby = fgetc(fp);
      store[0] = fgetc(fp);
      pecy = store[0] & 15;
      pecx = (store[0] & 240) >> 4;
      pebx = fgetc(fp);
      peby = fgetc(fp);
      temp = fgetc(fp);
      if (temp >= 254 || temp < 240) Report_Error("Error opening Level");
      for (x = 0; x < 22000; x++)
         level_store[x] = fgetc(fp);
      for (x = 0; x < num_monsters << 2; x++)
         monster_store[x] = fgetc(fp);
   }
   fclose(fp);
}

void Initiate_Sprites(sprite_ptr mouse, sprite_ptr ctrl_btn,
                      sprite_ptr blocks, sprite_ptr arrow,
                      sprite_ptr loadar)
{
   Sprite_Init(mouse, 160, 160, 0, 0, 0, 0, 16, 16, 1);
   Sprite_Init(ctrl_btn, 0, 176, 0, 0, 0, 0, 16, 16, 13);
   Sprite_Init(blocks, 0, 0, 0, 0, 0, 0, 16, 16, 61);
   Sprite_Init(arrow, 0, 192, 0, 0, 0, 0, 16, 16, 1);
   Sprite_Init(loadar, 0, 0, 0, 0, 0, 0, 16, 16, 5);
   // change1
   Sprite_Init((sprite_ptr)&enemy, 100, 100, 0, 0, 0, 0, 16, 16, 8);
}

void Get_Editor_Graphics(sprite_ptr mouse, sprite_ptr ctrl_btn,
                         sprite_ptr blocks, sprite_ptr arrow,
                         sprite_ptr loadar)
{
   int x;

   Grab_Bitmap(mouse, 0, 0, 5);
   Grab_Bitmap(loadar, 0, 0, 6);
   Grab_Bitmap(loadar, 1, 1, 6);
   Grab_Bitmap(ctrl_btn, 12, 2, 6);
   Grab_Bitmap(loadar, 2, 3, 6);
   Grab_Bitmap(loadar, 3, 4, 6);
   Grab_Bitmap(loadar, 4, 5, 6);
   Grab_Bitmap((sprite_ptr)&enemy, 4, 0, 3);
   Grab_Bitmap((sprite_ptr)&enemy, 5, 11, 2);
   // change2
   for (x = 0; x < 4; x++)
      Grab_Bitmap((sprite_ptr)&enemy, x, x * 3, 7);
   Grab_Bitmap((sprite_ptr)&enemy, 6, 0, 8);
   Grab_Bitmap((sprite_ptr)&enemy, 7, 3, 8);
   for (x = 0; x < 12; x++)
      Grab_Bitmap(ctrl_btn, x, x + 1, 5);
   for (x = 0; x < 18; x++)
      Grab_Bitmap(blocks, x, x, 0);
   for (x = 0; x < 18; x++)
      Grab_Bitmap(blocks, x + 18, x, 1);
   for (x = 0; x < 11; x++)
      Grab_Bitmap(blocks, x + 36, x, 2);
   for (x = 0; x < 14; x++)
      Grab_Bitmap(blocks, x + 47, x, 4);
   Grab_Bitmap(arrow, 0, 13, 5);
   Grab_Bitmap(blocks, 63, 14, 5);
}

void Report_Error(char *error_desc)
{
   Set_Video_Mode(0x03);
   printf("%s", error_desc);
   exit(0);
}

void main(void)
{
   FILE *handle;
   sprite mouse, ctrl_btn, blocks, arrow, loadar;
   int x, y, buttons, num_buttons, prev_x, prev_y, mod, blk_off = 0;
   int offset, scrn_num, mx, my = 0, scrnx, scrny, state = 1, rc, done = 0;
   int domon = 1, mon_num;
   char far filename[13] = "TPE001", mon_x[2], mon_y[2];
   unsigned char far *background;
   struct find_t fileinfo;

   if (!DetectMouse())
   {
      printf("Mouse Driver not detected...");
      exit(0);
   }
   Open_Sprite_PCX(pcxfile, 0);
   Initiate_Sprites((sprite_ptr)&mouse, (sprite_ptr)&ctrl_btn,
                    (sprite_ptr)&blocks, (sprite_ptr)&arrow,
                    (sprite_ptr)&loadar);
   Get_Editor_Graphics((sprite_ptr)&mouse, (sprite_ptr)&ctrl_btn,
                       (sprite_ptr)&blocks, (sprite_ptr)&arrow,
                       (sprite_ptr)&loadar);

   if ((level_store = _fmalloc(22000)) == NULL)
      Report_Error("ERROR: Can't allocate memory for level");
   for (x = 0; x < 22000; x++)
      level_store[x] = 63;
   level_store[0] = 68;
   level_store[21999] = 69;
   mouseback = _fmalloc(256);
   arrow.x = 48;
   arrow.y = 194;
   if ((monster_store = _fmalloc(20000)) == NULL)
      Report_Error("ERROR: Can't allocate memory for type 2 monsters");
   for (x = 0; x < 20000; x++)
      monster_store[x] = NULL;
   slx = 88;
   sfile[(slx / 8) - 5] = '\0';
   Set_Video_Mode(0x13);
   _ffree(sprites);
   Open_Sprite_PCX(pcxfile, 1);
   Setup_Screen((sprite_ptr)&ctrl_btn);
   Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
   _ffree(sprites);
   Squeeze_Mouse(MOUSE_RESET, NULL, NULL, &num_buttons);
   Mouse_Setup();
   Mouse_Copy(160, 100);
   mouse.x = 160;
   mouse.y = 100;
   Draw_Sprite_Clipped((sprite_ptr)&mouse);
   Display_Sprite_Bar((sprite_ptr)&blocks);
   Draw_Sprite_Clipped((sprite_ptr)&arrow);
   while (!done)
   {      
      vblank();
      prev_x = mouse.x;
      prev_y = mouse.y;
      Squeeze_Mouse(MOUSE_BUTT_POS, &x, &y, &buttons);
      mouse.x = x;
      mouse.y = y;
      if (prev_x != x || prev_y != y)
      {
         Mouse_Function(prev_x, prev_y);
         Mouse_Copy(mouse.x, mouse.y);
         Draw_Sprite_Clipped((sprite_ptr)&mouse);
      }
      if (state != 6 && kbhit())
         ftemp = getch();
      if (ftemp == 27 && state != 6)
         done = 1;
      if (buttons == 1)
      {
         ////////////////////
         // Check for exit //
         ////////////////////
         if (mouse.x >= 0 && mouse.x < 16 && mouse.y >= 178 && (state == 1 || state == 0))
         {
            Mouse_Function(mouse.x, mouse.y);
            background = _fmalloc(8000);
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  background[((my - 20) * 140) + (mx - 20)] = video_buffer[(my * 320) + mx];
                  video_buffer[(my * 320) + mx] = 155;
               }
            }
            Bline(20, 20, 159, 20, 193);
            Bline(20, 20, 20, 69, 193);
            Bline(20, 69, 159, 69, 94);
            Bline(159, 20, 159, 69, 94);
            Blit_String(24, 24, 2, "Exit - ???", 1);
            loadar.x = 24;
            loadar.y = 60;
            loadar.curr_frame = 4;
            Draw_Sprite((sprite_ptr)&loadar);
            loadar.x = 148;
            loadar.y = 60;
            loadar.curr_frame = 1;
            Draw_Sprite((sprite_ptr)&loadar);
            Mouse_Function(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            mod = state;
            state = 7;
         }
         /////////////
         // Exit OK //
         /////////////
         if (mouse.x >= 24 && mouse.x < 40 && mouse.y >= 60 && mouse.y < 68
             && state == 7)
         {
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
               }
            }
            _ffree(background);
            done = 1;
         }
         /////////////////////////////////////
         // Scroll sprite bar [LEFT BUTTON] //
         /////////////////////////////////////
         if (mouse.x >= 32 && mouse.x < 48 && mouse.y >= 178 && 
             mouse.y < 192 && sprite_count > 0 && state == 1)
         {
            sprite_count--;
            Mouse_Function(mouse.x, mouse.y);
            Display_Sprite_Bar((sprite_ptr)&blocks);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(50);
         }
         //////////////////////////////////////
         // Scroll sprite bar [RIGHT BUTTON] //
         //////////////////////////////////////
         // change3
         if (mouse.x >= 144 && mouse.x < 160 && mouse.y >= 178 &&
             mouse.y < 192 && sprite_count < 63 && state == 1)
         {
            sprite_count++;
            Mouse_Function(mouse.x, mouse.y);
            Display_Sprite_Bar((sprite_ptr)&blocks);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(50);
         }
         ///////////////
         // New level //
         ///////////////
         if (mouse.x >= 192 && mouse.x < 208 && mouse.y >= 178 &&
             mouse.y < 192 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            pscx = pscy = psbx = psby = 0;
            pecx = pecy = 9;
            pebx = 19;
            peby = 10;
            for (mx = 0; mx < 22000; mx++)
               level_store[mx] = 63;
            level_store[0] = 68;
            level_store[21999] = 69;
            strcpy(sfile, "TPE001");
            num_monsters = 0;
            for (mx = 0; mx < 20000; mx++)
               monster_store[mx] = NULL;
            _fmemset(video_buffer, 0, 56640);
            if(dgrid)
               Draw_Grid();
            Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(200);
         }
         //////////////////
         // Save episode //
         //////////////////
         if (mouse.x >= 176 && mouse.x < 192 && mouse.y >= 178 &&
             mouse.y < 192 && state == 1)
         {
            background = _fmalloc(8000);
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  background[((my - 20) * 140) + (mx - 20)] = video_buffer[(my * 320) + mx];
                  video_buffer[(my * 320) + mx] = 155;
               }
            }
            Bline(20, 20, 159, 20, 193);
            Bline(20, 20, 20, 69, 193);
            Bline(20, 69, 159, 69, 94);
            Bline(159, 20, 159, 69, 94);
            Blit_String(24, 24, 2, "Filename", 1);
            Blit_String(24, 44, 2, "Template", 1);
            for (my = 33; my < 42; my++)
            {
               for (mx = 32; mx < 152; mx++)
               {
                  Plot_Pixel_Fast(mx, my, 0);
                  Plot_Pixel_Fast(mx, my + 20, 0);
               }
            }
            Blit_String(40, 34, 2, sfile, 1);
            Blit_String(40, 54, 2, pcxfile, 1);
            state = 6;
         }
         ////////////////////////////
         // Load episode requester //
         ////////////////////////////
         if (mouse.x >= 160 && mouse.x < 176 && mouse.y >= 178 &&
             mouse.y < 192 && state == 1)
         {
            file_ctr = 0;
            mod = 0;
            rc = _dos_findfirst("*.TPE", _A_NORMAL, &fileinfo);
            while (rc == 0)
            {
               rc = _dos_findnext(&fileinfo);
               mod++;
            }
            filename_store = _fmalloc((mod + 1) * 12);
            _fmemset(filename_store, 0, ((mod + 1) * 12));
            filecnt = mod;
            mod = 0;
            rc = _dos_findfirst("*.TPE", _A_NORMAL, &fileinfo);
            for (mx = 0; mx < strlen(fileinfo.name); mx++)
               filename_store[(mod * 12) + mx] = fileinfo.name[mx];
            while (rc == 0)
            {
               rc = _dos_findnext(&fileinfo);
               mod++;
               for (mx = 0; mx < strlen(fileinfo.name); mx++)
                  filename_store[(mod * 12) + mx] = fileinfo.name[mx];
            }
            background = _fmalloc(7000);
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  background[((my - 20) * 140) + (mx - 20)] = video_buffer[(my * 320) + mx];
                  video_buffer[(my * 320) + mx] = 155;
               }
            }
            Bline(20, 20, 159, 20, 193);
            Bline(20, 20, 20, 69, 193);
            Bline(20, 69, 159, 69, 94);
            Bline(159, 20, 159, 69, 94);
            loadar.x = 140;
            loadar.y = 37;
            loadar.curr_frame = 0;
            Draw_Sprite((sprite_ptr)&loadar);
            loadar.y = 60;
            loadar.curr_frame = 1;
            Draw_Sprite((sprite_ptr)&loadar);
            for (my = 24; my < 66; my++)
            {
               for (mx = 24; mx < 136; mx++)
               {
                  Plot_Pixel_Fast(mx, my, 0);
               }
            }
            Print_TPE_Files();
            state = 2;
         }
         //////////////////////////////////////
         // Load different Graphics Template //
         //////////////////////////////////////
         if (mouse.x >= 224 && mouse.x < 240 && mouse.y >= 178 &&
             mouse.y < 192 && state == 1)
         {
            mod = file_ctr = 0;
            rc = _dos_findfirst("*.GTF", _A_NORMAL, &fileinfo);
            while (rc == 0)
            {
               rc = _dos_findnext(&fileinfo);
               mod++;
            }
            filename_store = _fmalloc((mod + 1) * 12);
            _fmemset(filename_store, 0, ((mod + 1) * 12));
            filecnt = mod;
            mod = 0;
            rc = _dos_findfirst("*.GTF", _A_NORMAL, &fileinfo);
            for (mx = 0; mx < strlen(fileinfo.name); mx++)
               filename_store[(mod * 12) + mx] = fileinfo.name[mx];
            while (rc == 0)
            {
               rc = _dos_findnext(&fileinfo);
               mod++;
               for (mx = 0; mx < strlen(fileinfo.name); mx++)
                  filename_store[(mod * 12) + mx] = fileinfo.name[mx];
            }
            background = _fmalloc(7000);
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  background[((my - 20) * 140) + (mx - 20)] = video_buffer[(my * 320) + mx];
                  video_buffer[(my * 320) + mx] = 155;
               }
            }
            Bline(20, 20, 159, 20, 193);
            Bline(20, 20, 20, 69, 193);
            Bline(20, 69, 159, 69, 94);
            Bline(159, 20, 159, 69, 94);
            loadar.x = 140;
            loadar.y = 37;
            loadar.curr_frame = 0;
            Draw_Sprite((sprite_ptr)&loadar);
            loadar.y = 60;
            loadar.curr_frame = 1;
            Draw_Sprite((sprite_ptr)&loadar);
            for (my = 24; my < 66; my++)
            {
               for (mx = 24; mx < 136; mx++)
               {
                  Plot_Pixel_Fast(mx, my, 0);
               }
            }
            Print_PCX_Files();
            state = 3;
         }
         ///////////////////////////////////////////////////////////////
         // Scroll filelist for episode/template reqeuster [UP ARROW] //
         ///////////////////////////////////////////////////////////////
         if (mouse.x >= 140 && mouse.x < 150 && mouse.y >= 37 &&
             mouse.y < 45 && (state == 2 || state == 3) && file_ctr > 0)
         {
            if (state == 2)
            {
               for (my = 24; my < 66; my++)
               {
                  for (mx = 24; mx < 136; mx++)
                  {
                     Plot_Pixel_Fast(mx, my, 0);
                  }
               }
               file_ctr--;
               Print_TPE_Files();
               delay(200);
            }
            if (state == 3)
            {
               for (my = 24; my < 66; my++)
               {
                  for (mx = 24; mx < 136; mx++)
                  {
                     Plot_Pixel_Fast(mx, my, 0);
                  }
               }
               file_ctr--;
               Print_PCX_Files();
               delay(200);
            }
         }
         /////////////////////////////////////////////////////////////////
         // Scroll filelist for episode/template requester [DOWN ARROW] //
         /////////////////////////////////////////////////////////////////
         if (mouse.x >= 140 && mouse.x < 150 && mouse.y >= 45 &&
             mouse.y < 63 && (state == 2 || state == 3) && file_ctr < filecnt - (file_ctr + (9 - filecnt)))
         {
            if (state == 2)
            {
               for (my = 24; my < 66; my++)
               {
                  for (mx = 24; mx < 136; mx++)
                  {
                     Plot_Pixel_Fast(mx, my, 0);
                  }
               }
               file_ctr++;
               Print_TPE_Files();
               delay(200);
            }
            if (state == 3)
            {
               for (my = 24; my < 66; my++)
               {
                  for (mx = 24; mx < 136; mx++)
                  {
                     Plot_Pixel_Fast(mx, my, 0);
                  }
               }
               file_ctr++;
               Print_PCX_Files();
               delay(200);
            }
         }
         //////////////////////
         // Move up a screen //
         //////////////////////
         if (mouse.x >= 256 && mouse.x < 272 && mouse.y >= 178 &&
             mouse.y < 192 && screen_y > 0 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            _fmemset(video_buffer, 0, 56640);
            if (dgrid)
               Draw_Grid();
            screen_y--;
            Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(100);
         }
         ////////////////////////
         // Move down a screen //
         ////////////////////////
         if (mouse.x >= 272 && mouse.x < 288 && mouse.y >=178 &&
             mouse.y < 192 && screen_y < 9 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            _fmemset(video_buffer, 0, 56640);
            if (dgrid)
               Draw_Grid();
            screen_y++;
            Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(100);
         }
         ////////////////////////
         // Move left a screen //
         ////////////////////////
         if (mouse.x >= 288 && mouse.x < 304 && mouse.y >= 178 &&
             mouse.y < 192 && screen_x > 0 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            _fmemset(video_buffer, 0, 56640);
            if (dgrid)
               Draw_Grid();
            screen_x--;
            Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(100);
         }
         /////////////////////////
         // Move right a screen //
         /////////////////////////
         if (mouse.x >= 304 && mouse.x < 320 && mouse.y >= 178 &&
             mouse.y < 192 && screen_x < 9 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            _fmemset(video_buffer, 0, 56640);
            if (dgrid)
               Draw_Grid();
            screen_x++;
            Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(100);
         }
         ////////////////////////////////////
         // Enter/exit fullscreen map-mode //
         ////////////////////////////////////
         if (mouse.x >= 240 && mouse.x < 256 && mouse.y >= 178 &&
             mouse.y < 192 && (state == 1 || state == 0))
         {
            if (state == 1)
            {
               Mouse_Function(mouse.x, mouse.y);
               mod = 0;
               Mouse_Function(mouse.x, mouse.y);
               _fmemset(video_buffer, 0, 56640);
               for (scrny = 0; scrny < 10; scrny++)
               {
                  for (scrnx = 0; scrnx < 10; scrnx++)
                  {
                     for (my = 0; my < 11; my++)
                     {
                        for (mx = 0; mx < 20; mx++)
                        {
                           offset = ((scrny * 11) * 320) + (scrnx * 20) + (my * 320) + mx;
                           if (level_store[mod] < 47)
                              video_buffer[offset] = 155;
                           if (level_store[mod] >= 47 && level_store[mod] < 53)
                              video_buffer[offset] = 219;
                           if (level_store[mod] >= 53 && level_store[mod] < 59)
                              video_buffer[offset] = 155;
                           if (level_store[mod] == 59)
                              video_buffer[offset] = 128;
                           if (level_store[mod] == 60)
                              video_buffer[offset] = 217;
                           if (level_store[mod] > 63 && level_store[mod] < 68)
                              video_buffer[offset] = 11;
                           if (level_store[mod] > 67)
                              video_buffer[offset] = 212;
                           mod++;
                        }
                     }
                  }
               }
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               state = 0;
               delay(300);
               goto Jumpit;
            }
            if (!state)
            {
               Mouse_Function(mouse.x, mouse.y);
               _fmemset(video_buffer, 0, 56640);
               if (dgrid)
                  Draw_Grid();
               Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               state = 1;
               delay(300);
            }
            Jumpit:;
         }
         ////////////////////////////////////
         // Place a sprite block on-screen //
         ////////////////////////////////////
         if (mouse.y < 176 && state == 1)
         {
            if (sprite_count + blk_off < 61)
            {
               mod = mouse.x % 16;
               blocks.x = mouse.x - mod;
               mod = mouse.y % 16;
               blocks.y = mouse.y - mod;
               blocks.curr_frame = sprite_count + blk_off;
               scrn_num = (screen_y * 10) + screen_x;
               offset = scrn_num * 220;
               offset += (((blocks.y / 16) * 20) + (blocks.x / 16));
               if (level_store[offset] != 65 && level_store[offset] < 68)
               {
                  level_store[offset] = (char)(sprite_count + blk_off);
                  Mouse_Function(mouse.x, mouse.y);
                  Erase_Sprite((sprite_ptr)&blocks);
                  Draw_Sprite((sprite_ptr)&blocks);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
               }
            }
            if (sprite_count + blk_off > 60)
            {
               mod = mouse.x % 16;
               enemy.x = mouse.x - mod;
               mod = mouse.y % 16;
               enemy.y = mouse.y - mod;
               enemy.curr_frame = (sprite_count + blk_off) - 61;
               scrn_num = (screen_y * 10) + screen_x;
               offset = scrn_num * 220;
               offset += (((enemy.y / 16) * 20) + (enemy.x / 16));
               if (level_store[offset] != 65 && level_store[offset] < 68)
               {
                  Mouse_Function(mouse.x, mouse.y);
                  level_store[offset] = (char)(64 + enemy.curr_frame);
                  Erase_Sprite((sprite_ptr)&enemy);
                  Draw_Sprite((sprite_ptr)&enemy);
                  if (enemy.curr_frame == 1)
                  {
                     domon = 1;
                     for (mx = 0; mx < num_monsters << 2; mx += 4)
                        if ((monster_store[mx] == (offset & 255)) &&
                            (monster_store[mx + 1] == (offset & 65280) >> 8))
                            domon = 0;
                     if (domon)
                     {
                        mod = num_monsters << 2;
                        monster_store[mod]     = offset & 255;
                        monster_store[mod + 1] = (offset & 65280) >> 8;
                        monster_store[mod + 2] = 0;
                        monster_store[mod + 3] = 0;
                        num_monsters++;
                     }
                  }
                  if (enemy.curr_frame == 4)
                  {
                     domon = offset;
                     scrn_num = (pscy * 10) + pscx;
                     offset = scrn_num * 220;
                     offset += ((psby * 20) + psbx);
                     level_store[offset] = 63;
                     if (screen_x == pscx && screen_y == pscy && (psbx != (enemy.x / 16) || psby != (enemy.y / 16)))
                     {
                        enemy.x = psbx * 16;
                        enemy.y = psby * 16;
                        Erase_Sprite((sprite_ptr)&enemy);
                        if (dgrid)
                        {
                           Bline(enemy.x, enemy.y, enemy.x + 15, enemy.y, 155);
                           Bline(enemy.x, enemy.y, enemy.x, enemy.y + 15, 155);
                        }
                     }
                     pscx = screen_x;
                     pscy = screen_y;
                     mod = domon - (((pscy * 10) + pscx) * 220);
                     level_store[domon] = 68;
                     domon = mod % 20;
                     psbx = domon;
                     psby = (mod - domon) / 20;
                  }
                  if (enemy.curr_frame == 5)
                  {
                     domon = offset;
                     scrn_num = (pecy * 10) + pecx;
                     offset = scrn_num * 220;
                     offset += ((peby * 20) + pebx);
                     level_store[offset] = 63;
                     if (screen_x == pecx && screen_y == pecy && (pebx != (enemy.x / 16) || peby != (enemy.y / 16)))
                     {
                        enemy.x = pebx * 16;
                        enemy.y = peby * 16;
                        Erase_Sprite((sprite_ptr)&enemy);
                        if (dgrid)
                        {
                           Bline(enemy.x, enemy.y, enemy.x + 15, enemy.y, 155);
                           Bline(enemy.x, enemy.y, enemy.x, enemy.y + 15, 155);
                        }
                     }
                     pecx = screen_x;
                     pecy = screen_y;
                     mod = domon - (((pecy * 10) + pecx) * 220);
                     level_store[domon] = 69;
                     domon = mod % 20;
                     pebx = domon;
                     peby = (mod - domon) / 20;
                  }
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
               }
            }
         }
         if (mouse.x >= 48 && mouse.x < 144 && mouse.y > 177 && state == 1)
         {
            mod = ((mouse.x - (mouse.x % 16)) / 16) - 3;
            blk_off = mod;
            Mouse_Function(mouse.x, mouse.y);
            Erase_Arrow((sprite_ptr)&arrow);
            arrow.x = mouse.x - (mouse.x % 16);
            Draw_Sprite_Clipped((sprite_ptr)&arrow);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
         }
         if (mouse.x >= 140 && mouse.x < 150 && mouse.y >= 60 &&
             mouse.y < 68 && (state == 2 || state == 3))
         {
            if (state == 2)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
                  }
               }
               _ffree(background);
               _ffree(filename_store);
               state = 1;
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               delay(200);
            }
            if (state == 3)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
                  }
               }
               _ffree(background);
               _ffree(filename_store);
               state = 1;
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               delay(200);
            }
         }
//!@#
         if (mouse.x >= 24 && mouse.x < 136 && mouse.y >= 24 &&
             mouse.y < 66 && (state == 2 || state == 3)) 
         {
            if (state == 2)
            {
               my = 0;
               mod = (((mouse.y - 26) / 9) + file_ctr) * 12;
               for (mx = mod; mx < mod + 12; mx++)
               {
                  filename[my] = filename_store[mx];
                  my++;
               }
               if (mod / 12 < filecnt)
               {
                  Load_Episode(filename);
                  Open_Sprite_PCX(pcxfile, 1);
                  Get_Editor_Graphics((sprite_ptr)&mouse, (sprite_ptr)&ctrl_btn,
                                      (sprite_ptr)&blocks, (sprite_ptr)&arrow,
                                      (sprite_ptr)&loadar);
                  _fmemset(video_buffer, 0, 56640);
                  if (dgrid)
                     Draw_Grid();
                  Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
                  Display_Sprite_Bar((sprite_ptr)&blocks);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  state = 1;
                  _ffree(filename_store);
                  _ffree(sprites);
                  _ffree(background);
                  delay(200);
               }
            }
            if (state == 3)
            {               
               my = 0;
               mod = (((mouse.y - 26) / 9) + file_ctr) * 12;
               for (mx = mod; mx < mod + 12; mx++)
               {
                  filename[my] = filename_store[mx];
                  my++;
               }
               if (mod / 12 < filecnt)
               {
                  Open_Sprite_PCX(filename, 1);
                  strcpy(pcxfile, filename);
                  Get_Editor_Graphics((sprite_ptr)&mouse, (sprite_ptr)&ctrl_btn,
                                      (sprite_ptr)&blocks, (sprite_ptr)&arrow,
                                      (sprite_ptr)&loadar);
                  _fmemset(video_buffer, 0, 56640);
                  if (dgrid)
                     Draw_Grid();
                  Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
                  Display_Sprite_Bar((sprite_ptr)&blocks);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  state = 1;
                  _ffree(filename_store);
                  _ffree(sprites);
                  _ffree(background);
                  delay(200);
               }
            }
         }
         //////////////////////////////////
         // Assess 2nd monster variables //
         //////////////////////////////////
         if (mouse.y >= 26 && mouse.y < 34 && state == 4)
         {
            if (mouse.x >= 24 && mouse.x < 40)
            {
               Mouse_Function(mouse.x, mouse.y);
               level_store[offset] = 63;
               for (mx = 0; mx < num_monsters << 2; mx += 4)
               {
                  if ((monster_store[mx] == (offset & 255)) &&
                      (monster_store[mx + 1] == (offset & 65280) >> 8))
                  {
                     for (my = mx; my < num_monsters << 2; my += 4)
                     {
                        monster_store[my]     = monster_store[my + 4];
                        monster_store[my + 1] = monster_store[my + 5];
                        monster_store[my + 2] = monster_store[my + 6];
                        monster_store[my + 3] = monster_store[my + 7];
                     }
                     goto EndCheck;
                  }
               }
               EndCheck: num_monsters--;
               
               for (my = 20; my < 40; my++)
               {
                  for (mx = 20; mx < 77; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 57) + (mx - 20)];
                  }
               }
               Erase_Sprite((sprite_ptr)&blocks);
               if (dgrid)
               {
                  Bline(blocks.x, blocks.y, blocks.x + 15, blocks.y, 155);
                  Bline(blocks.x, blocks.y, blocks.x, blocks.y + 15, 155);
               }
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               state = 1;
               _ffree(background);
               delay(200);
            }


            if (mouse.x >= 44 && mouse.x < 60)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 40; my++)
               {
                  for (mx = 20; mx < 77; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 57) + (mx - 20)];
                  }
               }
               _ffree(background);
               background = _fmalloc(8000);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     background[((my - 20) * 140) + (mx - 20)] = video_buffer[(my * 320) + mx];
                     video_buffer[(my * 320) + mx] = 155;
                  }
               }
               Bline(20, 20, 159, 20, 193);
               Bline(20, 20, 20, 69, 193);
               Bline(20, 69, 159, 69, 94);
               Bline(159, 20, 159, 69, 94);
               loadar.x = 24;
               loadar.y = 56;
               loadar.curr_frame = 4;
               Draw_Sprite((sprite_ptr)&loadar);
               Blit_String(24, 24, 2, "Left & Right", 1);
               for (my = 42; my < 52; my++)
               {
                  for (mx = 48; mx < 68; mx++)
                  {
                     video_buffer[(my * 320) + mx] = 0;
                     video_buffer[(my * 320) + mx + 50] = 0;
                  }
               }
               loadar.x = 70;
               loadar.y = 39;
               loadar.curr_frame = 0;
               Draw_Sprite((sprite_ptr)&loadar);
               loadar.x = 120;
               Draw_Sprite((sprite_ptr)&loadar);
               _ffree(background);
               loadar.x = 145;
               loadar.y = 56;
               loadar.curr_frame = 1;
               Draw_Sprite((sprite_ptr)&loadar);
               for (mx = 0; mx < num_monsters << 2; mx += 4)
               {
                  if ((monster_store[mx] == (offset & 255)) &&
                      (monster_store[mx + 1] == (offset & 65280) >> 8))
                  {
                     mon_num = mx / 4;
                     sprintf(mon_x, "%i", monster_store[(mon_num << 2) + 2]);
                     sprintf(mon_y, "%i", monster_store[(mon_num << 2) + 3]);
                  }
               }
               Blit_String(50, 44, 2, mon_x, 1);
               Blit_String(100, 44, 2, mon_y, 1);
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               state = 5;
               delay(200);
            }

            if (mouse.x >= 64 && mouse.x < 72)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 40; my++)
               {
                  for (mx = 20; mx < 77; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 57) + (mx - 20)];
                  }
               }
               state = 1;
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               _ffree(background);
               delay(200);
            }
         }
         //////////////////////////////////////
         // Change type 2 monster attributes //
         //////////////////////////////////////
         if (state == 5)
         {
            if (mouse.x >= 70 && mouse.x < 79)
            {
               if (mouse.y >= 39 && mouse.y < 47)
               {
                  Mouse_Function(mouse.x, mouse.y);
                  ftemp = monster_store[(mon_num << 2) + 2];
                  ftemp++;
                  sprintf(mon_x, "%i", ftemp);
                  monster_store[(mon_num << 2) + 2] = ftemp;
                  for (my = 42; my < 52; my++)
                  {
                     for (mx = 48; mx < 68; mx++)
                     {
                        video_buffer[(my * 320) + mx] = 0;
                     }
                  }
                  Blit_String(50, 44, 2, mon_x, 1);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  delay(80);
               }
               if (mouse.y >= 47 && mouse.y < 55 && monster_store[(mon_num << 2) + 2] > 0)
               {
                  Mouse_Function(mouse.x, mouse.y);
                  ftemp = monster_store[(mon_num << 2) + 2];
                  ftemp--;
                  sprintf(mon_x, "%i", ftemp);
                  monster_store[(mon_num << 2) + 2] = ftemp;
                  for (my = 42; my < 52; my++)
                  {
                     for (mx = 48; mx < 68; mx++)
                     {
                        video_buffer[(my * 320) + mx] = 0;
                     }
                  }
                  Blit_String(50, 44, 2, mon_x, 1);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  delay(80);
               }
            }
            if (mouse.x >= 120 && mouse.x < 129)
            {
               if (mouse.y >= 39 && mouse.y < 47)
               {
                  Mouse_Function(mouse.x, mouse.y);
                  ftemp = monster_store[(mon_num << 2) + 3];
                  ftemp++;
                  sprintf(mon_y, "%i", ftemp);
                  monster_store[(mon_num << 2) + 3] = ftemp;
                  for (my = 42; my < 52; my++)
                  {
                     for (mx = 98; mx < 118; mx++)
                     {
                        video_buffer[(my * 320) + mx] = 0;
                     }
                  }
                  Blit_String(100, 44, 2, mon_y, 1);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  delay(80);
               }
               if (mouse.y >= 47 && mouse.y < 55 && monster_store[(mon_num << 2) + 3] > 0)
               {
                  Mouse_Function(mouse.x, mouse.y);
                  ftemp = monster_store[(mon_num << 2) + 3];
                  ftemp--;
                  sprintf(mon_y, "%i", ftemp);
                  monster_store[(mon_num << 2) + 3] = ftemp;
                  for (my = 42; my < 52; my++)
                  {
                     for (mx = 98; mx < 118; mx++)
                     {
                        video_buffer[(my * 320) + mx] = 0;
                     }
                  }
                  Blit_String(100, 44, 2, mon_y, 1);
                  Mouse_Copy(mouse.x, mouse.y);
                  Draw_Sprite_Clipped((sprite_ptr)&mouse);
                  delay(80);
               }
            }
            if (mouse.x >= 24 && mouse.x < 40 && mouse.y >= 56 &&
                mouse.y < 65)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
                  }
               }
               _ffree(background);
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               delay(200);
               state = 1;
            }
         }
         /////////////////
         // Exit Cancel //
         /////////////////
         if (mouse.x >= 148 && mouse.x < 156 && mouse.y >= 60 && mouse.y < 68
             && state == 7)
         {
            Mouse_Function(mouse.x, mouse.y);
            for (my = 20; my < 70; my++)
            {
               for (mx = 20; mx < 160; mx++)
               {
                  video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
               }
            }
            _ffree(background);
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            state = mod;
            delay(200);
         }
         /////////////////////////
         // Alternate grid view //
         /////////////////////////
         if (mouse.x >= 208 && mouse.x < 224 && mouse.y >= 178 &&
             mouse.y < 192 && state == 1)
         {
            Mouse_Function(mouse.x, mouse.y);
            if (dgrid)
            {
               dgrid = 0;
               _fmemset(video_buffer, 0, 56640);
               Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
               goto hehe;
            }
            if (!dgrid)
            {
               dgrid = 1;
               _fmemset(video_buffer, 0, 56640);
               Draw_Grid();
               Draw_Screen((sprite_ptr)&blocks, screen_x, screen_y);
            }
            hehe:;
            Mouse_Copy(mouse.x, mouse.y);
            Draw_Sprite_Clipped((sprite_ptr)&mouse);
            delay(200);
         }

      }
      if (buttons == 2)
      {
         if (mouse.y < 176 && state == 1)
         {
            mod = mouse.x % 16;
            blocks.x = mouse.x - mod;
            mod = mouse.y % 16;
            blocks.y = mouse.y - mod;
            scrn_num = (screen_y * 10) + screen_x;
            offset = scrn_num * 220;
            offset += (((blocks.y / 16) * 20) + (blocks.x / 16));
//            if (level_store[offset] != 65 && level_store[offset] < 68)
            if (level_store[offset] != 65 && level_store[offset] != 68 && level_store[offset] != 69)
            {
               level_store[offset] = 63;
               Mouse_Function(mouse.x, mouse.y);
               Erase_Sprite((sprite_ptr)&blocks);
               if (dgrid)
               {
                  Bline(blocks.x, blocks.y, blocks.x + 15, blocks.y, 155);
                  Bline(blocks.x, blocks.y, blocks.x, blocks.y + 15, 155);
               }
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
            }
            if (level_store[offset] == 65)
            {
               Mouse_Function(mouse.x, mouse.y);
               background = _fmalloc(8000);
               for (my = 20; my < 40; my++)
               {
                  for (mx = 20; mx < 77; mx++)
                  {
                     background[((my - 20) * 57) + (mx - 20)] = video_buffer[(my * 320) + mx];
                     video_buffer[(my * 320) + mx] = 155;
                  }
               }
               Bline(20, 20, 76, 20, 193);
               Bline(20, 20, 20, 39, 193);
               Bline(20, 39, 76, 39, 94);
               Bline(76, 20, 76, 39, 94);
               loadar.x = 24;
               loadar.y = 26;
               loadar.curr_frame = 2;
               Draw_Sprite((sprite_ptr)&loadar);
               loadar.x = 44;
               loadar.curr_frame++;
               Draw_Sprite((sprite_ptr)&loadar);
               loadar.x = 64;
               loadar.curr_frame = 1;
               Draw_Sprite((sprite_ptr)&loadar);
               state = 4;
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
            }
         }
      }
      //////////////////////////////////////////////
      // Enter episode filename in save requester //
      //////////////////////////////////////////////
      if (state == 6)
      {
         if (kbhit())
         {
            ftemp = getch();
            if (isalnum(ftemp) && slx < 104)
            {
               Mouse_Function(mouse.x, mouse.y);
               sprintf(fchar, "%c", ftemp);
               for (my = 34; my < 42; my++)
               {
                  for (mx = slx; mx < slx + 8; mx++)
                  {
                     video_buffer[(320 * my) + mx] = 0;
                  }
               }
               fchar[0] = toupper(fchar[0]);
               Blit_String(slx, 34, 2, fchar, 1);
               sfile[(slx / 8) - 5] = fchar[0];
               sfile[(slx / 8) - 4] = '\0';
               slx += 8;
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
            }
            if (ftemp == 8 && slx > 40)
            {
               slx -= 8;
               Mouse_Function(mouse.x, mouse.y);
               for (my = 34; my < 42; my++)
               {
                  for (mx = slx; mx < slx + 8; mx++)
                  {
                     video_buffer[(320 * my) + mx] = 0;
                  }
               }
               sfile[(slx / 8) - 5] = '\0';
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
            }
            if (ftemp == 13 && slx > 40)
            {
               Mouse_Function(mouse.x, mouse.y);
               strcpy(filename, sfile);
               strcat(filename, ".TPE");
               handle = fopen(filename, "wb");
               fprintf(handle, "TPEDF");
               fprintf(handle, "%c", (num_monsters & 65280) >> 8);
               fprintf(handle, "%c", num_monsters & 255);
               mod = 0;
               for (mx = 0; mx < 8; mx++)
               {
                  if (pcxfile[mx] == '.')
                     mod = 1;
                  if (mod == 0)
                     fprintf(handle, "%c", pcxfile[mx]);
                  else
                     fprintf(handle, "%c", 32);
               }
               fprintf(handle, "%c", 240 + (rand()%12));
               mod = pscy + (pscx << 4);
               fprintf(handle, "%c", mod);
               fprintf(handle, "%c", 240 + (rand()%12));
               fprintf(handle, "%c", psbx);
               fprintf(handle, "%c", psby);
               mod = pecy + (pecx << 4);
               fprintf(handle, "%c", mod);
               fprintf(handle, "%c", pebx);
               fprintf(handle, "%c", peby);
               fprintf(handle, "%c", 240 + (rand()%12));
               for (mx = 0; mx < 22000; mx++)
                  fprintf(handle, "%c", level_store[mx]);
               for (mx = 0; mx < num_monsters << 2; mx++)
                  fprintf(handle, "%c", monster_store[mx]);
               fclose(handle);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
                  }
               }
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               _ffree(background);
               state = 1;
            }
            if (ftemp == 27)
            {
               Mouse_Function(mouse.x, mouse.y);
               for (my = 20; my < 70; my++)
               {
                  for (mx = 20; mx < 160; mx++)
                  {
                     video_buffer[(my * 320) + mx] = background[((my - 20) * 140) + (mx - 20)];
                  }
               }
               Mouse_Copy(mouse.x, mouse.y);
               Draw_Sprite_Clipped((sprite_ptr)&mouse);
               state = 1;
               _ffree(background);
               delay(200);
               ftemp = '/0';
            }
         }
      }

   }
   _ffree(sprites);
   _ffree(mouseback);
   _ffree(level_store);
   _ffree(filename_store);
   _ffree(monster_store);
   Sprite_Delete((sprite_ptr)&ctrl_btn);
   Sprite_Delete((sprite_ptr)&blocks);
   Sprite_Delete((sprite_ptr)&arrow);
   Sprite_Delete((sprite_ptr)&loadar);
   Sprite_Delete((sprite_ptr)&enemy);
   Set_Video_Mode(0x03);
   printf("\nPacEdit - Copyright (c) 1997 Don Williamson\n");
}
