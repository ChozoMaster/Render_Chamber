// Vamco:
// Author: Dennis Basgier
// 20.01.2013

#ifndef Clustering_H
#define Clustering_H

#include <stdlib.h>
#include <map>
#include <set>
#include <algorithm>
#include <vtkLineSource.h>

#include <vtkTriangleFilter.h>
#include <vtkExtractEdges.h>
#include <vtkSmartPointer.h>
#include <vtkCellCenters.h>
#include <vtkCellArray.h>
#include "Liver.h"
#include <Qt>
#include <QtGui>

typedef struct Node Node;

//Struct for a tree Node in the Clustering tree
struct Node
{
	double hierarchyLevel;
	int ID;
	Node *left;
	Node *right;
	Node(): ID(-1){}
	Node* copyNode(){
		Node* copied = new Node();
		copied->hierarchyLevel = this->hierarchyLevel;
		copied->ID = this->ID;
		copied->left = this->left;
		copied->right = this->right;
		return copied;
	}
};

//Struct for the ClusterID - VertexID - Map, -1 means the specific Vertex is not assigned to a cluster
struct data_t
{
	int value;
	data_t(): value(-1){} 
};

//enum for Linkage Criteria
enum linkage { 
	fastTest=1,
	average=2, 
	maximum=3, 
	minimum=4,
	EuklidFeatureCompleteLinkage = 5,
	ChebyshevFeatureCompleteLinkage = 6,
	EuklidFeatureCentroidLinkage = 7,
	ChebyshevFeatureCentroidLinkage = 8

};

//Struct for Neighbor Distance List
struct connections{double distance; int A; int B;};


//Class for all Clustering Related Calculations

class Clustering : public QObject{

	Q_OBJECT



public:

	//Spatial Hierachical Clustering Core Function:
	//Input Mesh: For Neighbor Relationship
	//FeatureVectors
	//link: The chosen Linkage Criterion
	Node* hierarchicalClustering(Liver* liver, std::map<int, std::vector<double> > FeatureVectors, linkage link);

	//Method for update of the Neighbor Distance List
	int changeClusterDistances(std::map<int, data_t> PointClusterIDs, linkage link, std::list<connections>* allConnections, std::map<int, std::vector<double> > SegHistogramData, Node* base);

	//Retrieving the Neighor Relationsship
	static void GetConnectedVertices(vtkSmartPointer<vtkPolyData> mesh, int seed, std::set<int>* connectedVertices);

	//Calculate Distances of Vertex Neighbors
	static std::list<Clustering::connections>* getNeighborDistances(Liver* liver, std::map<int, std::vector<double> > SegHistogramData, linkage link);

	//Changing the Cluster-Vertex-IDMap after a Merge
	static void changePointClusterIDs(std::map<int, data_t>* clusterIds, int oldClusterID, int newClusterID);

	//Get the Cluster Vector:
	//base = Root Node Input
	//HierarchyLevel: threshold to cut the tree
	//allCLusters : Output, First vector are the Clusters, inside Vector are the VertexID which are members of the Cluster
	static void getAllClusterVector(Node* base, double HierachyLevel, std::vector< std::vector<int> >* allClusters);
	static void traverseTree(Node* base, std::vector<int>* cluster );



	static int updateClusterDistance(std::map<int, data_t> PointClusterIDs, std::list<connections>* allConnections, std::vector<int>* allPointsFromCluster,  std::map<int, std::vector<double> > SegHistogramData, linkage link);


	//functions for Getting the Cluster Distances with FeatureVector Distances
	static double getHisMaxMaxClusterDistance(std::vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point);
	static double getHisEuclidianMaxClusterDistance(std::vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point);
	static double getHisMaxAvgClusterDistance(std::vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point);
	static double getHisEuclidianAvgClusterDistance(std::vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point);

	//transform the Cluster Vector to A Map
	static std::map<int, int> transformToClusterMap(std::vector< std::vector<int> >* allClusters);

	//Extract Borders (Marching Squares)
	static std::set<int>* getClusterBorders(std::vector< std::vector<int> >* allClusters, Liver* liver);

	//Add Border to a Mesh
	static void addClusterBorderPoints(int seed, vtkSmartPointer<vtkPolyData> mesh, std::map<int, int> clusterMap, std::set<int>* borderPoints);

	//Draw Borders
	static std::vector<vtkActor*> drawBorders(vtkSmartPointer<vtkPolyData> mesh, std::map<int, int> clusterMap);

	//Method for hiearchical Clustering with Average Distances, Output is the top Node of the hierarchiTree
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**")) Node* hierarchicalClustering(Liver* liver, vtkDoubleArray* distances, linkage link);
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**")) void changeClusterDistances(linkage link, std::vector<connections>* allConnections, int i, vtkDoubleArray* distances, Node* base);
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**")) static std::vector<Clustering::connections>* getNeighborDistances(Liver* liver, vtkDoubleArray* distances);
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**")) static void updateClusterDistance(std::vector<connections>* allConnections, std::vector<int>* allPointsFromCluster, int index, vtkDoubleArray* distances, linkage link);

	//functions for Getting the Cluster Distances based on Average Distances
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**"))static double getAverageClusterDistance(std::vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point);
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**"))static double getMaximumClusterDistance(std::vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point);
	__declspec(deprecated("**Deprecated Funtion: Based on Average Feature Distance**"))static double getMinimumClusterDistance(std::vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point);

	//Debug Methods
	void createTestTree(Node* base, int depth );
signals:
	void updateProgressBar(int value, int link);


};


#endif