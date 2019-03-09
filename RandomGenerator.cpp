
/*
* RandomGenerator.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/28/2018
* Version 1.0
*
* Random generator includes a set of functions that allow
* for random generation of floats or Vector3d objects based
* on either normal or uniform distributions.
*
*/
#include "RandomGenerator.h"
#include "Vector.h"
#include <math.h>

using namespace std;

double gauss(double mean, double std, int seed)
{
   const int itblmax = 20;	// length - 1 of table describing F inverse
   const double didu = 40.0;	// delta table position/delta ind. variable

   // interpolation table for F inverse
   static double tbl[] =
      {0.00000E+00, 6.27500E-02, 1.25641E-01, 1.89000E-01,
       2.53333E-01, 3.18684E-01, 3.85405E-01, 4.53889E-01,
       5.24412E-01, 5.97647E-01, 6.74375E-01, 7.55333E-01,
       8.41482E-01, 9.34615E-01, 1.03652E+00, 1.15048E+00,
       1.28167E+00, 1.43933E+00, 1.64500E+00, 1.96000E+00,
       3.87000E+00};
  
   static int first_time = 1;

   double u;
   double di;
   int index, minus;
   double delta, gaussian_random_value;

   if (first_time){
#ifdef WIN32
      srand((unsigned)seed);
#else
      srand48(seed);
#endif
      first_time = 0;
  }

   // compute uniform random number between 0.0 - 0.5, and a sign with 
   // probability 1/2 of being either + or -
   
#ifdef WIN32
   int rn = rand();
   u = double(rn) / double(RAND_MAX);
#else
   u = drand48();
#endif
   if (u >= 0.5){
      minus = 0;
      u = u - 0.5;
   }
   else
      minus = 1;

  // interpolate gaussian random number using table

   index = (int)(di = (didu * u));
   if(index == itblmax)
      delta = tbl[index];
   else{
      di -= index;
      delta =  tbl[index] + (tbl[index + 1] - tbl[index]) * di;
    }
   gaussian_random_value = mean + std * (minus? -delta : delta);

   return gaussian_random_value;
}

//-----------------------------------------------------------------
/*
uniform(double min, double max)
* PURPOSE : Generate a random real number within a range of
*           numbers that can be interpreted as a sample of a 
*           uniform distribution of numbers.
* INPUTS :  double min, minimum value possible in range of random numbers
*           double max, maximum value possible in range of random numbers
* OUTPUTS : double, real, random number between min and max
*/
//-----------------------------------------------------------------

double uniform(double min, double max)
{
   double r = ((max - min) * drand48() + min);	// drand48() returns value between 0 and 1
   return r;
}

//-----------------------------------------------------------------
/*
randSphereVec()
* PURPOSE : Generate a random directional vector off of the surface
*           of a given sphere. Called to assign random direction 
*           for particles with a omnidirectional or spherical 
*           ParticleGenerator
* INPUTS :  NONE
* OUTPUTS : Vector3d, radial directional vector
*/
//-----------------------------------------------------------------

Vector3d randSphereVec()
{
   double theta = uniform(-PI, PI);	// azimuth angle
   double y = uniform(-1.0, 1.0); 	// height

   float r = sqrt(1 - pow(y,2));
   Vector3d v;
   v.set((r * cos(theta)), y, (-r * sin(theta)));

   return v;
}

//-----------------------------------------------------------------
/*
generateVelocity(float speed_avg, float speed_range)
* PURPOSE : Generate a random Vector3d object to represent
*           velocity given speed parameters and using
*           randomized omnidirectional vectors.
* INPUTS :  float speed_avg, average speed in distribution
*           float speed_range, standard deviation
* OUTPUTS : Vector3d velocity
*/
//-----------------------------------------------------------------

Vector3d generateVelocity(float speed_avg, float speed_range)
{
   float s = gauss(speed_avg, speed_range/3, 4);
   Vector3d u = randSphereVec();
   Vector3d velocity = fabs(s) * u;
   return velocity;
}

//-----------------------------------------------------------------
/*
generatePosition(Vector3d position_avg, Vector3d position_range)
* PURPOSE : Generate random (x,y,z) position coordinates given
*           a position average and distribution range.
* INPUTS :  Vector3d position_avg, average position in space
*           Vector3d position_range, standard deviation for position
* OUTPUTS : Vector3d position
*/
//-----------------------------------------------------------------

Vector3d generatePosition(Vector3d position_avg, Vector3d position_range)
{
   Vector3d position;
   float x = gauss(position_avg.x, position_range.x, 1);
   float y = gauss(position_avg.y, position_range.y, 2);
   float z = gauss(position_avg.z, position_range.z, 3);

   position.set(x, y, z);
   return position;
}

//-----------------------------------------------------------------
/*
generateMass(float mass_avg, float mass_range)
* PURPOSE : Generate random mass given
*           an average and distribution range.
* INPUTS :  float mass_avg, average mass
            float mass_range, standard deviation of mass
* OUTPUTS : float mass
*/
//-----------------------------------------------------------------

float generateMass(float mass_avg, float mass_range)
{
   float mass = gauss(mass_avg, mass_range, 5);
   return mass;
}

