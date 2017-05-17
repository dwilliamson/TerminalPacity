#include "shapes.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -5> look_at <0, 0, 0> }
light_source { <2, 2, -5> color White }

sphere { < 0, 1, 0>, 1 pigment { White } finish { Shiny } scale <1.5, 1, 1> }
cone { <0, 0.35, 0>, 1, <0, -2, 0>, 0 pigment { Orange } }
