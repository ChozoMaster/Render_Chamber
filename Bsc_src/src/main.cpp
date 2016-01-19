#include <vtkCylinderSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleFlight.h>
#include <QVTKInteractor.h>
#include <Qt>
#include <QtGui>
#include "mainWindow.h"
#include "MouseInteractorStyle2.h"
#include "MouseInteractorRubberBand.h"
#include <vector>
#include <vtkActor.h>

vtkStandardNewMacro(MouseInteractorStyle2);
vtkStandardNewMacro(MouseInteractorRubberBand);
vtkStandardNewMacro(HighlightInteractor);
vtkStandardNewMacro(InteractorStyleMoveGlyph);

int main(int argc, char *argv[])
{
	std::cout << "VTK SOURCE VERSION:" << vtkVersion::GetVTKSourceVersion() << std::endl;
	std::cout << "VTK MAJOR VERSION:" <<vtkVersion::GetVTKMajorVersion() << std::endl;
	std::cout << "VTK MINOR VERSION:" <<vtkVersion::GetVTKMinorVersion() << std::endl;
	//_CrtSetDbgFlag(_CrtSetDbgFlag(0)|_CRTDBG_CHECK_ALWAYS_DF);
//	afxMemDF |= checkAlwaysMemDF;
	QApplication a(argc, argv);
	mainWindow mainWindow;

//	mainWindow.setWidgets(testWindows(2));
//	mainWindow.createGrid(5);
//	mainWindow.cameraSync();
	mainWindow.show();
	return a.exec();
	//return EXIT_SUCCESS;
}