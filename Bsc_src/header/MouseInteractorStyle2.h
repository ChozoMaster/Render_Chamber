// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#pragma once

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <Qt>
#include <QObject>
#include "QVTKWidget.h"

class MouseInteractorStyle2 :public QObject, public vtkInteractorStyleTrackballCamera 
{
	Q_OBJECT
public:
	

	static MouseInteractorStyle2* New();
	 vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);
	   virtual void OnLeftButtonDown();
signals:
	   void sendPointID(int pointID, bool ctrl);



private:


};

