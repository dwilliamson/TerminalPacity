#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -5> look_at <0, 0, 0> rotate <0, -135, 0> }
light_source { <2, 2, -10> color White rotate <0, -135, 0> }

#declare Open = 0.1

#declare Pacman =
union
{
   difference
   {
      sphere { <0, 0, 0>, 2.07 pigment { Yellow } finish { Shiny } scale <1.4, 1, 1> }
      box { <-2, -2, -2> <2, 2, 2> rotate <0, 0, 45> scale <0.8, Open, 2> translate <2.3, 0, 0> pigment { Red } finish { Shiny }}
   }
   sphere { <0.4, 0.8,   -2>, 0.3 scale <1.4, 1, 1> pigment { White } finish { Shiny } }
   sphere { <0.4, 0.8, -2.3>, 0.1 scale <1.4, 1, 1> pigment { Black } }
   sphere { <0.4, 0.8,    2>, 0.3 scale <1.4, 1, 1> pigment { White } finish { Shiny } }
   sphere { <0.4, 0.8,  2.3>, 0.1 scale <1.4, 1, 1> pigment { Black } }
}

object { Pacman translate <-0.4, 0, -0.4> }
