// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "RenderTupel.h"
#include "ColorSource.h"

//#define pcaScale 100

RenderTupel::RenderTupel(Liver * expert, Liver * segmentation, RelativeDistance * disExpertToSeg, RelativeDistance * disSegToExpert) {
	this->expert = expert;
	this->segmentation = segmentation;
	this->disExpertToSeg = disExpertToSeg;
	this->disSegToExpert = disSegToExpert;
	vtkPolyData * copy = vtkPolyData::New();
	copy->DeepCopy(segmentation->getData());
	segmentationPCA3D = new Liver(copy);
	//TODO Scalar From Too Hard Coded -> BAD!
	ColorSource cs;
	segmentationPCA3D->setLookupTable(segmentation->getColorOpacityLookupTable(),cs.getScalarFrom(), cs.getScalarTo());
	vtkPolyData * copy2 = vtkPolyData::New();
	copy2->DeepCopy(segmentation->getData());
	segmentationPCA2D = new Liver(copy2);
	segmentationPCA2D->setLookupTable(segmentation->getColorOpacityLookupTable(),cs.getScalarFrom(), cs.getScalarTo());
}

RenderTupel::~RenderTupel(){
	if(this->expert != 0) {
		delete this->expert;
		this->expert=0;
	}
	if (this->segmentation != 0){
		delete this->segmentation;
		this->segmentation=0;
	}
	if (this->disExpertToSeg != 0) {
		delete this->disExpertToSeg;
		this->disExpertToSeg=0;
	}
	if (this->disSegToExpert != 0) {
		delete this->disSegToExpert;
		this->disSegToExpert;
	}
}

Liver * RenderTupel::getExpertLiver(){
	return expert;
}

Liver * RenderTupel::getSegmentationLiver(){
	return segmentation;
}
RelativeDistance * RenderTupel::getDistanceExpert2Seg(){
	return disExpertToSeg;
}
RelativeDistance * RenderTupel::getDistanceSeg2Expert(){
	return disSegToExpert;
}

void RenderTupel::movePCA3D(int pcaScale)
{
	double * transformation = segmentationPCA3D->getData()->GetCenter();
	//vtkPolyData * copy = vtkPolyData::New();
	//copy->DeepCopy(segmentation->getData());
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	////	transformFilter->SetInput(segmentation->getData());
	transformFilter->SetInput(segmentationPCA3D->getData());
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transformFilter->SetTransform( transform );

	transform->Translate(pcaX * pcaScale - transformation[0], pcaY * pcaScale - transformation[1], pcaZ * pcaScale - transformation[2]);
	transform->Update();
	transformFilter->Update();
	segmentationPCA3D = new Liver(transformFilter->GetOutput());
	segmentationPCA3D->setLookupTable(segmentation->getColorOpacityLookupTable(),0.0, 40.0);
}

void RenderTupel::movePCA2D(int pcaScale)
{
	double * transformation = segmentationPCA2D->getData()->GetCenter();
	//vtkPolyData * copy = vtkPolyData::New();
	//copy->DeepCopy(segmentation->getData());
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	////	transformFilter->SetInput(segmentation->getData());
	transformFilter->SetInput(segmentationPCA2D->getData());
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transformFilter->SetTransform( transform );

	transform->Translate((pcaX* pcaScale) - transformation[0] , (pcaY * pcaScale) - transformation[1], 0);
	transform->Update();
	transformFilter->Update();
	segmentationPCA2D = new Liver(transformFilter->GetOutput());
	transformFilter->GetOutput()->GetCenter(labelPosition);
	segmentationPCA2D->getActor()->GetProperty()->SetColor(0.5,0.5,0.5);
	//TODO: HARD CODED -> Bad
	ColorSource cs;
	segmentationPCA2D->setLookupTable(segmentation->getColorOpacityLookupTable(),cs.getScalarFrom(), cs.getScalarTo());
}

Liver * RenderTupel::getSegmentationLiverPCA2D()
{
	return segmentationPCA2D;
}

Liver * RenderTupel::getSegmentationLiverPCA3D()
{
	return segmentationPCA3D;
}



