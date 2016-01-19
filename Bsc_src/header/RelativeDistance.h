// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef _RELATIVE_DISTANCE_H_
#define _RELATIVE_DISTANCE_H_

#include <vtkpolyData.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkKdTreePointLocator.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <map>
#include <list>
#include <math.h>
#include "IdDistanceTupel.h"
#include <stdio.h>
#include <stdlib.h>
/**
    This class calculates the relative distances betweeen each point in two point sets (A,B) from the view point of set A. The point sets can be of different size. The situation that many points of A (if it has more points than B) map to the same point of B or vice versa is  ignored.
 */
class RelativeDistance {

  public:
   	RelativeDistance(vtkPolyData * dataA, vtkPolyData * B, int);
   	int getNumberOfCalculatedDistances();
    double getRelativeDistanceToClosestPointInOtherSet(int tupleIndex);
	double getMinimalRelativeDistance();
    // returns the highest encountered distance over all distances
    double getMaximalRelativeDistance();
	double labelPosition[3];
   	~RelativeDistance();
    //explicity starts the calculation of the relative distances
	void calculate(bool totalCorrespondence,bool withDistances);
    //returns the distance tupel comprising of the id of the corresponding point in the set B and the distance to this point. The input value is the id of a point in set A
   	IdDistanceTupel getDistanceTupelForIndexOfSetA(int i);
	vtkSmartPointer<vtkDoubleArray> getDistanceArray();

  private:
  	vtkPolyData * dataA;
    vtkPolyData * dataB;
    vtkKdTreePointLocator * locator;
    vtkDoubleArray * indexAdistanceB;
    std::map<int, IdDistanceTupel> * indexAdistanceTuple;
    double maxDistance;
	double minDistance;
    int getPointIdOfClosestPointInOtherSet(int, double *, bool, bool);
	int getPointIdOfClosestPointInOtherSet(vtkDoubleArray* distanceArray, vtkDoubleArray* insideOutsideArray, vtkDoubleArray* targetPointIDs,int i);
	double calculateDistance(double[] , double[]);
    void saveMappingOfCurrentIndexToTheObtainedPointIdInTheOtherSet(int i, int closestPointId);
	

};

#endif