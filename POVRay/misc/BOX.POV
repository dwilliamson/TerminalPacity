#include "shapes.inc"
#include "colors.inc"
#include "metals.inc"
#include "finish.inc"

camera { location <1.85, -0.3, -7> look_at <1.85, -0.3, 0> }
light_source { <0.5, -0.5, -8> color White }

difference
{
   box
   {
      <-1, -0.67, -1> <4.1, 0.8, 1>
      texture
      {
         pigment
         {
            granite
            turbulence 2
            rotate y*90
            color_map
            {
               [0.02 color rgbf <0.25, 0.25, 0.25, 0.0> ]
               [0.06 color rgbf <0.20, 0.20, 0.20, 0.0> ]
               [0.10 color rgbf <0.10, 0.10, 0.10, 0.0> ]
               [1.00 color rgbf <0.40, 0.40, 0.40, 0.0> ]
            }
         }
         finish { F_MetalA }
      }
   }
   text
   {
      ttf "timrom.ttf" "DonzaNoid" 0.5, 0
      pigment { Yellow }
      finish { reflection .25 specular 1 }
      translate <-0.8, -0.25, -1.25>
   }
}
   
