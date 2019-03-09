/*
* StateVector.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: October 8, 2018
* Version 1.0
*
* Data structure which has capacity to hold the state
* (position and velocity, or their derivatives) of each
* particle in the system. This makes for a more efficient
* integration, particularly in the case of higher order
* Runge Kutta.
*/
#include "StateVector.h"
#include "Vector.h"
#include "Particle.h"
#include <math.h>
#include <iostream>


using namespace std;

//-----------------------------------------------------------------
/*
StateVector::StateVector()
*
* PURPOSE : Default constructor
* INPUTS :  NONE
* OUTPUTS : NONE
*/
//-----------------------------------------------------------------

StateVector::StateVector()
{
   numParticles = 0;
   length = 0;
   states = NULL;
}

//-----------------------------------------------------------------
/*
StateVector::StateVector(int n)
*
* PURPOSE : Variable constructor
* INPUTS :  int n, number of particles in the system
* OUTPUTS : NONE
*/
//-----------------------------------------------------------------

StateVector::StateVector(int n)
{
   numParticles = n;
   length = numParticles * 2;
   states = new Vector3d[length]; // state properties stored in flat array
}

//-----------------------------------------------------------------
/*
StateVector::fillConstant()
*
* PURPOSE : 
* INPUTS : NONE           
* OUTPUTS : NONE 
*/
//-----------------------------------------------------------------
void StateVector::fillConstant(float c)
{
   for (int i = 0; i < length; i++)
   {
      states[i].set(c, c, c);
   }
}

//-----------------------------------------------------------------
/*
void StateVector::copyFromSV(Boid* boids)
*
* PURPOSE : 
* INPUTS : NONE           
* OUTPUTS : NONE 
*/
//-----------------------------------------------------------------

void StateVector::copyFromSV(Particle* particles)
{
   for (int i=0; i < numParticles; i++)
   {
      particles[i].position.set(states[i].x, states[i].y, states[i].z);
      particles[i].velocity.set(states[i + numParticles].x, states[i + numParticles].y, states[i + numParticles].z);
   }
}

//-----------------------------------------------------------------
/*
void StateVector::copyToSV(Boid* boids)
*
* PURPOSE : 
* INPUTS : NONE           
* OUTPUTS : NONE 
*/
//-----------------------------------------------------------------

void StateVector::copyToSV(Particle* particles)
{
   for (int i=0; i < numParticles; i++)
   {
      states[i].set(particles[i].position.x, particles[i].position.y, particles[i].position.z);
   }

   for (int j = numParticles; j < length; j++)
   {
      states[j].set(particles[j-numParticles].velocity.x, particles[j-numParticles].velocity.y, particles[j-numParticles].velocity.z);
   }
}

//-----------------------------------------------------------------
/*
StateVector StateVector::add(StateVector s2)
*
* PURPOSE : 
* INPUTS : NONE           
* OUTPUTS : NONE 
*/
//-----------------------------------------------------------------
StateVector StateVector::add(StateVector s2) // add this to another statevector
{
   StateVector result = StateVector(numParticles);
   result.fillConstant(0.0);
   if (length != s2.getLength())
   {
      std::cerr<< "State vectors must be the same length in order to perform addition." << std::endl;
   }
   else{
      for (int i = 0; i < length; i++)
      {
         Vector3d r = states[i] + s2.states[i];
         result.states[i] = r;
      }
      
      return result;
   }
}
//-----------------------------------------------------------------
/*
StateVector StateVector::mult(float k)
*
* PURPOSE : 
* INPUTS : NONE           
* OUTPUTS : NONE 
*/
//-----------------------------------------------------------------

StateVector StateVector::mult(float k) // multiply this by a scalar
{
   StateVector result = StateVector(numParticles);

   for (int i = 0; i < length; i++)
   {
      Vector3d r = states[i] * k;
      result.states[i] = r;
   }
   
   return result;   
}

//-----------------------------------------------------------------
/*
StateVector::print()
*
* PURPOSE : Print the values of the state vector
* INPUTS : NONE         
* OUTPUTS : NONE
*/
//-----------------------------------------------------------------
void StateVector::print()
{
   for(int j = 0; j < (length); j++)
   {
      std::cout << "index: " << j << std::endl;
      std::cout << "(" << states[j].x << ", " << states[j].y << ", " << states[j].z << ")" << std::endl;
      std::cout << " " << std::endl;
   }

}

