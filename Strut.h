/*
* Strut.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: October 8, 2018
* Version 1.0
*/

#ifndef __STRUT_H__
#define __STRUT_H__

#include "Vector.h"
#include "Particle.h"

class Strut{				// Particle attributes are made public for benefit of the View
	public:
            float k;
            float d;
            float l_rest;
                
            int v_indices[2];
            Strut();					// Default constructor
            Strut(float k_const, float d_const, float l);
            
            void connectVerts(int p1_i, int p2_i);
            void computeVertForces(Particle* particles);
            void setLRest(float lrest);
};

#endif
