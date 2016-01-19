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
#include <vtkAreaPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <Qt>
#include <QObject>
#include "QVTKWidget.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleRubberBandPick.h"

class MouseInteractorRubberBand : public vtkInteractorStyleTrackballActor
{
	//Q_OBJECT
public:
	

	static MouseInteractorRubberBand* New();
	MouseInteractorRubberBand();
	 vtkTypeMacro(MouseInteractorRubberBand, vtkInteractorStyleTrackballActor);
	   virtual void OnMiddleButtonDown();
	   virtual void OnMiddleButtonUp();
	   virtual void OnMouseMove();
	   virtual void OnLeftButtonDown();
	   virtual void OnLeftButtonUp();
	   virtual void OnChar();
	

	   void StartSelect();



protected:
	virtual void Zoom();
	virtual void Pick();
	void RedrawRubberBand();

	int StartPosition[2];
	int EndPosition[2];

	int Moving;

	vtkUnsignedCharArray *PixelArray;

	int CurrentMode;
//signals:
	 //  void sendPointID(int pointID, bool ctrl);



private:


};

