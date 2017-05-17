#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -5> look_at <0, 0, 0> }
light_source { <2, 2, -10> color White }

#declare Open = 0.4

#declare Pacman =
union
{
   difference
   {
      sphere { <0, 0, 0>, 2.07 pigment { Red } finish { Shiny } scale <1.4, 1, 1> }
      box { <-3, -3, -3> <3, 0, 3> pigment { Red } }
   }
   sphere { <0.4, 0.8,   -2>, 0.3 scale <1.4, 1, 1> pigment { White } finish { Shiny } }
   sphere { <-0.4, 0.8,  -2>, 0.3 scale <1.4, 1, 1> pigment { White } finish { Shiny } }
   sphere { <0.4, 0.8, -2.3>, 0.1 scale <1.4, 1, 1> pigment { Black } }
   sphere { <-0.4, 0.8,-2.3>, 0.1 scale <1.4, 1, 1> pigment { Black } }
   cylinder { <0, 0, 0> <0, -1, 0> 2.07 * 1.3 pigment { Red } finish { Shiny } }
}

object { Pacman translate <0, 0, 0> }
