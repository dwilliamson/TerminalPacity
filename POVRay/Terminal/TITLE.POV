#include "colors.inc"
#include "shapes.inc"
#include "finish.inc"

#include "pacity.inc"
#include "terminal.inc"

camera { location <17, 1, -60> look_at <17, 1, 0> }
light_source { <17, 5, -10> color White atmosphere off }
light_source { <500,500,-1000> color White atmosphere off }
light_source {<17, 2, -5> White fade_distance 0.75 * 0.75 fade_power 1.5 }

#declare Title_Caption =
union
{
   object { Hollow_T translate <-19, 14, 0> }
   object { Hollow_E translate <-9, 14, 0> }
   object { Hollow_R translate <1, 14, 0> }
   object { Hollow_M translate <11, 14, 0> }
   object { Hollow_I translate <21, 14, 0> }
   object { Hollow_N translate <26, 14, 0> }
   object { Hollow_A translate <36, 14, 0> }
   object { Hollow_L translate <46, 14, 0> }

   object { Letter_P translate <-9, 4, 0> }
   object { Letter_A translate <1, 4, 0> }
   object { Letter_C translate <11, 4, 0> }
   object { Letter_I translate <21, 4, 0> }
   object { Letter_T translate <26, 4, 0> }
   object { Letter_Y translate <36, 4, 0> }
}

object { Title_Caption translate <0, 11, 0> }
plane
{
   y, 0
   texture
   {
      pigment { SeaGreen }
      finish { reflection .35 specular 1 }
      normal { ripples .35 turbulence .5 scale .25 }
   }
}

#declare Atmosphere1 =
atmosphere
{
  type 4
  samples 30
  distance 500
  scattering 0.1
  aa_level 8
  aa_threshold 0.1
  jitter 0.1
}

atmosphere { Atmosphere1 }

sphere { <17, 2, -6> 1 pigment { White } normal { dents 0.9 } }

sky_sphere { pigment { image_map { gif "fract004.gif" } } }
