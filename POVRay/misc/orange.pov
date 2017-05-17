#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -5> look_at <0, 0, 0> }
light_source { <2, 2, -5> color White }

sphere { < 0, 0, 0>, 2 texture { pigment { Orange } normal { wrinkles bump_size 0.7 turbulence 0.5 } finish { Shiny } } scale <1.5, 1, 1> }
