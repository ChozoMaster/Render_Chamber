// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "LiverDataLoader.h"



vtkPolyData * LiverDataLoader::loadMhdMarchingCubes(vtkImageData * copy1, bool decimate){
	vtkImageResample* vtkir = vtkImageResample::New(); 
	vtkir->SetInput(copy1);
	vtkir->SetAxisOutputSpacing(0,3);
	vtkir->SetAxisOutputSpacing(1,3);
	vtkir->SetAxisOutputSpacing(2,3);
	vtkir->Update();
	vtkImageThreshold * threshold = vtkImageThreshold::New();

	threshold->SetInput(vtkir->GetOutput());
	threshold->ThresholdByUpper(1);
	threshold->SetInValue(255);
	threshold->SetOutValue(0);
	threshold->Update();

	vtkImageGaussianSmooth * gauss = vtkImageGaussianSmooth::New();  
	gauss->SetStandardDeviations(3, 3, 3);
	gauss->SetInput(threshold->GetOutput());
	gauss->Update();

	vtkMarchingCubes * cubes = vtkMarchingCubes::New();
	cubes->SetInputConnection(gauss->GetOutputPort());
	cubes->SetValue(0,1);
	cubes->Update();


	vtkAlgorithmOutput * data=0;

	if (true) {
		vtkDecimatePro * decimatePro = vtkDecimatePro::New();
		decimatePro->SetTargetReduction(0.6);
		decimatePro->SetInputConnection(cubes->GetOutputPort());
		decimatePro->Update();
		data = decimatePro->GetOutputPort();
	} else {
		data = cubes->GetOutputPort();
	}

	vtkSmoothPolyDataFilter * smoothPoly = vtkSmoothPolyDataFilter::New();
	smoothPoly->SetInputConnection(data);
	smoothPoly->SetNumberOfIterations(50);
	smoothPoly->Update();

	vtkFillHolesFilter * fillHoles = vtkFillHolesFilter::New();
	fillHoles->SetInputConnection(smoothPoly->GetOutputPort());
	fillHoles->SetHoleSize(100.0);
	fillHoles->Update();

	// A bit ugly, see: http://vtk.org/Bug/view.php?id=9053
	if (fillHoles->GetOutput()->GetNumberOfCells() == 0){
		data = smoothPoly->GetOutputPort();
	}else {
		data = fillHoles->GetOutputPort();
	}
	vtkPolyDataNormals * normals = vtkPolyDataNormals::New();
	normals->SetInputConnection(data);
	normals->Update();

	vtkPolyData * copy = vtkPolyData::New();

	copy->DeepCopy(normals->GetOutput());
	cout << "\n mhd number of points:" << copy->GetNumberOfPoints() << "\n";

	//Delete vtk stuff we do not need anymore
	normals->Delete();            
	data->Delete();
	cubes->Delete();
	gauss->Delete();	
	return copy;
}

vtkPolyData * LiverDataLoader::loadVtk(std::string path) {
	vtkPolyDataReader * reader = vtkPolyDataReader::New();
	reader->SetFileName(path.c_str());
	reader->Update();

	vtkPolyData * copy = vtkPolyData::New();
	copy->DeepCopy(reader->GetOutput());


	//      reader->Delete();            
	return copy;

}
vtkPolyData * LiverDataLoader::loadOFF(std::string path) {
	vtkOFFReader * reader = vtkOFFReader::New();
	reader->SetFileName(path.c_str());
	reader->Update();
	vtkPolyData * copy = vtkPolyData::New();
	copy->DeepCopy(reader->GetOutput());          
	return copy;

}

