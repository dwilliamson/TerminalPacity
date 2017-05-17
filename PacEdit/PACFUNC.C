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
   video_buffer[((y << 6) + (y << 8)) + x] = color;
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
   
   for (index = 0; index < 64; index++)
      sprite->frames[index] = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/// Free up all memory allocated for a sprite and its frames
//////////////////////////////////////////////////////////////////////////////
void Sprite_Delete(sprite_ptr sprite)
{
   int index;
   
   for (index = 0; index < 64; index++)
      if (sprite->frames[index] != NULL)
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
   offset = ((sprite->y << 6) + (sprite->y << 8)) + sprite->x;
   
   for(y = 0; y < sprite->height; y++)
   {
      for (x = 0; x < sprite->width; x++)
      {
         data = work_sprite[work_offset + x];
         if (data)
            video_buffer[offset + x] = data;
      }
   offset      += 320;
   work_offset += sprite->width;
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
/// Replace any sprite with a colour 0 background
//////////////////////////////////////////////////////////////////////////////
void Erase_Sprite(sprite_ptr sprite)
{
   int offset, y, x;
   
   offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;
   
   for (y = 0; y < sprite->height; y++)
   {
      for (x = 0; x < sprite->width; x++)
      {
         video_buffer[offset + x] = 0;
      }
      offset += 320;
//      work_offset += sprite->width;
   }
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
            video_buffer[offset + x] = col[counter];
            
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
