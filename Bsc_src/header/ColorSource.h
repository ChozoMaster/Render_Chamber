// Vamco:
// Author: Dennis Basgier
// 20.01.2013


#ifndef _COLOR_SOURCE_H_
#define _COLOR_SOURCE_H_

#include<vtkLookupTable.h>
#include<vtkDoubleArray.h>
#include<vtkPolyData.h>
#include<vtkColorTransferFunction.h>
#include "RelativeDistance.h"
#include <vtkMath.h>
#include <set>
/*
Colorsource provides the scalars values for the image data and the lookup table that maps the scalar values to a color.

*/
class ColorSource {
public:
	ColorSource();
	//Get Rainbow Colors for a specific Number of Colors
	vtkLookupTable * getRainbowLookupTable(int num);

	//Returns the color lookup table
	vtkLookupTable * getLookupTable();

	//The scalars to assign the specific color
	vtkDoubleArray * getScalars(RelativeDistance *);

	// For this scalars-method a maximal distance (e.g. over all liver pairs) can be provided in order to enforce a coloring from 0 to maxDistanceOverAllLOADEDLivers
	vtkDoubleArray * getScalars(RelativeDistance *, double, double);
	vtkDoubleArray* getScalars(vtkDoubleArray * distances, double maxDistance, double minDistance);
	vtkDoubleArray* getScalars(std::vector< std::vector<int> >* clusterVec, int minClusterSize);


	double getScalarFrom();
	double getScalarTo();
	void setOpacity(double);
	double getOpacity();
private:
	double normalize(double);
	double returnScalarArray(double normalizedDistance);

	double from;
	double to;
	double opacity;
};



#endif