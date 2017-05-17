/* PACFUNC.C */

/* File containing functions used by every part of the program Terminal
   PACity... */

/////////////////////////////////////////////////////////////////////////////
/// Does just as it says - sets the video mode by setting the LSB of AX
/// and calls the video interrupt 10h. DO NOT attempt to set SVGA modes
/// with this function.
/////////////////////////////////////////////////////////////////////////////
void Set_Video_Mode(int mode)
{
   union REGS inregs, outregs;

   inregs.h.ah = 0x00;
   inregs.h.al = (unsigned int)mode;
   int86(0x10, &inregs, &outregs);
}

/////////////////////////////////////////////////////////////////////////////
/// Sets a value in the previously allocated video buffer by using
/// bit-shifting. As it takes time to call this function, it is not used
/// often.
/////////////////////////////////////////////////////////////////////////////
void Plot_Pixel_Fast(int x, int y, unsigned int color)
{
   video_buffer[pixtab[y + 40] + x] = color;
}

unsigned char Get_Pixel(int x, int y)
{
   return video_buffer[((y << 6) + (y << 8)) + x];
}
/////////////////////////////////////////////////////////////////////////////
/// Sets the corresponding register on the VGA card to each of its RGB
/// elements. Each element can only be 6 bits in length (0 - 63) giving
/// you a total of 256 colours with 262144 possible values.
/////////////////////////////////////////////////////////////////////////////
void Set_Palette_Register(int index, RGB_color_ptr color)
{
   outp(PALETTE_MASK, 0xff);
   outp(PALETTE_REGISTER_WR, index);

   outp(PALETTE_DATA, color->red);
   outp(PALETTE_DATA, color->green);
   outp(PALETTE_DATA, color->blue);
}

/////////////////////////////////////////////////////////////////////////////
/// This function reads the VGAs colour registers and stores the RGB values
/// of the requested colour in the RGB_color structure.
/////////////////////////////////////////////////////////////////////////////
void Get_Palette_Register(int index, RGB_color_ptr color)
{
   outp(PALETTE_MASK, 0xFF);
   outp(PALETTE_REGISTER_RD, index);

   color->red   = inp(PALETTE_DATA);
   color->green = inp(PALETTE_DATA);
   color->blue  = inp(PALETTE_DATA);
}

/////////////////////////////////////////////////////////////////////////////
/// Function "grabs" a sprite from the far pointer 'sprites'. It takes the
/// width and height from the sprite structure and turns your bitmap into
/// a nice grid - easier to read. WARNING: The .PCX file HAS to be 320 pixels
/// wide!
/////////////////////////////////////////////////////////////////////////////
void Grab_Bitmap(sprite_ptr sprite, int sprite_frame, int grab_x, int grab_y)
{
   int x_off, y_off, x, y;
   char far *sprite_data;

   sprite->frames[sprite_frame] = (char far *)_fmalloc(sprite->width *
                                  sprite->height + 1);
   sprite_data = sprite->frames[sprite_frame];
   x_off = (sprite->width  + 1) * grab_x + 1;
   y_off = (sprite->height + 1) * grab_y + 1;
   y_off = y_off * 320;

   for (y = 0; y < sprite->height; y++)
   {
      for (x = 0; x < sprite->width; x++)
      {
         sprite_data[y * sprite->width + x] = sprites[y_off + x_off + x];
      }
      y_off += 320;
   }
   sprite->num_frames++;
}

/////////////////////////////////////////////////////////////////////////////
/// Fill the Sprite structure with the necessary information
//////////////////////////////////////////////////////////////////////////////
void Sprite_Init(sprite_ptr sprite, int x, int y, int ac, int as, int mc,
                 int ms, int swidth, int sheight, int frames)
{
   int index;
   
   sprite->x            = x;
   sprite->y            = y;
   sprite->x_old        = x;
   sprite->y_old        = y;
   sprite->width        = swidth;
   sprite->height       = sheight;
   sprite->anim_clock   = ac;
   sprite->anim_speed   = as;
   sprite->motion_clock = mc;
   sprite->motion_speed = ms;
   sprite->curr_frame   = 0;
   sprite->state        = 0;
   sprite->num_frames   = frames;
   
   for (index = 0; index < 63; index++)
      sprite->frames[index] = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/// Free up all memory allocated for a sprite and its frames
//////////////////////////////////////////////////////////////////////////////
void Sprite_Delete(sprite_ptr sprite)
{
   int index;
   
   for (index = 0; index < 63; index++)
      _ffree(sprite->frames[index]);
}

//////////////////////////////////////////////////////////////////////////////
/// Draw any given sprite to the video buffer
//////////////////////////////////////////////////////////////////////////////
void Draw_Sprite(sprite_ptr sprite)
{
   char far *work_sprite;
   int work_offset = 0, offset, x, y;
   unsigned char data;
   
   work_sprite = sprite->frames[sprite->curr_frame];
//   offset = ((sprite->y << 6) + (sprite->y << 8)) + sprite->x;
   offset = pixtab[sprite->y + 40] + sprite->x;

   for(y = 0; y < sprite->height; y++)
   {
      for (x = 0; x < sprite->width; x++)
      {
         data = work_sprite[work_offset + x];
         if (data && video_buffer[offset + x] != 155 && video_buffer[offset + x] != 127)
            video_buffer[offset + x] = data;
      }
      offset      += 320;
      work_offset += sprite->width;
   }
}

void Draw_Sprite_Quick(sprite_ptr sprite, int sx, int sy, char frame)
{
   char far *work_sprite;
   int work_offset = 0, offset, x, y;

   work_sprite = sprite->frames[frame];
   offset = pixtab[sy + 40] + sx;

   for (y = 0; y < 16; y++)
   {
      for (x = 0; x < 16; x++)
      {
         video_buffer[offset + x] = work_sprite[work_offset + x];
      }
      offset      += 320;
      work_offset += 16;
   }
}

//////////////////////////////////////////////////////////////////////////////
/// Wait for the vertical retrace ie. for the CRT to
/// return to the top left of the monitor
//////////////////////////////////////////////////////////////////////////////
void vblank(void)
{
   while (inp(0x3da) & 0x08)
      ;
   while (!(inp(0x3da) & 0x08))
      ;
}

//////////////////////////////////////////////////////////////////////////////
/// The heart of the keyboard ISR. Take input straight from the keyboard and
/// fill the keys store with a TRUE if the key is pressed or a FALSE if it
/// is idle
//////////////////////////////////////////////////////////////////////////////
void interrupt New_Key_Int()
{
   unsigned char al;

   al = inp(0x60);
   raw_key = al;
   al = inp(0x61);
   outp(0x61, al);
   al &= 0x7F;
   outp(0x61, al);
   al = 0x20;
   outp(0x20, al);
   switch(raw_key)
   {
      case 1:
      {
         key_table[9] = 1;
      } break;
      case 129:
      {
         key_table[9] = 0;
      } break;
      case 77:
      {
         key_table[2] = 1;
      } break;
      case 205:
      {
         key_table[2] = 0;
      } break;
      case 75:
      {
         key_table[3] = 1;
      } break;
      case 203:
      {
         key_table[3] = 0;
      } break;
      case 29:
      {
         key_table[7] = 1;
      } break;
      case 157:
      {
         key_table[7] = 0;
      } break;
      case 60:
      {
         key_table[1] = 1;
      } break;
      case 188:
      {
         key_table[1] = 0;
      } break;
      case 61:
      {
         key_table[4] = 1;
      } break;
      case 189:
      {
         key_table[4] = 0;
      } break;
      case 63:
      {
         key_table[5] = 1;
      } break;
      case 191:
      {
         key_table[5] = 0;
      } break;

      // Cheat Keys... ALIOTHSF

      case 30:
      {
         key_table[11] = 1;
      } break;
      case 158:
      {
         key_table[11] = 0;
      } break;

      case 38:
      {
         key_table[12] = 1;
      } break;
      case 166:
      {
         key_table[12] = 0;
      } break;

      case 23:
      {
         key_table[13] = 1;
      } break;
      case 151:
      {
         key_table[13] = 0;
      } break;

      case 24:
      {
         key_table[14] = 1;
      } break;
      case 152:
      {
         key_table[14] = 0;
      } break;

      case 20:
      {
         key_table[15] = 1;
      } break;
      case 148:
      {
         key_table[15] = 0;
      } break;

      case 35:
      {
         key_table[16] = 1;
      } break;
      case 163:
      {
         key_table[16] = 0;
      } break;

      case 31:
      {
         key_table[17] = 1;
      } break;
      case 159:
      {
         key_table[17] = 0;
      } break;

      case 33:
      {
         key_table[18] = 1;
      } break;
      case 161:
      {
         key_table[18] = 0;
      } break;
      default: break;
   }
} 
  
//////////////////////////////////////////////////////////////////////////////
/// Take control of the keyboard COMPLETELY - no need to access through BIOS
//////////////////////////////////////////////////////////////////////////////
void Install_Keyboard(void)
{
   char x;

   Old_Key_Isr = _dos_getvect(0x09);
   _dos_setvect(0x09, New_Key_Int);
   for (x = 0; x < 10; x++)
      key_table[x] = 0;
}

//////////////////////////////////////////////////////////////////////////////
/// Removes previously installed keyboard ISR
//////////////////////////////////////////////////////////////////////////////
void Delete_Keyboard(void)
{
   _dos_setvect(0x09, Old_Key_Isr);
}

//////////////////////////////////////////////////////////////////////////////
/// Replace any sprite with a colour 0 background
//////////////////////////////////////////////////////////////////////////////
void Erase_Sprite(sprite_ptr sprite)
{
   int offset, y, x;
   
//   offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;
   offset = pixtab[sprite->y + 40] + sprite->x;

   for (y = 0; y < sprite->height; y++)
   {
      for (x = 0; x < sprite->width; x++)
      {
         if (video_buffer[offset + x] != 155 && video_buffer[offset + x] != 127)
            video_buffer[offset + x] = 0;
      }
      offset += 320;
   }
}

//////////////////////////////////////////////////////////////////////////////
/// Timer ISR which updates variables and then "chains" to the old timer ISR
//////////////////////////////////////////////////////////////////////////////
void interrupt far Timer()
{
   static int firetime = 0;

   if (fire_anim)
   {
      firetime++;
      if (firetime == 12)
      {
         anim_pos++;
         if (anim_pos == 7) anim_pos = 2;
         firetime = 0;
      }
   }
   counter += tvect;
   if (counter == 8)
   {
      counter = 6;
      tvect = -1;
   }
   if (counter == -1)
   {
      counter = 1;
      tvect = 1;
   }
   tctr++;
   if (sprite_dir)
   {
      sprite_ctr++;
      if (sprite_ctr == 18)
         sprite_dir = 0;
   }
   if (!sprite_dir)
   {
      sprite_ctr--;
      if (sprite_ctr == -1)
      {
         sprite_ctr = 0;
         sprite_dir = 1;
      }
   }
   Old_Time_Isr();
}

//////////////////////////////////////////////////////////////////////////////
/// Reprogram the internal timer to run at a given frequency (hz)
//////////////////////////////////////////////////////////////////////////////
void Change_Timer(unsigned int new_hz)
{
   outp(0x43, 0x3C);
   outp(0x40, 0xFF & new_hz);
   outp(0x40, (new_hz >> 8) & 0xFF);
}

//////////////////////////////////////////////////////////////////////////////
/// Take a character in internal memory and 'blit' it to screen.
//////////////////////////////////////////////////////////////////////////////
void Blit_Char(int xc, int yc, char c, int color, int trans_flag)
{
   int offset, x, y;
   char far *work_char;
   unsigned char bit_mask = 0x80;
   int counter = 0;
   
   work_char = rom_char_set + c * CHAR_HEIGHT;
   
   offset = (yc << 8) + (yc << 6) + xc;
   
   for (y = 0; y < CHAR_HEIGHT; y++)
   {
      bit_mask = 0x80;
      
      for (x = 0; x < CHAR_WIDTH; x++)
      {
         if ((*work_char & bit_mask))
            video_buffer[offset + x] = col[color][counter];
            
         else if (!trans_flag)
            video_buffer[offset + x] = 0;
         
            bit_mask = (bit_mask >> 1);
      }
      offset += 320;
      work_char++;
      counter++;
   }
}                 

//////////////////////////////////////////////////////////////////////////////
/// Link a string with character output saving time.
//////////////////////////////////////////////////////////////////////////////
void Blit_String(int x, int y, int color, char *string, int trans_flag)
{
   int index;

   for (index = 0; string[index] !=0; index++)
   {
      Blit_Char(x + (index << 3), y, string[index], color, trans_flag);
   }
}

void Generate_Pixel_Table(void)
{
   unsigned int x;

   for (x = 0; x < 40; x++)
      pixtab[x] = (x - 40) * 320;
   for (x = 0; x < 200; x++)
      pixtab[x + 40] = x * 320;
   for (x = 0; x < 73; x++)
      coltab[x] = 0;
   coltab[73] = 248;
   coltab[74] = 247;
   coltab[75] = 246;
   coltab[76] = 245;
   coltab[77] = 244;
   coltab[78] = 243;
   coltab[79] = 242;
   coltab[80] = 241;
   for (x = 80; x < 113; x++)
      coltab[x] = 240;
   coltab[113] = 241;
   coltab[114] = 242;
   coltab[115] = 243;
   coltab[116] = 244;
   coltab[117] = 245;
   coltab[118] = 246;
   coltab[119] = 247;
   for (x = 120; x < 200; x++)
      coltab[x] = 0;

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

void Blit_Char_Credit(int xc, int yc, char c, int color, int trans_flag)
{
   int offset, x, y;
   char far *work_char;
   unsigned char bit_mask = 0x80;
   
   work_char = rom_char_set + c * CHAR_HEIGHT;
   
   offset = pixtab[yc + 40] + xc;
   
   for (y = 0; y < CHAR_HEIGHT; y++)
   {
      bit_mask = 0x80;

      if (coltab[yc + y])
      {
         for (x = 0; x < CHAR_WIDTH; x++)
         {         
            if ((*work_char & bit_mask))
               video_buffer[offset + x] = coltab[yc + y];
               
            else if (!trans_flag)
               video_buffer[offset + x] = 0;
         
            bit_mask = (bit_mask >> 1);
         }
         offset += 320;
         work_char++;
      }
   }
}                 


void Blit_String_Credit(int x, int y, int color, char *string, int trans_flag)
{
   int index;

   for (index = 0; string[index] !=0; index++)
   {
      Blit_Char_Credit(x + (index << 3), y, string[index], color, trans_flag);
   }
}

void ptext(unsigned char *string, unsigned char x, unsigned char y, unsigned char col)
{
   int mx;

   for (mx = 0; mx < strlen(string); mx++)
   {
      text_buffer[((y * 132) + (x + mx)) << 1] = string[mx];
      text_buffer[(((y * 132) + (x + mx)) << 1) + 1] = col;
   }
}

void pvalue(unsigned char value, unsigned char x, unsigned char y, unsigned char col)
{
   unsigned char text[6] = "\0";

   sprintf(text, "%i", value);
   ptext(text, x, y, col);
}
