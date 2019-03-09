/*
* Particle.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/09/2018
* Version 1.0
*
*/

#include "Strut.h"
#include "Particle.h"
#include "Vector.h"
#include <math.h>
using namespace std;

//-----------------------------------------------------------------
/*
Strut::Strut()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None
*/
//-----------------------------------------------------------------

Strut::Strut()
{
   k = 0;
   d = 0;
   l_rest = 0;
   v_indices[0] = -1; // negative one indicates empty space
   v_indices[1] = -1;
}

Strut::Strut(float k_const, float d_const, float l)
{
   k = k_const;
   d = d_const;
   l_rest = l;

   v_indices[0] = -1;
   v_indices[1] = -1;
}


void Strut::connectVerts(int p1_i, int p2_i)
{
   v_indices[0] = p1_i;
   v_indices[1] = p2_i;
}

void Strut::computeVertForces(Particle* particles)
{
   int i_index = v_indices[0];
   int j_index = v_indices[1];

   Particle i = particles[i_index];
   Particle j = particles[j_index];

   Vector3d x_ij = {(j.position.x - i.position.x), (j.position.y - i.position.y), (j.position.z - i.position.z)};
   float l_ij = sqrt(pow(x_ij.x, 2) + pow(x_ij.y, 2) + pow(x_ij.z, 2));
   Vector3d u_ij = {(x_ij.x/l_ij), (x_ij.y/ l_ij), (x_ij.z/l_ij)};
   float delta_length = l_ij - l_rest;
   
   Vector3d Fs_ij = {(k * delta_length * u_ij.x), (k * delta_length * u_ij.y), (k * delta_length * u_ij.z)};
   Vector3d Fs_ji = {-Fs_ij.x, -Fs_ij.y, -Fs_ij.z};

   Vector3d delta_velocity = {(j.velocity.x - i.velocity.x), (j.velocity.y - i.velocity.y), (j.velocity.z - i.velocity.z)};
   float dot = delta_velocity * u_ij;

   Vector3d Fd_ij = {(d * dot * u_ij.x), (d * dot * u_ij.y), (d * dot * u_ij.z)};
   Vector3d Fd_ji = {-Fd_ij.x, -Fd_ij.y, -Fd_ij.z};

   Vector3d Fi = {(Fs_ij.x + Fd_ij.x), (Fs_ij.y + Fd_ij.y), (Fs_ij.z + Fd_ij.z)};
   Vector3d Fj = {(Fs_ji.x + Fd_ji.x), (Fs_ji.y + Fd_ji.y), (Fs_ji.z + Fd_ji.z)};

   if (particles[i_index].isPinned == true){
      Fi.set(0.0, 0.0, 0.0);
   }

   if (particles[j_index].isPinned == true){
      Fj.set(0.0, 0.0, 0.0);
   }

   particles[i_index].addForce(Fi);
   particles[j_index].addForce(Fj);

}

void Strut::setLRest(float lrest)
{
   l_rest = lrest;
}
