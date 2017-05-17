#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

// The following variables specify the position of the light source for BOTH
// letter objects...

#declare FILL_X = 4
#declare FILL_Y = -4
#declare NOFL_X = 3
#declare NOFL_Y = 3

camera { location <5, 0, -25> look_at <5, 0, 0> }
light_source { <FILL_X, FILL_Y, -10> color White }

sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <4, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <3, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <5, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
sphere { <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <0, 0, 0>, <3, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <2, 0, 0>, <4, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <4, -3, 0>, <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <6, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <8, 0, 0>, <5, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <3, -4.5, 0>, <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <5, -4.5, 0>, <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
cylinder { <5, -8, 0>, <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

// The following is additional - Part 2

sphere { <4, -7, 0> 0.6 pigment { Plum } finish { Shiny } }
sphere { <1.5, -1, 0> 0.6 pigment { Plum } finish { Shiny } }
sphere { <6.5, -1, 0> 0.6 pigment { Plum } finish { Shiny } }
 
cylinder { <4, -7, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
cylinder { <1.5, -1, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
cylinder { <6.5, -1, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
