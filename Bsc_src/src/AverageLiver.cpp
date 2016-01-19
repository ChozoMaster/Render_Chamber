// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "AverageLiver.h"

AverageLiver::AverageLiver(std::vector<RenderTupel*> tupel, std::set<int> liverNums, bool onlyChoice){

	std::set<int>::iterator it;
	this->onlyChoice = onlyChoice; 
	tupels = tupel;
	SegAverageDistances = vtkDoubleArray::New();
	SegAverageDistances->SetNumberOfComponents(1);
	SegAverageDistances->SetNumberOfTuples(tupel.back()->getSegmentationLiver()->getData()->GetNumberOfPoints());
	segMaxAverageDistance = std::numeric_limits<double>::min();
	segMinAverageDistance = std::numeric_limits<double>::max();
	this->liverNums = liverNums;
	setupAverageSeqLiver();
	vtkPolyData* copy = vtkPolyData::New();
	copy->DeepCopy(segAverage->getData());
	clusteringLiver = new Liver(copy);
}

void AverageLiver::setupAverageSeqLiver(){
	vtkPolyData * copy2 = vtkPolyData::New();
	copy2->DeepCopy(tupels.back()->getSegmentationLiver()->getData());
	for(vtkIdType j = 0; j < tupels.back()->getSegmentationLiver()->getData()->GetNumberOfPoints(); j++) {
		std::vector<double> vec; 
		double tempPoint[3];
		double resultPoint[3] = {0,0,0};
		for(int i = 0; i < tupels.size(); i++){
			if(onlyChoice){
				if(liverNums.find(i) != liverNums.end()){
					vec.push_back(tupels.at(i)->getDistanceSeg2Expert()->getDistanceTupelForIndexOfSetA(j).getDistance());
					tupels.at(i)->getSegmentationLiver()->getData()->GetPoint(j,tempPoint);
					resultPoint[0] = resultPoint[0] + tempPoint[0];
					resultPoint[1] = resultPoint[1] + tempPoint[1];
					resultPoint[2] = resultPoint[2] + tempPoint[2];


				}
			}else{
				vec.push_back(tupels.at(i)->getDistanceSeg2Expert()->getDistanceTupelForIndexOfSetA(j).getDistance());
				tupels.at(i)->getSegmentationLiver()->getData()->GetPoint(j,tempPoint);
				resultPoint[0] = resultPoint[0] + tempPoint[0];
				resultPoint[1] = resultPoint[1] + tempPoint[1];
				resultPoint[2] = resultPoint[2] + tempPoint[2];
			}

		}
		if(!onlyChoice){
			resultPoint[0] = resultPoint[0] / tupels.size();
			resultPoint[1] = resultPoint[1] / tupels.size();
			resultPoint[2] = resultPoint[2] / tupels.size();
		}else{
			resultPoint[0] = resultPoint[0] / liverNums.size();
			resultPoint[1] = resultPoint[1] / liverNums.size();
			resultPoint[2] = resultPoint[2] / liverNums.size();
		}
		copy2->GetPoints()->SetPoint(j,resultPoint);

		SegHistogramData[j] = vec;

	}
	allSegHistogramData = SegHistogramData;
	calculateAverageDistances(SegHistogramData, SegAverageDistances, &segMaxAverageDistance, &segMinAverageDistance);
	segAverage = new Liver(copy2);

	ColorSource cs;
	segAverage->setLookupTable(cs.getLookupTable(), cs.getScalarFrom(), cs.getScalarTo());
	segAverage->setScalarVisibility(true);
	scalars = cs.getScalars(SegAverageDistances, segMaxAverageDistance, segMinAverageDistance);
	segAverage->setScalars(scalars);

}




void AverageLiver::calculateAverageDistances(std::map<int, std::vector<double> > histogramData, vtkDoubleArray* averageDistances, double* maxAverageDistance, double* minAverageDistance)
{
	std::map<int, std::vector<double> > ::const_iterator itr;



	for(itr = histogramData.begin(); itr != histogramData.end(); ++itr){
		double tempResult = 0;
		for(int i = 0; i < itr->second.size(); i++){
			tempResult = tempResult + itr->second.at(i);
		}
		double result = tempResult / itr->second.size();
		int id = itr->first;
		averageDistances->SetValue(id, result);
		if(*maxAverageDistance < result)
			*maxAverageDistance = result;
		if(*minAverageDistance > result)
			*minAverageDistance = result;
	}
}


void AverageLiver::restoreHistogramData(){
	SegHistogramData = allSegHistogramData;
}

void AverageLiver::reduceHistogramDataToChoice( std::vector<int> choice )
{
	std::map<int, std::vector<double> >::iterator itr;
	for(itr = SegHistogramData.begin(); itr != SegHistogramData.end(); ++itr){
		itr->second.clear();
		for(int i = 0; i < choice.size(); i++){
			itr->second.push_back(allSegHistogramData[itr->first].at(choice.at(i)));
		}
	}
}
