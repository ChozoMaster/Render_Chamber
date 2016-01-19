#include "LiverDataLoader.h"



vtkPolyData * LiverDataLoader::loadMhdMarchingCubes(vtkAlgorithmOutput* copy1, bool decimate){
	//vtkSmartPointer<vtkMetaImageReader>  metaReader = vtkSmartPointer<vtkMetaImageReader>::New();
	//	//	metaReader->Update();
	////		const char* test = path.c_str();

	//   	 	//metaReader->SetFileName(path.c_str());
	//		metaReader->SetFileName(path);
	//	//	metaReader->BreakOnError();
	//   	 	metaReader->Update();
	//	//	metaReader->getOut;
	//		vtkImageData * copy1 = vtkImageData::New();
	//		copy1->DeepCopy(metaReader->GetOutput());
	//		metaReader->Delete();
   // 		vtkImageThreshold * threshold = vtkImageThreshold::New();
			//
   // 		threshold->SetInputConnection(copy1);
   // 		threshold->ThresholdByUpper(1);
   // 		threshold->SetInValue(255);
   // 		threshold->SetOutValue(0);
			//threshold->Update();
    
    		vtkImageGaussianSmooth * gauss = vtkImageGaussianSmooth::New();  
    		gauss->SetStandardDeviations(3, 3, 3);
    		gauss->SetInputConnection(copy1);
			gauss->Update();

    		vtkMarchingCubes * cubes = vtkMarchingCubes::New();
    		cubes->SetInputConnection(gauss->GetOutputPort());
    		cubes->SetValue(0,1);
    		cubes->Update();
				

    		vtkAlgorithmOutput * data=0;
			
    		if (decimate) {
	    		vtkDecimatePro * decimatePro = vtkDecimatePro::New();
				decimatePro->SetTargetReduction(0.5);
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
        //    normals->FlipNormalsOn();
            normals->Update();
			
			vtkPolyData * copy = vtkPolyData::New();
			copy->DeepCopy(normals->GetOutput());
            
			
            //Delete vtk stuff we do not need anymore
            normals->Delete();            
            data->Delete();
            cubes->Delete();
            gauss->Delete();
          //  threshold->Delete();
            //metaReader->Delete();
			
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
		//static vtkPolyData loadVtk();
