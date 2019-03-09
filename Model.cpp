//********************************************************************************
/*
 Model.cpp
 CPSC 8170 Physically Based Animation
 Donald H. House     8/23/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 10/28/2018

 Implementation for Lattice Deformer
 */
//********************************************************************************

#include "Model.h"
#include "Vector.h"
#include "StateVector.h"
#include "Particle.h"
#include "RandomGenerator.h"
#include "objtriloader.h"
#include "Cell.h"
#include "Lattice.h"

#include <cstdlib>
#include <cstdio>
#include <math.h>       

using namespace std;

//-----------------------------------------------------------------
/*
Model::Model()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None, initiates the simulation
*/
//-----------------------------------------------------------------

Model::Model(){
  //initSimulation();
  dispinterval = 1;
}

//-----------------------------------------------------------------
/*
Model::setBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
* PURPOSE : Receive bounding information about the 3d mesh to be deformed
* INPUTS :  float minX, float minY, float minZ, minimum bounding values for the mesh
            float maxX, float maxY, float maxZ, maximum bounding values for the mesh
* OUTPUTS : None, sets class variables
*/
//-----------------------------------------------------------------

void Model::setBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
  minX_bound = minX;
  minY_bound = minY;
  minZ_bound = minZ;
  maxX_bound = maxX;
  maxY_bound = maxY;
  maxZ_bound = maxZ;
 
}

//-----------------------------------------------------------------
/*
Model::constructLattice()
* PURPOSE : Receive bounding information about the 3d mesh to be deformed
* INPUTS :  float minX, float minY, float minZ, minimum bounding values for the mesh
            float maxX, float maxY, float maxZ, maximum bounding values for the mesh
* OUTPUTS : None, sets class variables
*/
//-----------------------------------------------------------------

void Model::constructLattice()
{

//============================================
  // Set springy mesh parameters
  float cubeMass = 1000.0;
  float k = 11.1;
  float d = 2.8;
  float l_rest = 5.0;

//============================================
  // Define lattice and cell dimensions
  float latticeWidth = maxX_bound - minX_bound;
  float latticeHeight = maxY_bound - minY_bound;
  float latticeDepth = maxZ_bound - minZ_bound;

  int L = 2;  // num depth planes
  int M = 12; // num rows
  int N = 4;  // num cols

  numParticles = ((L + 1) * (M + 1) * (N + 1));

  int numCells = L * M * N;
  lattice = Lattice(numCells);
  lattice.setBounds(minX_bound, minY_bound, minZ_bound, maxX_bound, maxY_bound, maxZ_bound);

  float cellWidth = fabs(latticeWidth/N);
  float cellHeight = fabs(latticeHeight/M);
  float cellDepth = fabs(latticeDepth/L);

  lattice.setCellDimensions(cellWidth, cellHeight, cellDepth);
//============================================
  // Set initial particle parameters
  float particleMass = cubeMass/numParticles;
  float speed_avg = 1.0;
  float speed_range = 0.08;
  
//============================================
// Construct all particles (vertices) in the lattice and add to system particle list
  
  particles = new Particle[numParticles];
  int p_index = 0; // index in system particle list

  for (int z = 0; z < L + 1; z++)
  {
     for (int y = 0; y < M + 1; y++)
     {
        for (int x = 0; x < N + 1; x++)
        {
           Vector3d particlePosition = {minX_bound + (x * cellWidth), minY_bound + (y * cellHeight), minZ_bound + (z * cellDepth)};
           //Vector3d particleVelocity = generateVelocity(speed_avg, speed_range);
           Vector3d particleVelocity = {0.0, 0.0, 0.0};
           particles[p_index] = Particle(particlePosition, particleVelocity, particleMass);
	   // Pin top of the lattice deformer to show effect of gravity
           if (y == M){
              particles[p_index].isPinned = true;
           }
           p_index = p_index + 1;
        }
      }
  }

//============================================
// Construct struts -- struts at this point are not attached to any particles
  numStruts = (M * N * L * 16) + (4 * M * L) + (4 * N * L) + (4 * M * N);
  struts = new Strut[numStruts];
  for (int st = 0; st < numStruts; st++)
  {
     struts[st] = Strut(k, d, l_rest);
  }

//============================================
// Construct the cells of the lattice and attach to constructed particles
  int strut_index = 0;
  float depth_height_diagonal = sqrt(pow(cellDepth, 2) + pow(cellHeight, 2));
  float depth_width_diagonal = sqrt(pow(cellDepth, 2) + pow(cellWidth, 2));
  float height_width_diagonal = sqrt(pow(cellHeight, 2) + pow(cellWidth, 2));
  float cell_diagonal = sqrt(pow(cellHeight,2) + pow(cellWidth,2) + pow(cellDepth,2));

  for (int p = 0; p < L; p++)
  {
     for (int r = 0; r < M; r++)
     {
        for (int c = 0; c < N; c++)
        {
           lattice.cells[(p*M + r)*N + c] = Cell(p, r, c);
           int back_bottom_left = c + (r*(N + 1)) + (p*((M + 1) * (N + 1)));
           int back_bottom_right = back_bottom_left + 1;
           int back_top_left = c + ((r + 1)*(N + 1)) + (p* (M + 1) * (N + 1));
           int back_top_right = back_top_left + 1;
           int front_bottom_left = c + (r*(N + 1)) + ((p + 1)*(M + 1)*(N + 1));
           int front_bottom_right = front_bottom_left + 1;
           int front_top_left = c + ((r + 1)*(N + 1)) + ((p + 1)*(M + 1)*(N + 1));
           int front_top_right = front_top_left + 1;

           lattice.cells[(p*M + r)*N + c].vertIndices[0] = back_bottom_left;
           lattice.cells[(p*M + r)*N + c].vertIndices[1] = back_bottom_right;
           lattice.cells[(p*M + r)*N + c].vertIndices[2] = back_top_left;
           lattice.cells[(p*M + r)*N + c].vertIndices[3] = back_top_right;
           lattice.cells[(p*M + r)*N + c].vertIndices[4] = front_bottom_left;
           lattice.cells[(p*M + r)*N + c].vertIndices[5] = front_bottom_right;
           lattice.cells[(p*M + r)*N + c].vertIndices[6] = front_top_left;
           lattice.cells[(p*M + r)*N + c].vertIndices[7] = front_top_right;

           lattice.cells[(p*M + r)*N + c].setMinBounds(particles[back_bottom_left].position.x, particles[back_bottom_left].position.y, particles[back_bottom_left].position.z);
           lattice.cells[(p*M + r)*N + c].setMaxBounds(particles[front_top_right].position.x, particles[front_top_right].position.y, particles[front_top_right].position.z);
           
           //============================================
           // Connect struts between the particles in each cell

           // FOR EACH CELL: CONNECT STRUTS ALONG INTERNAL DIAGONALS
           struts[strut_index].connectVerts(front_top_left, back_bottom_right); // p6 to p1
           struts[strut_index].setLRest(cell_diagonal); 
           strut_index += 1;
           struts[strut_index].connectVerts(front_top_right, back_bottom_left); // p7 to p0
           struts[strut_index].setLRest(cell_diagonal);
           strut_index += 1;
           struts[strut_index].connectVerts(back_top_left, front_bottom_right); // p2 to p5
           struts[strut_index].setLRest(cell_diagonal); 
           strut_index += 1;
           struts[strut_index].connectVerts(back_top_right, front_bottom_left); // p3 to p4
           struts[strut_index].setLRest(cell_diagonal); 
           strut_index += 1;

           // FOR EACH COLUMN: CONNECT STRUTS FRONT TO BACK
           struts[strut_index].connectVerts(back_bottom_left, front_bottom_left); // p0 to p4
           struts[strut_index].setLRest(cellDepth);
           strut_index += 1;
           struts[strut_index].connectVerts(back_top_left, front_top_left); // p2 to p6
           struts[strut_index].setLRest(cellDepth);
           strut_index += 1;
           // diagonals
           struts[strut_index].connectVerts(back_top_left, front_bottom_left); // p2 to p4
           struts[strut_index].setLRest(depth_height_diagonal);
           strut_index += 1;
           struts[strut_index].connectVerts(front_top_left, back_bottom_left); // p6 to p0
           struts[strut_index].setLRest(depth_height_diagonal); 
           strut_index += 1;

           if (c == N - 1){ // if last cell in row
              struts[strut_index].connectVerts(back_bottom_right, front_bottom_right); // p1 to p5
              struts[strut_index].setLRest(cellDepth);
              strut_index += 1;  
              struts[strut_index].connectVerts(back_top_right, front_top_right); // p3 to p7
              struts[strut_index].setLRest(cellDepth);
              strut_index += 1;  
              // diagonals
              struts[strut_index].connectVerts(back_top_right, front_bottom_right); // p3 to p5
              struts[strut_index].setLRest(depth_height_diagonal); 
              strut_index += 1;
              struts[strut_index].connectVerts(back_bottom_right, front_top_right); // p1 to p7
              struts[strut_index].setLRest(depth_height_diagonal);
              strut_index += 1;
           }

           // FOR EACH ROW: CONNECT STRUTS LEFT TO RIGHT
           struts[strut_index].connectVerts(back_bottom_left, back_bottom_right); // p0 to p1
           struts[strut_index].setLRest(cellWidth);
           strut_index += 1;
           struts[strut_index].connectVerts(front_bottom_left, front_bottom_right); // p4 to p5
           struts[strut_index].setLRest(cellWidth);
           strut_index += 1;
           // diagonals
           struts[strut_index].connectVerts(back_bottom_right, front_bottom_left); // p1 to p4
           struts[strut_index].setLRest(depth_width_diagonal);
           struts[strut_index].connectVerts(back_bottom_left, front_bottom_right); // p0 to p5
           struts[strut_index].setLRest(depth_width_diagonal);
           strut_index += 1;

           if (r == M - 1){ // if last cell in row
              struts[strut_index].connectVerts(back_top_left, back_top_right); // p2 to p3
              struts[strut_index].setLRest(cellWidth);
              strut_index += 1;  
              struts[strut_index].connectVerts(front_top_left, front_top_right); // p6 to p7
              struts[strut_index].setLRest(cellWidth);
              strut_index += 1;  
              // diagonals
              struts[strut_index].connectVerts(back_top_right, front_top_left); // p3 to p6
              struts[strut_index].setLRest(depth_width_diagonal); 
              strut_index += 1;
              struts[strut_index].connectVerts(back_top_left, front_top_right); // p2 to p7
              struts[strut_index].setLRest(depth_width_diagonal);
              strut_index += 1;
           }
           
           // FOR EACH DEPTH PLANE: CONNECT STRUTS BOTTOM TO TOP
           struts[strut_index].connectVerts(back_bottom_left, back_top_left); // p0 to p2
           struts[strut_index].setLRest(cellHeight);
           strut_index += 1;
           struts[strut_index].connectVerts(back_bottom_right, back_top_right); // p1 to p3
           struts[strut_index].setLRest(cellHeight);
           strut_index += 1;
           // diagonals
           struts[strut_index].connectVerts(back_bottom_left, back_top_right); // p0 to p3
           struts[strut_index].setLRest(height_width_diagonal);
           strut_index += 1;
           struts[strut_index].connectVerts(back_bottom_right, back_top_left); // p1 to p2
           struts[strut_index].setLRest(height_width_diagonal); 
           strut_index += 1;

           if (p == L - 1){ // if last cell in row
              struts[strut_index].connectVerts(front_bottom_left, front_top_left); // p4 to p6
              struts[strut_index].setLRest(cellHeight);
              strut_index += 1;  
              struts[strut_index].connectVerts(front_bottom_right, front_top_right); // p5 to p7
              struts[strut_index].setLRest(cellHeight);
              strut_index += 1;  
              // diagonals
              struts[strut_index].connectVerts(front_top_left, front_bottom_right); // p6 to p5
              struts[strut_index].setLRest(height_width_diagonal);
              strut_index += 1;
              struts[strut_index].connectVerts(front_bottom_left, front_top_right); // p4 to p7
              struts[strut_index].setLRest(height_width_diagonal);
              strut_index += 1;
           }

        }
      }
  }

}

//-----------------------------------------------------------------
/*
Model::initSimulation()
* PURPOSE : Initiate the lattice deformer as well as force simulation.
            This method: 1) initiates time and timestep 2) constructs
            the lattice represented by particles (vertices) and struts
            (edges) to bound the given mesh 3) initializes the system
            state vector.
* INPUTS :  NONE, uses class variables
* OUTPUTS : NONE, defines class variables
*/
//-----------------------------------------------------------------

void Model::initSimulation(){

//============================================
  // Set simulation parameters
   t = 0;
   h = 0.05;                                    
   n = 0;

   S = StateVector(numParticles);     // copy all position and velocity values to the state vector
   S.copyToSV(particles);

}

//-----------------------------------------------------------------
/*
StateVector Model::F(StateVector state_vec, float time)
* PURPOSE : System Dynamics Function
* INPUTS :  StateVector state_vec, contains the current state of the system
            float time, current time
* OUTPUTS : StateVector deriv, derivative of the system state vector (contains velocities and accelerations)
*/
//-----------------------------------------------------------------
StateVector Model::F(StateVector state_vec, float time)
{
   int nb = state_vec.getNumParticles();
   int len = state_vec.getLength();
   
   // copy velocities from S to the first half of derivative state vector
   StateVector deriv = StateVector(nb);
   for (int k=nb; k < len; k++)
   {
      deriv.states[k - nb].set(S.states[k].x, S.states[k].y, S.states[k].z);
   } 
   
   // loop through each vertex
   for(int m=0; m < numParticles; m++)
   {
      particles[m].clearForce();
      particles[m].computeExtForces();
   }

   for (int n=0; n < numStruts; n++){
      struts[n].computeVertForces(particles);
   }

   for (int o=0; o < numParticles; o++)
   {
      particles[o].computeAcceleration(); 
      // copy acceleration to derivative state vector
      deriv.states[o + numParticles].set(particles[o].acceleration.x, particles[o].acceleration.y, particles[o].acceleration.z);
   }

   
   
   return deriv;
} 
//-----------------------------------------------------------------
/*
StateVector Model::numInt(StateVector Sn, StateVector Sn1, float timestep)
* PURPOSE : Perform 4th order Runge Kutta integration on the current system state vector
* INPUTS :  StateVector Sn, system state vector at time n
            StateVector S_dot, system state vector at time n+1
            float timestep
* OUTPUTS : StateVector Snew, result of integration 
*/
//-----------------------------------------------------------------

StateVector Model::numInt(StateVector Sn, StateVector S_dot, float timestep)
{
   StateVector Snew = StateVector(numParticles);
   StateVector K1 = S_dot;
   StateVector K2 = F(Sn.add(K1.mult(timestep/2.0)), (t + (timestep/2.0)));
   StateVector K3 = F(Sn.add(K2.mult(timestep/2.0)), (t + (timestep/2.0)));
   StateVector K4 = F(Sn.add(K3.mult(timestep)), (t + timestep));

   StateVector RK = K1.add(K2.mult(2));
   RK = RK.add(K3.mult(2));
   RK = RK.add(K4);
   RK = RK.mult(timestep/6.0);
   
   Snew = Sn.add(RK);
   return Snew;
}

//-----------------------------------------------------------------
/*
Model::timeStep()
* PURPOSE : Perform one time step in the simulation
* INPUTS :  None
* OUTPUTS : None, updates particles 
*/
//-----------------------------------------------------------------

void Model::timeStep(){

StateVector Snew = StateVector(numParticles);

  if(running){
     S.copyToSV(particles);
     Sdot = F(S, t);
     Snew = numInt(S, Sdot, h);
     S = Snew;
     S.copyFromSV(particles);
     n = n + 1;
     t = n * h;
   }
}

//-----------------------------------------------------------------
/*
Model::startSimulation()
* PURPOSE : Start the simulation
* INPUTS :  None
* OUTPUTS : None, changes state of boolean flag
*/
//-----------------------------------------------------------------

void Model::startSimulation(){
  running = true;
}
