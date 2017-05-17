#include "shapes.inc"
#include "metals.inc"
#include "colors.inc"
#include "finish.inc"

camera { location <0, 0, -10> look_at <0, 0, 0> }
light_source { <3.5, 3.5, -7> color White }
background { color <1.0, 1.0, 1.0> }

#declare btexture = texture { T_Chrome_4B }
#declare Base_Build =
union
{
   cylinder
   {
      <-2, 0, 0> <2, 0, 0>, 0.3
      texture { btexture }
      scale <1, 1, 0.2>
   }
   sphere
   {
      <-2, 0, 0>, 0.3
      texture { btexture }
      scale <1, 1, 0.2>
   }
   sphere
   {
      <2, 0, 0>, 0.3
      texture { btexture }
      scale <1, 1, 0.2>
   }
   difference
   {
      torus
      {
         0.33, 0.03
         texture { pigment { LightGrey } finish { Dull } }
         rotate <90, 0, 0>
         translate <-2, 0, 0>
      }
      box
      {
         <-2, 0.4, -0.4>, <-1.6, -0.4, 0.4>
         texture { pigment { LightGrey } finish { Dull } }
      }
   }
   difference
   {
      torus
      {
         0.33, 0.03
         texture { pigment { LightGrey } finish { Dull } }
         rotate <90, 0, 0>
         translate <2, 0, 0>
      }
      box
      {
         <2, 0.4, -0.4>, <1.6, -0.4, 0.4>
         texture { pigment { LightGrey } finish { Dull } }
      }
   }
   cylinder
   {
      <-2, 0.33, 0>, <2, 0.33, 0>, 0.03
      texture { pigment { LightGrey } finish { Dull } }
   }
   cylinder
   {
      <-2, -0.33, 0>, <2, -0.33, 0>, 0.03
      texture { pigment { LightGrey } finish { Dull } }
   }
}

difference
{
   object { Base_Build translate <0, 0, 0> scale <2, 0.5, 0> }
   union
   {
   // 1.82
      box { <-2.83, 0.15, -0.4>, <-2.73, -0.15, 0> texture { btexture } }
      box { <-1.01, 0.15, -0.4>, <-0.91, -0.15, 0> texture { btexture } }
      box { <+1.01, 0.15, -0.4>, <+0.91, -0.15, 0> texture { btexture } }
      box { <+2.83, 0.15, -0.4>, <+2.73, -0.15, 0> texture { btexture } }
   }
}
light_source { <-1.92, 0, -0.15> color White }
