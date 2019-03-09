/*
* RandomGenerator.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/21/2018
* Version 1.0
*/

#ifndef __RANDOMGENERATOR_H__
#define __RANDOMGENERATOR_H__

#include "Vector.h"

double gauss(double mean, double std, int seed);
double uniform(double min, double max);
Vector3d randSphereVec();

Vector3d generateVelocity(float speed_avg, float speed_range);
Vector3d generatePosition(Vector3d position_avg, Vector3d position_range);
float generateMass(float mass_avg, float mass_range);


#endif
