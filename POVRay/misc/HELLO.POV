#include "colors.inc"

camera {
   location <-5, .15, -2>
   look_at <.3,.2,1>
   angle 36
  }

  light_source { <500,500,-1000> White }

  plane { y,0
    texture {
      pigment { SeaGreen }
      finish { reflection .35 specular 1 }
      normal { ripples .35 turbulence .5 scale .25 }
    }
  }

  text { ttf "timrom.ttf" "Team CHAOS" 25, 0.1*y
    pigment { Black }
    finish { reflection .25 specular 1 }
    translate -3*x
  }

  #include "skies.inc"

  sky_sphere { S_Cloud5 }
