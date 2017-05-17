#include "colors.inc"
#include "finish.inc"
#include "textures.v2"

camera
{
   location <0, 2, -3>
   look_at  <0, 1, 2>
}

sphere
{
   <0, 1, 2>, 2
   texture
   {
      Red_Marble
      finish { Shiny }
   }
}

light_source { <2, 4, -3> color White }
