/*
* Cell.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/28/2018
* Version 1.0
*
*/

#ifndef __CELL_H__
#define __CELL_H__

#include "Vector.h"
#include "Particle.h"

class Cell{
   public:
      int vertIndices[8];	// indices of vertices of cube w/in particle list

      int planeCoord;	  	// coordinates for cell location w/in the lattice
      int rowCoord;
      int colCoord;

      Vector3d centerPos;	// location in space of center of cell

      float cellMinX;
      float cellMinY;
      float cellMinZ;
      float cellMaxX;
      float cellMaxY;
      float cellMaxZ;

      Cell();
      Cell(int p, int r, int c);

      void setMinBounds(float minx, float miny, float minz);
      void setMaxBounds(float maxx, float maxy, float maxz);
};	

#endif

