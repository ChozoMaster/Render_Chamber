// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "Liver.h"

Liver::Liver(vtkPolyData * data) {
	this->data = data;
	this->mapper = vtkPolyDataMapper::New();
	this->mapper->SetInput(this->data);
	this->actor = vtkActor::New();
	this->actor->SetMapper(this->mapper);
	this->actor->GetProperty()->BackfaceCullingOn();
	this->actor->SetPickable(true);
	this->colorOpacityTable =0;
	this->scalar=0;
}

void Liver::setScalars(vtkDoubleArray * scalars) {
	data->GetPointData()->SetScalars(scalars);
	this->scalar = scalars;
}

vtkDoubleArray * Liver::getScalars(){
	return scalar;
}

vtkPolyData * Liver::getData(){
	return data;
}

vtkLookupTable * Liver::getColorOpacityLookupTable() {
	return colorOpacityTable;
}

void Liver::setScalarVisibility(bool state) {
	if (state) {
		mapper->ScalarVisibilityOn();
	} else {
		mapper->ScalarVisibilityOff();
	}
}

vtkActor * Liver::getActor(){
	return actor;
}

void Liver::setLookupTable(vtkLookupTable * table, double scalarFrom, double scalarTo) {
	this->colorOpacityTable = table;
	mapper->SetLookupTable(table);
    mapper->SetScalarRange(scalarFrom, scalarTo);
	mapper->Modified();
}