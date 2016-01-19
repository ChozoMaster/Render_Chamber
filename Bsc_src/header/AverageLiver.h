// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef AverageLiver_H
#define AverageLiver_H


#include "RenderTupel.h"
#include "ColorSource.h"
#include <limits>
#include <stdlib.h>

class AverageLiver {
public:
	//Constructor 
	//tupel: InputData
	//liverNums: selection of Liver Samples, if one exists
	//onlyChoice: Take only the selected subset to calculate
	AverageLiver(std::vector<RenderTupel*> tupel, std::set<int> liverNums, bool onlyChoice);

	//Calculate the average Feature Distances
	void calculateAverageDistances(std::map<int, std::vector<double> > histogramData, vtkDoubleArray* averageDistances, double* maxAverageDistance, double* minAverageDistance);

	//Reduces the Feature input Data to the Selection Subset
	void reduceHistogramDataToChoice(std::vector<int> choice);

	//Setups average Mesh (Geometrical)
	void setupAverageSeqLiver();

	//Restores the whole Data Set, if a subset was previously created
	void restoreHistogramData();

	//Input Data: Meshes and Features
	std::vector<RenderTupel*> tupels;

	//Clustering Liver Mesh
	Liver* clusteringLiver;


	Liver* segAverage;
	std::map<int, std::vector<double> > SegHistogramData;
	std::map<int, std::vector<double> > allSegHistogramData;
	vtkDoubleArray * SegAverageDistances;

	vtkDoubleArray * scalars;
	double segMaxAverageDistance;
	double segMinAverageDistance;
	std::set<int> liverNums;
	bool onlyChoice;

};
#endif