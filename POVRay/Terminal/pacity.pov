camera { location <17, 0, -50> look_at <17, 0, 0> }
light_source { <17, 0, -10> color White }

#declare Letter_P =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3.5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5.5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
 
   difference
   {
      union
      {
         torus { 2.5, 0.4 rotate <90, 0, 0> translate <5.5, -2.5, 0> pigment { Grey } finish { Shiny } }
         torus { 0.5, 0.4 rotate <90, 0, 0> translate <5.5, -2.5, 0> pigment { Grey } finish { Shiny } }
      }
      box { <0, -8, -1> <5.5, 2, 1> pigment { Grey } finish { Shiny } }
   }
   
   cylinder { <0, 0, 0>, <5.5, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -8, 0>, <2, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -2, 0>, <2, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -2, 0>, <5.5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -3, 0>, <5.5, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -5, 0>, <3.5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5.5, -5, 0>, <3.5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }

   sphere { <1, -1, 0>, 0.6 pigment { Red } finish { Shiny } }
   sphere { <1, -7, 0>, 0.6 pigment { Red } finish { Shiny } }

   cylinder { <1, -1, 0> <1, -7, 0>, 0.6 pigment { Red } finish { Shiny } }
   cylinder { <1, -1, 0> <5.5, -1, 0>, 0.6 pigment { Red } finish { Shiny } }
   cylinder { <1, -4, 0> <5.5, -4, 0>, 0.6 pigment { Red } finish { Shiny } }

   difference
   {
      torus { 1.5, 0.6 rotate <90, 0, 0> translate <5.5, -2.5, 0> pigment { Red } finish { Shiny } }
      box { <0, -8, -1> <5.5, 2, 1> pigment { Red } finish { Shiny } }
   }
}

#declare Letter_A =
union
{
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3.5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <4.5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3, -4, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -4, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2.5, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5.5, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } } 
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <2, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, 0, 0>, <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, 0, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -8, 0>, <2.5, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2.5, -6, 0>, <5.5, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5.5, -6, 0>, <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3, -4, 0>, <5, -4, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3.5, -2, 0>, <3, -4, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3.5, -2, 0>, <4.5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <4.5, -2, 0>, <5, -4, 0>, 0.4 pigment { Grey } finish { Shiny } }

   sphere { <1.25, -7, 0>, 0.6 pigment { Orange } finish { Shiny } }
   sphere { <2.75, -1, 0>, 0.6 pigment { Orange } finish { Shiny } }
   sphere { <6.75, -7, 0>, 0.6 pigment { Orange } finish { Shiny } }
   sphere { <5.25, -1, 0>, 0.6 pigment { Orange } finish { Shiny } }

   cylinder { <1.25, -7, 0>, <2.75, -1, 0>, 0.6 pigment { Orange } finish { Shiny } }
   cylinder { <6.75, -7, 0>, <5.25, -1, 0>, 0.6 pigment { Orange } finish { Shiny } }
   cylinder { <2.75, -1, 0>, <5.25, -1, 0>, 0.6 pigment { Orange } finish { Shiny } }
   cylinder { <1.75, -5, 0>, <6.25, -5, 0>, 0.6 pigment { Orange } finish { Shiny } }
}

#declare Letter_C =
union
{
   difference
   {
      union
      {
         torus { 4, 0.4 rotate <90, 0, 0> translate <4, -4, 0> pigment { Grey } finish { Shiny } }
         torus { 2, 0.4 rotate <90, 0, 0> translate <4, -4, 0> pigment { Grey } finish { Shiny } }
      }
      box { <4, -5, -1> <9, -3, 1> pigment { Grey } finish { Shiny } }
   }
   sphere { <5.8, -3, 0> 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5.8, -5, 0> 0.4 pigment { Grey } finish { Shiny } }
   sphere { <7.8, -3, 0> 0.4 pigment { Grey } finish { Shiny } }
   sphere { <7.8, -5, 0> 0.4 pigment { Grey } finish { Shiny } }
   
   cylinder { <5.8, -3, 0> <7.8, -3, 0> 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5.8, -5, 0> <7.8, -5, 0> 0.4 pigment { Grey } finish { Shiny } }
 
   difference
   {
      torus { 3, 0.6 rotate <90, 0, 0> translate <4, -4, 0> pigment { Yellow } finish { Shiny } }
      box { <4, -5.7, -1> <9, -2.3, 1> pigment { Yellow } finish { Shiny } }
   }
   sphere { <6.45, -2.2, 0> 0.65 pigment { Yellow } finish { Shiny } }
   sphere { <6.45, -5.8, 0> 0.65 pigment { Yellow } finish { Shiny } }
}

#declare Letter_I =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -8, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }

   sphere { <1, -1, 0> 0.6 pigment { Green } finish { Shiny } }
   sphere { <1, -7, 0> 0.6 pigment { Green } finish { Shiny } }
   cylinder { <1, -1, 0> <1, -7, 0> 0.6 pigment { Green } finish { Shiny } }
}

#declare Letter_T =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <0, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, 0, 0>, <8, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -2, 0>, <3, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -2, 0>, <5, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3, -2, 0>, <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5, -2, 0>, <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3, -8, 0>, <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <1, -1, 0> <7, -1, 0>, 0.6 pigment { Blue } finish { Shiny } }
   cylinder { <4, -1, 0> <4, -7, 0>, 0.6 pigment { Blue } finish { Shiny } }

   sphere { <1, -1, 0> 0.6 pigment { Blue } finish { Shiny } }
   sphere { <7, -1, 0> 0.6 pigment { Blue } finish { Shiny } }
   sphere { <4, -7, 0> 0.6 pigment { Blue } finish { Shiny } }
}

#declare Letter_Y =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <4, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <3, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, 0, 0>, <4, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <4, -3, 0>, <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, 0, 0>, <5, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <3, -4.5, 0>, <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5, -4.5, 0>, <5, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5, -8, 0>, <3, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   sphere { <4, -7, 0> 0.6 pigment { Plum } finish { Shiny } }
   sphere { <1.5, -1, 0> 0.6 pigment { Plum } finish { Shiny } }
   sphere { <6.5, -1, 0> 0.6 pigment { Plum } finish { Shiny } }
 
   cylinder { <4, -7, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
   cylinder { <1.5, -1, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
   cylinder { <6.5, -1, 0> <4, -4, 0>, 0.6 pigment { Plum } finish { Shiny } }
}

object { Letter_P translate <-9, 4, 0> }
object { Letter_A translate <1, 4, 0> }
object { Letter_C translate <11, 4, 0> }
object { Letter_I translate <21, 4, 0> }
object { Letter_T translate <26, 4, 0> }
object { Letter_Y translate <36, 4, 0> }
