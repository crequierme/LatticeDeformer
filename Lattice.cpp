/*
* Lattice.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/28/2018
* Version 1.0
*
*/

#include "Lattice.h"
#include "Cell.h"
#include "Vector.h"
#include <math.h>
using namespace std;

Lattice::Lattice()
{
   numCells = 0;
   cells = NULL;
}

Lattice::Lattice(int nc)
{
   numCells = nc;
   cells = new Cell[nc];
}

void Lattice::setBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
   minX = minx;
   minY = miny;
   minZ = minz;
   maxX = maxx;
   maxY = maxy;
   maxZ = maxz;
}

void Lattice::setCellDimensions(float w, float h, float d)
{
   cellWidth = w;
   cellHeight = h;
   cellDepth = d;
}

int Lattice::searchCellIndex(float x, float y, float z)
{
   int index = -1;
   for (int i = 0; i < numCells; i++)
   {
      //cout << cells[i].cellMinX << endl;
      if (x >= cells[i].cellMinX && x <= cells[i].cellMaxX){
         if(y >= cells[i].cellMinY && y <= cells[i].cellMaxY){
            if(z >= cells[i].cellMinZ && z <= cells[i].cellMaxZ){
               index = i;
            }
         }
      }
   
   }

    if (index == -1){
      cout << x << " " << y << " " << z << endl;
      cout << minX << " " << maxX << endl;
      cout << minY << " " << maxY << endl;
      cout << minZ << " " << maxZ << endl;
      cout << endl;
}

   return index;
}



