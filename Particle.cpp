/*
* Particle.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/09/2018
* Version 1.0
*
*/

#include "Particle.h"
#include "Vector.h"
#include <cmath>

using namespace std;

//-----------------------------------------------------------------
/*
Particle::Particle()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None
*/
//-----------------------------------------------------------------

Particle::Particle()
{	
   position.set(0.0, 0.0, 0.0);
   velocity.set(0.0, 0.0, 0.0);
   acceleration.set(0.0, 0.0, 0.0);
   mass = 1.0;

   force.set(0.0, 0.0, 0.0);
   isPinned = false;

}

//-----------------------------------------------------------------
/*
Particle::Particle(Vector3d x, Vector3d v, float m)
* PURPOSE : Variable constructor
* INPUTS :  Vector3d x, position of particle
            Vector3d v, initial velocity of particle
	    float m, mass of particle
* OUTPUTS : None, initializes a particle
*/
//-----------------------------------------------------------------

Particle::Particle(Vector3d x, Vector3d v, float m)
{
   position.set(x.x, x.y, x.z);
   velocity.set(v.x, v.y, v.z);
   mass = fabs(m);			// Mass must be positive

   force.set(0.0, 0.0, 0.0);
 
   isPinned = false;
}

void Particle::clearForce()
{
   force.set(0.0, 0.0, 0.0);
}

void Particle::addForce(Vector3d f)
{
   force.set(force.x + f.x, force.y + f.y, force.z + f.z);
}


/***DEFINE EXTERNAL FORCES HERE****/

void Particle::computeExtForces()
{
   Vector3d f;
   Vector3d g = {0.2, -0.6, -0.05};
   f.set(mass * g.x, mass * g.y, mass * g.z);
   if (isPinned == true){
      f.set(0.0, 0.0, 0.0);
   }
   addForce(f);
}

void Particle::computeAcceleration()
{
   acceleration.set(force.x/mass, force.y/mass, force.z/mass);  
}

