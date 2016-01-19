// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "MouseInteractorStyle2.h"

//MouseInteractorStyle2* MouseInteractorStyle2::New() : QObject, vtkInteractorStyleTrackballCamera{
//
//}
void MouseInteractorStyle2::OnLeftButtonDown()
{
	int* clickPos = this->GetInteractor()->GetEventPosition();
	bool ctrlPressed = false;
	std::cout << "CTRL " << GetInteractor()->GetControlKey() << std::endl;
	if(GetInteractor()->GetControlKey()){
		ctrlPressed = true;
	}
	// Pick from this location.
	vtkSmartPointer<vtkCellPicker>  picker =
		vtkSmartPointer<vtkCellPicker>::New();
	picker->Pick(clickPos[0], clickPos[1], 0,this->GetDefaultRenderer());

	double* pos = picker->GetPickPosition();
	std::cout << "Pick position (world coordinates) is: "
		<< pos[0] << " " << pos[1]
	<< " " << pos[2] << std::endl;

	std::cout << "Point ID " << picker->GetPointId() << std::endl;

	if(picker->GetPointId() != -1)
		emit sendPointID(picker->GetPointId(), ctrlPressed);

	//Create a sphere
	//vtkSmartPointer<vtkSphereSource> sphereSource =
	//vtkSmartPointer<vtkSphereSource>::New();
	//sphereSource->SetCenter(pos[0], pos[1], pos[2]);
	//sphereSource->SetRadius(0.1);

	////Create a mapper and actor
	//vtkSmartPointer<vtkPolyDataMapper> mapper =
	//  vtkSmartPointer<vtkPolyDataMapper>::New();
	//mapper->SetInputConnection(sphereSource->GetOutputPort());

	//vtkSmartPointer<vtkActor> actor =
	//  vtkSmartPointer<vtkActor>::New();
	//actor->SetMapper(mapper);


	////this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
	//this->GetDefaultRenderer()->AddActor(actor);
	// Forward events
	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}


