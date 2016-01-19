// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include <vector>
#include <vtkCamera.h>
#include <QObject>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>
#include <vtkDoubleArray.h>
#include <vtkAreaPicker.h>
#include <vtkTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkProp3DCollection.h>
#include <vtkCallbackCommand.h>
#include <vtkActor.h>
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkProperty.h>
#include <vtkPCAStatistics.h>
#include <vtkMultiCorrelativeStatistics.h>
#include <stdio.h>
#include <stdlib.h>
#include "RenderTupel.h"
#include <stdio.h>      /* printf */
#include <time.h> 

//Number of indices in Liver for Array initialization
//#define pointNum 2562
#define pointNum 50
//Number of indices in Vessel for Array initialization
//#define pointNum 3850

class PCA : public QObject {
	Q_OBJECT
public:
	static void calculatePCA(std::vector<RenderTupel*>*);
	

};