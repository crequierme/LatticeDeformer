/*
 Model.h
 CPSC 8170 Physically Based Animation
 Donald H. House     10/1/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 8/31/2018
*/

#ifndef __STATEVECTOR_H__
#define __STATEVECTOR_H__

#include "Vector.h"
#include "Particle.h"

class StateVector{
	private:
		int numParticles;	
                int length;
	public:
		StateVector();			
                StateVector(int np);

		Vector3d* states;	
		
                void fillConstant(float c);
     		void copyFromSV(Particle* particles); 
	        void copyToSV(Particle* particles); 

                StateVector add(StateVector s2); // add this to another statevector
                StateVector mult(float k); // multiply this by a scalar

                void print();
                int getNumParticles(){return numParticles;}
                int getLength(){return length;}

};	

#endif

