// Vamco:
// Author: Dennis Basgier
// 20.01.2013

#include "ColorSource.h"


#define VTK_CREATE(type, name) \
	vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

ColorSource::ColorSource() {
	this->from = 0.0;
	this->to=42.0;
	this->opacity=0.25;
}



void ColorSource::setOpacity(double opacity){
	this->opacity = opacity;
}
double ColorSource::getOpacity(){
	return opacity;
}

vtkDoubleArray* ColorSource::getScalars(RelativeDistance * distance, double maxDistance, double minDistance) {
	vtkDoubleArray * scalars = vtkDoubleArray::New();
	for (int i = 0; i < distance->getNumberOfCalculatedDistances(); i++) {
		double dis = distance->getRelativeDistanceToClosestPointInOtherSet(i);
		double normalizedDistance = 0; 
		if(dis >= 0){
			if((maxDistance == 0) && (dis == 0)){
				normalizedDistance = 0;
			}else{
				normalizedDistance = dis / abs(maxDistance);
			}
		}else{
			normalizedDistance = dis / abs(minDistance);
		}
		scalars->InsertTuple1(i, returnScalarArray(normalizedDistance));
	}
	return scalars;
}

vtkDoubleArray* ColorSource::getScalars(std::vector< std::vector<int> >* clusterVec, int minClusterSize) {
	vtkDoubleArray * scalars = vtkDoubleArray::New();
	for (int i = 0; i < clusterVec->size(); i++) {

		for (int j = 0; j < clusterVec->at(i).size(); j++) {
			scalars->InsertTuple1(clusterVec->at(i).at(j), i);
		}
	}
	return scalars;
}

vtkDoubleArray* ColorSource::getScalars(vtkDoubleArray * distances, double maxDistance, double minDistance) {
	vtkDoubleArray * scalars = vtkDoubleArray::New();
	for (int i = 0; i < distances->GetNumberOfTuples(); i++) {
		double dis = distances->GetValue(i);
		double normalizedDistance = 0; 
		if(dis > 0){
			normalizedDistance = dis / abs(maxDistance);
		}else{
			normalizedDistance = dis / abs(minDistance);
		}
		scalars->InsertTuple1(i, returnScalarArray(normalizedDistance));
	}
	return scalars;
}


double ColorSource::returnScalarArray(double normalizedDistance) {
	vtkDoubleArray * scalars = vtkDoubleArray::New();
	double scalar = 0;
	if(normalizedDistance > 0){
		if (normalizedDistance > .90) {
			scalar = 20;
		} else if (normalizedDistance > .80) {
			scalar = 19;
		} else if (normalizedDistance > .70) {
			scalar = 18;
		} else if (normalizedDistance > .60) {
			scalar = 17;
		} else if (normalizedDistance > .50) {
			scalar = 16;
		} else if (normalizedDistance > .40) {
			scalar = 15;
		} else if (normalizedDistance > .30) {
			scalar = 14;
		} else if (normalizedDistance > .20) {
			scalar = 13;
		} else if (normalizedDistance > .10) {
			scalar = 12;
		} else if (normalizedDistance > 0.0) {
			scalar = 11;
		}
	} else if (normalizedDistance == 0) {
		scalar = 10;
	} else if (normalizedDistance > -0.10) {
		scalar = 9;
	} else if (normalizedDistance > -0.2) {
		scalar = 8;
	} else if (normalizedDistance > -0.3) {
		scalar = 7;
	} else if (normalizedDistance > -0.4) {
		scalar = 6;
	} else if (normalizedDistance > -0.5) {
		scalar = 5;
	} else if (normalizedDistance > -0.6) {
		scalar = 4;
	} else if (normalizedDistance > -0.7) {
		scalar = 3;
	} else if (normalizedDistance > -0.8) {
		scalar = 2;
	} else if (normalizedDistance > -0.9) {
		scalar = 1;
	} else if (normalizedDistance >= -1.0) {
		scalar = 0;
	}
	return scalar;

}


vtkDoubleArray* ColorSource::getScalars(RelativeDistance * distance) {

	return getScalars(distance, distance->getMaximalRelativeDistance(), distance->getMinimalRelativeDistance());
}

// In order to offer the opportunitiy to make some colors transparent while other remains fully opaque we defined a
// look up table with redundant values, once with a fixed opacity of 1.0 and a 2nd version with a variable opacity 
// which can be set via the setter methods.
// If a opacity change is triggered (via GUI) all scalar values are iterated and if they have to be rendered transparent,
// their scalar value is incremented by 20 in order to map to the values of the look up table in range [20..39] in order
// to support opacity for the respective color

vtkLookupTable * ColorSource::getLookupTable() 
{
	vtkLookupTable * color = vtkLookupTable::New();
	color->SetNumberOfColors(to );
	color->SetTableRange(0.0, to);

	VTK_CREATE(vtkColorTransferFunction, cool2warm);
	cool2warm->SetColorSpaceToDiverging();
	cool2warm->AddRGBPoint(0.0, 0.230, 0.299, 0.754);
	cool2warm->AddRGBPoint(1.0, 0.706, 0.016, 0.150);


	for (int i = 0; i < (to/2); i++)
	{
		double *colorRGB = cool2warm->GetColor((double) i/((to/2)-1));
		color->SetTableValue(i, colorRGB[0], colorRGB[1], colorRGB[2], 1.0);
		color->SetTableValue(i + (to/2), colorRGB[0], colorRGB[1], colorRGB[2], opacity);
	}
	return color;
}


vtkLookupTable * ColorSource::getRainbowLookupTable(int num) {

	vtkLookupTable * color = vtkLookupTable::New();
	color->SetNumberOfColors((num * 2) );
	color->SetTableRange(0.0, num * 2 );
	double S = 0.6;
	double	V = 1;
	for(int i = 0; i < num; i++){
		double j = i;
		double H = j / num;
		double* rgb = vtkMath::HSVToRGB(H,S,V);
		double r = rgb[0];
		double g = rgb[1];
		double b = rgb[2];
		color->SetTableValue(i, r, g, b, 0.4);
		color->SetTableValue(i + num, r, g, b, 1.0);
	}

	return color;
}


double ColorSource::normalize(double rgb255) {
	return rgb255/255;
}

double ColorSource::getScalarFrom() {
	return from;
}

double ColorSource::getScalarTo() {
	return to;
}