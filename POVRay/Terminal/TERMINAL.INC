#declare Hollow_T =
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
}

#declare Hollow_E =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, 0, 0>, <8, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -8, 0>, <8, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -6, 0>, <2, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -2, 0>, <2, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -2, 0>, <2, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -3, 0>, <5, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5, -3, 0>, <5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5, -5, 0>, <2, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -5, 0>, <2, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
}

#declare Hollow_R =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -3, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <3.5, -5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } } 
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
   cylinder { <3.5, -5, 0>, <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <5.5, -5, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
}

#declare Hollow_M =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <4, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <4, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } } 
   sphere { <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -8, 0>, <2, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, 0, 0>, <4, -2, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -2.5, 0>, <4, -4.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <4, -2, 0>, <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <4, -4.5, 0>, <6, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -2.5, 0>, <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -8, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
}

#declare Hollow_I =
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
}

#declare Hollow_N =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, -5.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } } 
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <2, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -8, 0>, <2, -2.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, 0, 0>, <6, -5.5, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -2.5, 0>, <6, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, 0, 0>, <8, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <6, -5.5, 0>, <6, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, 0, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
}

#declare Hollow_A =
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
}

#declare Hollow_L =
union
{
   sphere { <0, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <8, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   sphere { <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }

   cylinder { <0, 0, 0>, <0, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, 0, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <0, -8, 0>, <8, -8, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -8, 0>, <8, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <8, -6, 0>, <2, -6, 0>, 0.4 pigment { Grey } finish { Shiny } }
   cylinder { <2, -6, 0>, <2, 0, 0>, 0.4 pigment { Grey } finish { Shiny } }
}
