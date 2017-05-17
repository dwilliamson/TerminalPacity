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


do this...

int x, y, button;

SqueezeMouse(MOUSE_BUTT_POS, &x, &y, &button);

now x,y contains it's location and button contains a 1 if the left button is
pressed or a 2 if the right is pressed.
