/*
* Cell.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 10/28/2018
* Version 1.0
*
* Class Cell represents one cell within a lattice. Each cell
* contains a list of the indexes of its vertices (the vertices/
* particles themselves are held inside the system's particle list)
*/

#include "Cell.h"
#include "Vector.h"
#include <math.h>
using namespace std;

//-----------------------------------------------------------------
/*
Cell::Cell()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None
*/
//-----------------------------------------------------------------

Cell::Cell()
{

}

//-----------------------------------------------------------------
/*
Cell::Cell(int p, int r, int c)
* PURPOSE : Variable constructor
* INPUTS :  int p, "plane" z coordinate within the lattice
            int r, "row" y coordinate within the lattice
            int c, "column" x coordinate within the lattice
* OUTPUTS : None
*/
//-----------------------------------------------------------------
Cell::Cell(int p, int r, int c)
{
   planeCoord = p;
   rowCoord = r;
   colCoord = c;
}

void Cell::setMinBounds(float minx, float miny, float minz)
{
   cellMinX = minx;
   cellMinY = miny;
   cellMinZ = minz;
}
      
void Cell::setMaxBounds(float maxx, float maxy, float maxz)
{
   cellMaxX = maxx;
   cellMaxY = maxy;
   cellMaxZ = maxz;
}
