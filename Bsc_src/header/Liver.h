// Vamco:
// Author: Dennis Basgier
// 20.01.2013

#ifndef _LIVER_H_
#define _LIVER_H_

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDoubleArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkActor.h>
#include <vtkProperty.h>

/**
   A Liver. Each Liver has an own actor, mapper and holds the poly data.
   The methods provided basically deploy methods of the underlying mapper or actor classes that are used. A newly instantiated liver has no scalars or look up table set. 
 */
class Liver {
	public:
		Liver(vtkPolyData *);
		void setScalars(vtkDoubleArray *);
		void setLookupTable(vtkLookupTable * , double, double);
		void setScalarVisibility(bool state);
		vtkActor * getActor();
		vtkPolyData * getData();
		vtkDoubleArray * getScalars();
		vtkLookupTable * getColorOpacityLookupTable();
		double transX;
		double transY;
		double transZ;

	private:
		vtkPolyData * data;
		vtkPolyDataMapper * mapper;
		vtkActor * actor;
		vtkLookupTable * colorOpacityTable;
		vtkDoubleArray * scalar; 
};
#endif