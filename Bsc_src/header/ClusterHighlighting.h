// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef clusterHighlighting_H
#define clusterHighlighting_H

#include "ui_clusterHighlightning.h"
#include <Qt>
#include <QtGui>
#include <QVTKWidget.h>
#include "AverageLiver.h"
#include "Clustering.h"
#include "ColorSource.h"

#define SAMPLEDISTANCE 1

struct  varianceStruct
{
	double min;
	double max;
	double avg;
	double variance;
	int clusterID;
	
};

struct  QuartilStruct
{
	double min;
	double max;
	double median;
	double lowQuartil;
	double highQuartil;
	double avg;
	int clusterID;
	int liverID;
};

struct  SDStruct
{
	int clusterSize;
	double threshold;
	double density;
	double separation;
	double VCR;
};

//class mainWindow;
class clusterHighlighting : public QDialog
{
	Q_OBJECT

public:
	clusterHighlighting(QWidget *parent, AverageLiver* avgLiver, double threshold, std::vector<int>* choice );

signals:
	void chosenCluster(std::set<int>* chosenClusters);
	void sendThreshold(int threshold);
	void sendColorChangeCluster(int clusterIndex, QColor col);

private:
	QWidget *parentWidget;
	Ui::Dialog ui;
	Node* activeCluster;
	std::vector<SDStruct > VCRVector;
	double optimalThresh;
	double threshold;
	Node* minimalCluster;
	Node* avgCluster;
	Node* maximalCluster;
	Node* cluster;
	AverageLiver* avgLiver;
	std::vector<graphStruct>* coordsVec;
	std::map<int, varianceStruct > intraVariance;
	std::map<int, QuartilStruct > intraQuartiles;
	std::vector<int> selectedClusterIDs;
	std::vector<int> PCAselection;
	QGraphicsLineItem* threshLine;
	double maxX ;
	double thresholdGlobalPosition;
	//temp ID for box plotting
	int clusterID;
	int mode;
	int sortValue;
	int sortCluster;
	int sortCriteria;
	bool doSort;

public slots:
//	void highlightClusters(std::vector<int>* clustersToHighlight);
	void setupGraph(Node* cluster, double thresh, vtkDoubleArray* distances, std::vector<int>* choice, vtkLookupTable* lookupTable);
	void highlightClusters();
	
	void highlightClusters(std::set<int>* clusterChoice);
	void resetGraph();
	void calculateIntraHistogramVariance(int cluster);
	void paintCandleChart(int clusterID);
	std::vector<std::vector<double> > calculateClusterCentroids(std::vector< std::vector<int> >* ClusterVector);
	double calculateInterClusterVarianceMaxDistance( std::vector<std::vector<double> > centroidVector);
	double calculateIntraClusterVarianceMaxDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector);
	void paintVRCindex();
	std::vector<SDStruct >  runClusterSimulation(int maxClusterCount);
	void paintVCRChart();
	double calculateInterClusterVarianceEucDistance(std::vector< std::vector<int> >* ClusterVector ,std::vector<std::vector<double> > centroidVector, std::vector<double> centroid, std::vector<std::set<int> > neighboringClusters);

	std::vector<double> calculateDataCentroid( std::vector<std::vector<double> > &centroidVector ); 


	double calculateIntraClusterVarianceEucDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector, double totalVariance);
	void paintBoxPlot();
	void calculateIntraHistogramQuartiles(int cluster);
	void paintParallelCoordinates(std::vector<int> Clusters, int clusterSortID, bool doSort, int sortValue, int sortCriteria, bool newLists);
	std::map<int, QuartilStruct > calculateIntraHistogramQuartilesOut(int cluster);
	void sortGraph();
	void parallelCoordinatesSignal();
	void SortedParallelCoordinatesSignal(std::vector<int> sortedVector);
	void findBestThreshold();
	std::vector<std::set<int> > getNeighboringClusters(std::vector< std::vector<int> >* allClusters);
	double calculateRSMSSTD(std::vector< std::vector<int> >* ClusterVector, std::vector<double> *dataCentroid);
	void repaintThreshline(int thresh);
	void changeClusterColor(QListWidgetItem * item);
	//	emit sendGraphData();
};



#endif