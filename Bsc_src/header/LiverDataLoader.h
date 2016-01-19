// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef _LIVER_DATA_LOADER_H_
#define _LIVER_DATA_LOADER_H_

#include <vtkPolyData.h>
#include <vtkImageResample.h>
#include "vtkOFFReader.h"
#include <vtkPolyDataReader.h>
#include <vtkMetaImageReader.h>
#include <vtkImageThreshold.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkMarchingCubes.h>
#include <vtkDecimatePro.h>
#include <vtkAlgorithmOutput.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataNormals.h>
#include <vtkFillHolesFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkImageData.h>
/**
Loads the livers either from a .mhd, .vtk, .off file. For mhd-files preproccessing steps are performed before the poly data are returned. 
The meshes will be moved to the center of the coordinate System.
The Feature Calculation will be handled in the Process.
For .mhd file: The mesh is extracted with Marching cubes. The Quality of the result depends on the parameters. Also, the meshes will be decimated to reduce the number of Points.
*/
class LiverDataLoader {
public:

	static vtkPolyData * loadMhdMarchingCubes(vtkImageData * copy1, bool decimate);
	static vtkPolyData * loadVtk(std::string path);
	static vtkPolyData * loadOFF(std::string path);
};
#endif