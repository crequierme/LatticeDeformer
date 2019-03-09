/*
 Model.h
 CPSC 8170 Physically Based Animation
 Donald H. House     8/26/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 8/31/2018
*/

#ifndef __MODEL_H__
#define __MODEL_H__

#include "Vector.h"
#include "StateVector.h"
#include "Particle.h"
#include "Strut.h"
#include "Cell.h"
#include "objtriloader.h"
#include "Lattice.h"

class Model{
  private:
    float h; 		// h, timestep
    int dispinterval;	
    bool running;	// flag to start simulation
    float t;		// t, Current time
    int n;

    int numParticles;
    Particle* particles;

    int numStruts;
    Strut* struts;

    StateVector S;
    StateVector* Spointer;
    StateVector Sdot;

    float minX_bound, minY_bound, minZ_bound, maxX_bound, maxY_bound, maxZ_bound;

    Lattice lattice;
    Lattice* Lpointer;


  public:
    Model();

    void setBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    void constructLattice();
    void initSimulation();

    StateVector F(StateVector state_vec, float time); 
    StateVector numInt(StateVector Sn, StateVector Sn1, float timestep);

    void timeStep();
    void startSimulation();     
  
    bool isSimRunning(){return running;}
    int displayInterval(){return dispinterval;}
    int getNumParticles(){return numParticles;}
    StateVector* getSPointer(){Spointer = &S; return Spointer;}
    Lattice* getLPointer(){Lpointer = &lattice; return Lpointer;}
    Particle* getParticles(){return particles;}
    Strut* getStruts(){return struts;}
    int getNumStruts(){return numStruts;}
};

#endif
