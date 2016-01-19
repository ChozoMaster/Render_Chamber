// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "RelativeDistance.h"
#include "vtkDataSet.h"

double euclideanDistance(double a[3],double b[3]){
	double sum = 0;
	sum+= pow(a[0] - b[0],2);
	sum+= pow(a[1] - b[1],2);
	sum+= pow(a[2] - b[2],2);
	sum = sqrt(sum);
	return sum;
}
RelativeDistance::RelativeDistance(vtkPolyData * dataA, vtkPolyData * dataB, int livernumID) {
	this->dataA = dataA;
	this->dataB = dataB;
	this->locator = vtkKdTreePointLocator::New();
	this->indexAdistanceB = vtkDoubleArray::New();
	this->maxDistance=0.0;
	this->minDistance=0.0;
	this->indexAdistanceTuple = new std::map<int, IdDistanceTupel>();
	char buffer [33];
	_itoa_s(livernumID,buffer,10);
	indexAdistanceB->SetName(buffer);
}


RelativeDistance::~RelativeDistance() {
	this->indexAdistanceTuple->clear();
	delete this->indexAdistanceTuple;
	this->indexAdistanceTuple=0;
}

void RelativeDistance::calculate( bool totalCorrespondence,bool gettoDistances )
{
	indexAdistanceB->SetNumberOfComponents(1);
	indexAdistanceB->SetNumberOfTuples(dataA->GetNumberOfPoints());
	if(gettoDistances){
		cout << "\nCalculation with predefined Distances\n" ;
		vtkDoubleArray* distances = vtkDoubleArray::SafeDownCast(dataA->GetPointData()->GetArray("Distances"));
		vtkDoubleArray* underestimation = vtkDoubleArray::SafeDownCast(dataA->GetPointData()->GetArray("Underestimation"));
		vtkDoubleArray* targetPointID = vtkDoubleArray::SafeDownCast(dataA->GetPointData()->GetArray("TargetId"));
		for (int i = 0; i < dataA->GetNumberOfPoints(); i++) {
			getPointIdOfClosestPointInOtherSet(distances, underestimation, targetPointID,i);
		}
	}else{

		locator->SetDataSet(dataB);
		locator->BuildLocator();

		double currentPoint[3];
		//Points inside test
		vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = 
			vtkSmartPointer<vtkSelectEnclosedPoints>::New();

		selectEnclosedPoints->SetInput(dataA);

		selectEnclosedPoints->SetSurface(dataB);
		selectEnclosedPoints->SetTolerance(0.0);

		selectEnclosedPoints->Update();

		//	#pragma omp parallel for
		for (int i = 0; i < dataA->GetNumberOfPoints(); i++) {
			dataA->GetPoint(i, currentPoint);
			//	int closestPointId = getPointIdOfClosestPointInOtherSet(i, currentPoint, false, totalCorrespondence);
			int closestPointId = getPointIdOfClosestPointInOtherSet(i, currentPoint, selectEnclosedPoints->IsInside(i), totalCorrespondence);
		}
	}

}



int RelativeDistance::getPointIdOfClosestPointInOtherSet(int i, double * currentPoint, bool inside, bool totalCorrespondence) {
	double closestPoint[3];
	int closestPointId;
	double distance;
	if(totalCorrespondence){
		dataB->GetPoint(i, closestPoint);
		distance = euclideanDistance(currentPoint, closestPoint);
		closestPointId = i;
	}	else{


		closestPointId = locator->FindClosestPoint(currentPoint);
		dataB->GetPoint(closestPointId, closestPoint);
		distance = calculateDistance(currentPoint, closestPoint);
	}
	if(distance != 0){
		if(inside){
			distance = -distance;
			indexAdistanceB->SetValue(i, distance);
			IdDistanceTupel tupel(closestPointId, distance);
			indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
		}
		else{
			indexAdistanceB->SetValue(i, distance);
			IdDistanceTupel tupel(closestPointId, distance);
			indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
		}
	}else{
		indexAdistanceB->SetValue(i, distance);
		IdDistanceTupel tupel(closestPointId, distance);
		indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
	}


	if (distance > maxDistance) {
		maxDistance = distance;
	}

	if (distance < minDistance) {
		minDistance = distance;
	}

	return closestPointId;

}

int RelativeDistance::getPointIdOfClosestPointInOtherSet(vtkDoubleArray* distanceArray, vtkDoubleArray* insideOutsideArray, vtkDoubleArray* targetPointIDs,int i) {
	int closestPointId = targetPointIDs->GetValue(i);

	double distance = distanceArray->GetValue(i);
	if(distance != 0){
		if(insideOutsideArray->GetValue(i) == 1){
			distance = -distance;
			indexAdistanceB->SetValue(i, distance);
			IdDistanceTupel tupel(closestPointId, distance);
			indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
		}
		else{
			indexAdistanceB->SetValue(i, distance);
			IdDistanceTupel tupel(closestPointId, distance);
			indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
		}
	}else{
		indexAdistanceB->SetValue(i, distance);
		IdDistanceTupel tupel(closestPointId, distance);
		indexAdistanceTuple->insert(std::pair<int, IdDistanceTupel>(i, tupel));
	}


	if (distance > maxDistance) {
		maxDistance = distance;
	}

	if (distance < minDistance) {
		minDistance = distance;
	}

	return closestPointId;

}
IdDistanceTupel RelativeDistance::getDistanceTupelForIndexOfSetA(int i) {
	std::map<int, IdDistanceTupel>::iterator it = indexAdistanceTuple->find(i);
	return it->second;
}

int RelativeDistance::getNumberOfCalculatedDistances() {
	return indexAdistanceB->GetNumberOfTuples();
}

double RelativeDistance::getRelativeDistanceToClosestPointInOtherSet(int tupleIndex) {
	return indexAdistanceB->GetTuple1(tupleIndex);
}

double RelativeDistance::getMaximalRelativeDistance(){
	return maxDistance;
}	

double RelativeDistance::getMinimalRelativeDistance(){
	return minDistance;
}

double RelativeDistance::calculateDistance(double * currentPoint, double *closestPoint) {

	double a = pow(currentPoint[0] - closestPoint[0],2);
	double b = pow(currentPoint[1] - closestPoint[1],2);
	double c = pow(currentPoint[2] - closestPoint[2],2);
	double distance = sqrt(a+b+c);
	return distance;
}

vtkSmartPointer<vtkDoubleArray> RelativeDistance::getDistanceArray()
{
	return indexAdistanceB;
}





