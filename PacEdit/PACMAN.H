/* PACMAN.H */

/* Header file for Terminal PACity - contains #defines, structures, globals
   and prototypes... */

#define PALETTE_MASK        0x3c6
#define PALETTE_REGISTER_RD 0x3c7
#define PALETTE_REGISTER_WR 0x3c8
#define PALETTE_DATA        0x3c9
#define CHAR_HEIGHT 8
#define CHAR_WIDTH  8

typedef struct RGB_color_type
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
} RGB_color, *RGB_color_ptr;

typedef struct sprite_typ
{
   int x, y;
   int x_old, y_old;
   int width, height;

   int anim_clock;
   int anim_speed;
   int motion_clock;
   int motion_speed;

   char far *frames[64];
   int      curr_frame;
   int      num_frames;
   int      state;
} sprite, *sprite_ptr;

unsigned char far *sprites;
unsigned char far *video_buffer = (char far *)0xA0000000L;
unsigned char far *rom_char_set = (char far *)0xF000FA6EL;
unsigned char far *double_buffer;
int key_table[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int raw_key, tvect = 1;
int counter = 0;
int tctr = 0;
unsigned char prev_pal[768];

void (interrupt far *Old_Key_Isr)();
void (interrupt far *Old_Time_Isr)();

//int col[8] = {230, 152, 78, 154, 154, 78, 152, 230};
int col[8] = {7, 8, 9, 10, 11, 12, 13, 13 };
