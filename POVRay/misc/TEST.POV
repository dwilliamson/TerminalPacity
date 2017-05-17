#include "colors.inc"   
#include "textures.inc" 
#include "finish.inc"
#include "woods.inc"

camera
{
  location  <0.0 , 2.0 ,-5.0>
  look_at   <0.0 , 0.0 , 0.0>
}

light_source
{
  0*x 
  color rgb 1.0  
  area_light
  <8, 0, 0> <0, 0, 8> 
  4, 4                
  adaptive 0          
  jitter              
  translate <40, 80, -40>   
}

plane
{
  y, -.18
  texture
  {
    pigment {color SeaGreen scale 0.5}
    finish {reflection 0.5}
    normal { ripples .35 turbulence .5 scale .25 }
  }
}

#include "skies.inc"
sky_sphere
{
   S_Cloud5
}


fog
{
  fog_type 1
  distance 100
  color SkyBlue
}

height_field {
  gif "c:\povray3\mine\fract002.gif"
  smooth
  pigment
  {
     gradient y
     color_map
     {
        [0.36 Brown]
        [0.52 color red 0.4 green 0.8 blue 0.2]
        [0.58 Grey]
        [0.60 White]
     }
     turbulence 0.1
  }
  translate <-.5, -.5, -.5>
  scale <17, 1.75, 17>
 }



