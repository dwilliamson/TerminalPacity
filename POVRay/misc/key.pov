#include "colors.inc"
#include "finish.inc"

camera { location <0, -0.2, -10> look_at <0, -0.2, 0> }
light_source { <2, 2, -10> color White }

torus { 2, 0.4 rotate <90, 0, 0> translate <-3, -2.8, 0> pigment { Yellow } finish { Shiny } }
cylinder { <-1.4, -1.2, 0> <3, 2.8, 0> 0.4 pigment { Yellow } finish { Shiny } }
cylinder { <3, 2.8, 0> <4, 1.47, 0> 0.3 pigment { Yellow } finish { Shiny } }
cylinder { <3.4, -1.8, 0> <1, 0.4, 0> 0.3 pigment { Yellow } finish { Shiny } }
