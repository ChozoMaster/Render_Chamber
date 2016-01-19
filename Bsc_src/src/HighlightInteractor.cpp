// ----------- Select and highlight 3D points ---------------------------------
// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "HighlightInteractor.h"




// Define interaction style


HighlightInteractor::HighlightInteractor()
	{
		this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
		this->SelectedActor = vtkSmartPointer<vtkActor>::New();
		this->SelectedActor->SetMapper(SelectedMapper);
	}

void HighlightInteractor::OnLeftButtonUp()
	{
		// Forward events
		vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

		vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();
		if(this->Points != NULL){
		vtkSmartPointer<vtkExtractGeometry> extractGeometry =
			vtkSmartPointer<vtkExtractGeometry>::New();
		extractGeometry->SetImplicitFunction(frustum);
		extractGeometry->SetInput(this->Points);
		
		extractGeometry->Update();
	
		vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
			vtkSmartPointer<vtkVertexGlyphFilter>::New();
		glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
		glyphFilter->Update();
		vtkPolyData* selected = glyphFilter->GetOutput();
		std::cout << "Selected " << selected->GetNumberOfPoints() << " points." << std::endl;
		std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
		this->SelectedMapper->SetInput(selected);
		std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
		this->SelectedMapper->ScalarVisibilityOff();
		std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;

		vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));
//		std::cout << "Total number of tuples: " << ids->GetNumberOfTuples() << std::endl;

		// Store the ids for further processing on the mesh
		// (selectionIds always have the latest selection of points in the mesh
		//    - we process the selected portion only after the user presses another command
		//      which indicates what processing needs to be done. For eg., fill holes)
		selectionIds = ids;


		this->SelectedActor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
		this->SelectedActor->GetProperty()->SetPointSize(3);

		this->CurrentRenderer->AddActor(SelectedActor);
		this->GetInteractor()->GetRenderWindow()->Render();
		this->HighlightProp(NULL);

///////////////////////////////////////////////////////////////////////////////////////////////////////////


}
}

void HighlightInteractor::SetPoints(vtkSmartPointer<vtkPolyData> points) 
	{
		this->Points = points;
	}



