#include "colors.inc"
#include "shapes.inc"
#include "finish.inc"

camera { location <0, 0, -10> look_at <0, 0, 0> }
light_source { <0, 2, -500> color White }

sphere { <0, 0, 0> 4 scale <1.4, 1, 1> pigment { White } normal { dents 0.8} }
