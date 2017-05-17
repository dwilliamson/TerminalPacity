#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -5> look_at <0, 0, 0> }
light_source { <2, 2, -5> color White }

sphere { < -1, -1.5, 0>, 0.8 pigment { Red } finish { Shiny } scale <1.5, 1, 1> }
sphere { < 1, -1.7, 0>, 0.8 pigment { Red } finish { Shiny } scale <1.5, 1, 1> }
cylinder { <0, 0, 0> <0.1, 1.2, 0>, 0.2 pigment { Green } finish { Shiny } }
cylinder { <0, 0, 0> <-1, -1.5, 0>, 0.2 pigment { Green } finish { Shiny } }
cylinder { <0, 0, 0> <1, -1.7,  0>, 0.2 pigment { Green } finish { Shiny } }
