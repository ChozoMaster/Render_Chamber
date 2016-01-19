// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkIdTypeArray.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkRendererCollection.h>
#include <vtkProperty.h>
#include <vtkPlanes.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPointSource.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkAreaPicker.h>
#include <vtkExtractGeometry.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkIdFilter.h>
#include <vtkCamera.h>

// SelectPolyData
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <vtkGenericDataObjectReader.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkStructuredGrid.h>
#include <vtkPLYWriter.h>

// Extract selection
#include <vtkInformation.h>
#include <vtkExtractSelection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkPointSet.h>
#include <string>

class HighlightInteractor : public vtkInteractorStyleRubberBandPick
{
public:
	static HighlightInteractor* New();
	vtkTypeMacro(HighlightInteractor,vtkInteractorStyleRubberBandPick);
	HighlightInteractor();
	virtual void OnLeftButtonUp();
	void SetPoints(vtkSmartPointer<vtkPolyData> points);

	vtkIdTypeArray* selectionIds;
	vtkSmartPointer<vtkPolyData> Points;
	vtkSmartPointer<vtkActor> SelectedActor;
	vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
	//std::vector<RenderTupel*> renderTupels;
	//vector<vtkSmartPointer<vtkPolyData> > allPoints;
private:

};