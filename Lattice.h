/*

 CPSC 8170 Physically Based Animation
 Caroline Requierme (crequie@clemson.edu), 8/31/2018
*/

#ifndef __LATTICE_H__
#define __LATTICE_H__

#include "Vector.h"
#include "Cell.h"

class Lattice{
   private:
      int numCells;
      float cellWidth;
      float cellHeight;
      float cellDepth;
      float minX, minY, minZ, maxX, maxY, maxZ;

   public:      
      Cell* cells;

      Lattice();
      Lattice(int nc);

      void setBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz);

      void setCellDimensions(float w, float h, float d);

      int searchCellIndex(float x, float y, float z);
};	

#endif

