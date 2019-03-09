/*
* Particle.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: October 8, 2018
* Version 1.0
*/

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Vector.h"

class Particle{				// Particle attributes are made public for benefit of the View
	public:
		Vector3d position;	
		Vector3d velocity;
		Vector3d acceleration;
                Vector3d force;
		float mass;

                bool isPinned;

		Particle();					// Default constructor
		Particle(Vector3d x, Vector3d v, float m);	// Variable Constructor
                
                void clearForce();
                void addForce(Vector3d f);
                void computeExtForces();
                void computeAcceleration();

};

#endif
