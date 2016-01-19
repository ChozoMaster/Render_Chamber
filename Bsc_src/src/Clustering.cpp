// Vamco:
// Author: Dennis Basgier
// 20.01.2013

#include "Clustering.h"
#include "TreeStructure.h"

using namespace std;





bool compareConnections(connections a,	connections b){
	return (a.distance < b.distance);
}





void Clustering::updateClusterDistance(std::vector<connections>* allConnections, vector<int>* allPointsFromCluster, int index, vtkDoubleArray* distances, linkage link){
	map<int, data_t> ::const_iterator itr;
	allConnections->erase(allConnections->begin() + 1, allConnections->begin() + index + 1);
	set<int>* allNeighbors = new set<int>();
	for (int i = 0; i < allPointsFromCluster->size(); i++)
	{


		for (int j = 0; j < allConnections->size(); j++)
		{


			if((allConnections->at(j).A == allPointsFromCluster->at(i)))
				allNeighbors->insert(allConnections->at(j).B);

			if((allConnections->at(j).B == allPointsFromCluster->at(i)))
				allNeighbors->insert(allConnections->at(j).A);

		}
	}

	set<int>::const_iterator itr2;
	for(itr2 = allNeighbors->begin(); itr2 != allNeighbors->end(); ++itr2)
	{
		double distanceToCluster;
		switch(link)
		{
		case average:{
			distanceToCluster = getAverageClusterDistance(allPointsFromCluster, distances, *itr2);
			break; }

		case maximum:{

			distanceToCluster = getMaximumClusterDistance(allPointsFromCluster, distances, *itr2);
			break; }

		case minimum:{

			distanceToCluster = getMinimumClusterDistance(allPointsFromCluster, distances, *itr2);
			break; }


		}

		bool temp = true;
		for (int j = 0; j < allPointsFromCluster->size(); j++)
		{
			vector<connections>::iterator itr3;
			for(itr3 = allConnections->begin(); itr3 != allConnections->end(); ++itr3)
			{
				if((itr3->A == *itr2 && itr3->B == allPointsFromCluster->at(j)) || (itr3->B == *itr2 && itr3->A == allPointsFromCluster->at(j))){
					if(temp){
						itr3->distance = distanceToCluster;
						temp= false;
					}
					else{
						allConnections->erase(itr3);
					}
					break;
				}
			}



		}

	}
	sort(allConnections->begin(), allConnections->end(), compareConnections);
}


int Clustering::updateClusterDistance(std::map<int, data_t> PointClusterIDs, std::list<connections>* allConnections, vector<int>* allPointsFromCluster,  std::map<int, std::vector<double> > SegHistogramData, linkage link){
	map<int, data_t> ::const_iterator itr;
	int erasedObjects = 0;
	set<int>* allNeighbors = new set<int>();
	for (int i = 0; i < allPointsFromCluster->size(); i++)
	{


		for (std::list<connections>::iterator it = allConnections->begin(); it != allConnections->end(); ++it)
		{


			if((it->A == allPointsFromCluster->at(i))){
				if(PointClusterIDs[it->B].value == PointClusterIDs[allPointsFromCluster->at(i)].value){
					it = allConnections->erase(it);
					erasedObjects++;
					if((it != allConnections->begin()) && (it != allConnections->begin()))
						--it;
					break;
				}else{
					switch(link)
					{
					case ChebyshevFeatureCompleteLinkage:{
						it->distance = getHisMaxMaxClusterDistance(allPointsFromCluster, SegHistogramData, it->B);
						break; }

					case ChebyshevFeatureCentroidLinkage:{
						it->distance = getHisMaxAvgClusterDistance(allPointsFromCluster, SegHistogramData, it->B);
						break; }

					case EuklidFeatureCompleteLinkage:{

						it->distance = getHisEuclidianMaxClusterDistance(allPointsFromCluster, SegHistogramData, it->B);
						break; }

					case EuklidFeatureCentroidLinkage:{

						it->distance = getHisEuclidianAvgClusterDistance(allPointsFromCluster, SegHistogramData, it->B);
						break; }


					}
				}
			}

			if((it->B == allPointsFromCluster->at(i))){
				if(PointClusterIDs[it->A].value == PointClusterIDs[allPointsFromCluster->at(i)].value){	
					it = allConnections->erase(it);	
					erasedObjects++;
					if((it != allConnections->begin()) && (it != allConnections->begin()))
						--it;

					break;
				}else{
					switch(link)
					{
					case ChebyshevFeatureCompleteLinkage:{
						it->distance = getHisMaxMaxClusterDistance(allPointsFromCluster, SegHistogramData, it->A);
						break; }

					case ChebyshevFeatureCentroidLinkage:{
						it->distance = getHisMaxAvgClusterDistance(allPointsFromCluster, SegHistogramData, it->A);
						break; }

					case EuklidFeatureCompleteLinkage:{

						it->distance = getHisEuclidianMaxClusterDistance(allPointsFromCluster, SegHistogramData, it->A);
						break; }

					case EuklidFeatureCentroidLinkage:{

						it->distance = getHisEuclidianAvgClusterDistance(allPointsFromCluster, SegHistogramData, it->A);
						break; }


					}
				}
			}
		}
	}
	return erasedObjects;
}

double Clustering::getAverageClusterDistance(vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point){
	double avgDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		avgDistance = avgDistance + abs(distances->GetValue(allPointsFromCluster->at(j)) - distances->GetValue(point));



	}
	return (avgDistance / allPointsFromCluster->size());
}

double Clustering::getHisEuclidianAvgClusterDistance(vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point){
	double avgDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double temp = 0.0;
		for(int i = 0; i < SegHistogramData[j].size(); i++){
			double a = abs(SegHistogramData[allPointsFromCluster->at(j)].at(i) - SegHistogramData[point].at(i));
			temp = temp + (a*a);
		}
		temp = sqrt(temp);
		avgDistance = avgDistance + temp;
	}
	return (avgDistance / allPointsFromCluster->size());
}

double Clustering::getHisMaxAvgClusterDistance(vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point){
	double avgDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double maxDist = 0.0;
		for(int i = 0; i < SegHistogramData[j].size(); i++){
			double temp = abs(SegHistogramData[allPointsFromCluster->at(j)].at(i) - SegHistogramData[point].at(i));
			if(maxDist = temp)
				maxDist = temp;
		}
		avgDistance = avgDistance + maxDist;
	}
	return (avgDistance / allPointsFromCluster->size());
}


double Clustering::getHisEuclidianMaxClusterDistance(vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point){
	double maxDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double temp = 0.0;
		for(int i = 0; i < SegHistogramData[j].size(); i++){
			double a = abs(SegHistogramData[allPointsFromCluster->at(j)].at(i) - SegHistogramData[point].at(i));
			temp = temp + (a*a);
		}
		temp = sqrt(temp);
		if(maxDistance < temp)
			maxDistance = temp;
	}
	return maxDistance;
}


double Clustering::getHisMaxMaxClusterDistance(vector<int>* allPointsFromCluster, std::map<int, std::vector<double> > SegHistogramData, int point){
	double maxDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double maxDist = 0.0;
		for(int i = 0; i < SegHistogramData[j].size(); i++){
			double temp = abs(SegHistogramData[allPointsFromCluster->at(j)].at(i) - SegHistogramData[point].at(i));
			if(maxDist < temp)
				maxDist = temp;
		}
		if(maxDistance < maxDist)
			maxDistance = maxDist;
	}
	return maxDistance;
}


double Clustering::getMaximumClusterDistance(vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point){
	double maxDistance = 0.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double temp = abs(distances->GetValue(allPointsFromCluster->at(j)) - distances->GetValue(point));
		if(maxDistance < temp)
			maxDistance = temp;



	}
	return maxDistance;
}

double Clustering::getMinimumClusterDistance(vector<int>* allPointsFromCluster, vtkDoubleArray* distances, int point){
	//TODO HARD CODED BAD!
	double minDistance = 1000.0;

	for (int j = 0; j < allPointsFromCluster->size(); j++)
	{
		double temp = abs(distances->GetValue(allPointsFromCluster->at(j)) - distances->GetValue(point));
		if(minDistance > temp)
			minDistance = temp;



	}
	return minDistance;
}


void Clustering::changeClusterDistances(linkage link, std::vector<connections>* allConnections, int i, vtkDoubleArray* distances, Node* base){


	switch(link)
	{
	case average:{
		std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
		getAllClusterVector(base, -1, allClusters);
		updateClusterDistance(allConnections, &allClusters->back(), i, distances, link);
		break; }

	case maximum:{
		std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
		getAllClusterVector(base, -1, allClusters);
		updateClusterDistance(allConnections, &allClusters->back(),  i, distances, link);
		break; }

	case minimum:{
		std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
		getAllClusterVector(base, -1, allClusters);
		updateClusterDistance(allConnections, &allClusters->back(),  i, distances, link);
		break; }

	case fastTest:
		break;
	}
}	

int Clustering::changeClusterDistances(std::map<int, data_t> PointClusterIDs, linkage link, std::list<connections>* allConnections, std::map<int, std::vector<double> > SegHistogramData, Node* base){

	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
	getAllClusterVector(base, -1, allClusters);
	int erasedObjectsNumber = updateClusterDistance(PointClusterIDs, allConnections, &allClusters->back(), SegHistogramData, link);
	return erasedObjectsNumber;
}	



Node* Clustering::hierarchicalClustering(Liver* liver, vtkDoubleArray* distances,  linkage link){
	std::vector<connections>* allConnections = getNeighborDistances(liver,distances);
	int clusterCount = 0;
	//first int is Point ID, 2nd int is Cluster ID
	map<int, data_t> PointClusterIDs;// = new map<int, data_t>();
	double baseSize = allConnections->size();
	double counter = 0;
	double lastValue = 0.0;
	map<int, Node*> ClusterMap;
	for(int i = 0; i < allConnections->size(); i++){
		lastValue = 1.0 - ((double) allConnections->size()/ baseSize);
		int b = lastValue * 100;
		emit updateProgressBar(lastValue * 100, link);
		counter = counter + 1;
		map<int, data_t> ::const_iterator itr;
		if(((PointClusterIDs[allConnections->at(i).A].value	== -1) && !(PointClusterIDs[allConnections->at(i).B].value	== -1)) || (!(PointClusterIDs[allConnections->at(i).A].value	== -1) && (PointClusterIDs[allConnections->at(i).B].value	== -1))){
			//If one of the Points are in the cluster

			if(!(PointClusterIDs[allConnections->at(i).A].value	== -1)){
				// If point A is in Cluster
				int leftID = PointClusterIDs[allConnections->at(i).A].value;
				//	int rightID = PointClusterIDs[allConnections->at(i).B].value;

				Node* right = new Node();;
				right->ID = allConnections->at(i).B;

				Node* base = new Node();
				base->hierarchyLevel = allConnections->at(i).distance;
				/*base.left = ClusterMap[leftID].copyNode();*/
				base->left = ClusterMap[leftID];
				base->right = right;

				//ClusterMap[leftID] = base;
				ClusterMap[ClusterMap.size()] = base;
				data_t clusterID;
				clusterID.value = (int) ClusterMap.size() - 1;
				//PointClusterIDs[allConnections->at(i).B] =  clusterID;
				PointClusterIDs[allConnections->at(i).B] =  clusterID;
				changePointClusterIDs(&PointClusterIDs, leftID, (int) ClusterMap.size() - 1);
				clusterCount = (int) ClusterMap.size();

				changeClusterDistances(link, allConnections, i, distances, base);
				i = 0;

				//	ClusterMap.erase(rightID);

			}
			else{
				// If point B is in Cluster

				int leftID = PointClusterIDs[allConnections->at(i).B].value;
				//	int rightID = PointClusterIDs[allConnections->at(i).B].value;

				Node* right = new Node();
				right->ID = allConnections->at(i).A;

				Node* base = new Node();
				base->hierarchyLevel = allConnections->at(i).distance;
				/*base.left = ClusterMap[leftID].copyNode();*/
				base->left = ClusterMap[leftID];
				base->right = right;

				//	ClusterMap[leftID] = base;
				ClusterMap[(int) ClusterMap.size()] = base;

				data_t clusterID;
				clusterID.value = (int) ClusterMap.size() - 1;
				//PointClusterIDs[allConnections->at(i).A] =  clusterID;
				PointClusterIDs[allConnections->at(i).A] = clusterID;
				clusterCount = (int) ClusterMap.size();
				changePointClusterIDs(&PointClusterIDs, leftID, (int) ClusterMap.size() - 1);
				changeClusterDistances(link, allConnections, i, distances, base);
				i = 0;
				//	ClusterMap.erase(rightID);
			}
		}
		else{
			if((PointClusterIDs[allConnections->at(i).A].value	== -1) && (PointClusterIDs[allConnections->at(i).B].value	== -1)){
				//If none of the Points are in the cluster
				Node* left = new Node();
				left->ID = allConnections->at(i).A;
				Node* right = new Node();;
				right->ID = allConnections->at(i).B;
				//ClusterMap[clusterCount] = new Node();
				Node* base = new Node();
				base->hierarchyLevel = allConnections->at(i).distance;
				base->left = left;
				base->right = right;

				ClusterMap[clusterCount]= base;
				data_t clusterID;
				clusterID.value = clusterCount;
				clusterCount = clusterCount + 1;
				PointClusterIDs[allConnections->at(i).A] =  clusterID;
				PointClusterIDs[allConnections->at(i).B] =  clusterID;
				std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
				changeClusterDistances(link, allConnections, i, distances, base);
				i = 0;

			}
			else{
				//If both of the Points are in the cluster
				int leftID = PointClusterIDs[allConnections->at(i).A].value;
				int rightID = PointClusterIDs[allConnections->at(i).B].value;

				if(rightID != leftID){
					Node* base = new Node();
					base->hierarchyLevel = allConnections->at(i).distance;
					base->left = ClusterMap[leftID];
					base->right = ClusterMap[rightID];

					//	ClusterMap[leftID] = base;
					ClusterMap[(int) ClusterMap.size()] = base;
					changePointClusterIDs(&PointClusterIDs, rightID, (int) ClusterMap.size()-1);
					changePointClusterIDs(&PointClusterIDs, leftID, (int) ClusterMap.size()-1);
					clusterCount = (int) ClusterMap.size();
					changeClusterDistances(link, allConnections, i, distances, base);
					i = 0;
				}
			}
		}

	}
	map<int, data_t>::const_iterator itr;
	itr = PointClusterIDs.begin();
	return ClusterMap[itr->second.value];


}



Node* Clustering::hierarchicalClustering(Liver* liver, std::map<int, std::vector<double> > FeatureVectors,  linkage link){
	std::list<connections>* allConnections = getNeighborDistances(liver,FeatureVectors, link);
	int clusterCount = 0;
	//first int is Point ID, 2nd int is Cluster ID
	map<int, data_t> PointClusterIDs;
	double baseSize = allConnections->size();
	double counter = 0;
	int numberOfMerges = 0;
	double lastValue = 0.0;
	int connectionsSize = baseSize;
	map<int, Node*> ClusterMap;
	cout << "\n Points in Seghist: " << FeatureVectors.size() << "\n";
	for (std::list<connections>::iterator it = allConnections->begin(); it != allConnections->end(); ++it){
		lastValue = 1.0 - (connectionsSize/ baseSize);
		int a = lastValue * 100;
		emit updateProgressBar(lastValue * 100, link);
		counter = counter + 1;
		map<int, data_t> ::const_iterator itr;
		//TODO BAD HARD CODED VALUE
		double minDistance = 10000000;
		connections temp;
		std::list<connections>::iterator indexToErease;
		for (std::list<connections>::iterator it2 = allConnections->begin(); it2 != allConnections->end()   ; ++it2){
			if(minDistance > it2->distance){
				indexToErease = it2;
				temp = *it2;
				minDistance = it2->distance;
			}
		}
		if(((PointClusterIDs[temp.A].value	== -1) && !(PointClusterIDs[temp.B].value	== -1)) || (!(PointClusterIDs[temp.A].value	== -1) && (PointClusterIDs[temp.B].value	== -1))){
			//If one of the Points are in the cluster

			if(!(PointClusterIDs[temp.A].value	== -1)){
				// If point A is in Cluster
				int leftID = PointClusterIDs[temp.A].value;

				Node* right = new Node();
				right->ID = temp.B;

				Node* base = new Node();


				base->hierarchyLevel = numberOfMerges;
				numberOfMerges++;
				map<int, Node*>::iterator mItL;
				mItL = ClusterMap.find(leftID);
				base->left = mItL->second->copyNode();
				base->right = right;
				mItL->second = base;
				data_t clusterID;
				clusterID.value = leftID;
				PointClusterIDs[temp.B] =  clusterID;
				it = allConnections->erase(indexToErease);
				connectionsSize--;
				connectionsSize -= changeClusterDistances(PointClusterIDs, link, allConnections, FeatureVectors, base);
				if(allConnections->empty()){
					break;
				}else{
					it = allConnections->begin();
				}
			}
			else{
				// If point B is in Cluster

				int leftID = PointClusterIDs[temp.B].value;
				Node* right = new Node();
				right->ID = temp.A;

				Node* base = new Node();
				base->hierarchyLevel = numberOfMerges;
				numberOfMerges++;
				map<int, Node*>::iterator mItL;
				mItL = ClusterMap.find(leftID);
				base->left = mItL->second->copyNode();
				base->right = right;
				mItL->second = base;
				data_t clusterID;
				clusterID.value = leftID;
				PointClusterIDs[temp.A] =  clusterID;
				it = allConnections->erase(indexToErease);
				connectionsSize--;
				connectionsSize -= changeClusterDistances(PointClusterIDs, link, allConnections, FeatureVectors, base);
				if(allConnections->empty()){
					break;
				}else{
					it = allConnections->begin();
				}
			}
		}
		else{
			if((PointClusterIDs[temp.A].value	== -1) && (PointClusterIDs[temp.B].value	== -1)){
				//If none of the Points are in the cluster
				Node* left = new Node();
				left->ID = temp.A;
				Node* right = new Node();;
				right->ID = temp.B;
				Node* base = new Node();
				base->hierarchyLevel = numberOfMerges;
				numberOfMerges++;
				base->left = left;
				base->right = right;
				ClusterMap.insert(std::pair<int,Node*>(clusterCount,base) );
				data_t clusterID;
				clusterID.value = clusterCount;
				clusterCount = clusterCount + 1;
				PointClusterIDs[temp.A] =  clusterID;
				PointClusterIDs[temp.B] =  clusterID;
				std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
				it = allConnections->erase(indexToErease);
				connectionsSize--;
				connectionsSize -= changeClusterDistances(PointClusterIDs, link, allConnections, FeatureVectors, base);
				if(allConnections->empty()){
					break;
				}else{
					it = allConnections->begin();
				}


			}
			else{
				//If both of the Points are in the cluster
				int leftID = PointClusterIDs[temp.A].value;
				int rightID = PointClusterIDs[temp.B].value;

				if(rightID != leftID){
					Node* base = new Node();
					base->hierarchyLevel = numberOfMerges;
					numberOfMerges++;
					map<int, Node*>::iterator mItL;
					map<int, Node*>::iterator mItR;
					mItL = ClusterMap.find(leftID);
					mItR = ClusterMap.find(rightID);
					base->left = mItL->second->copyNode();
					base->right = mItR->second->copyNode();
					ClusterMap.erase(mItR);
					mItL->second = base;
					changePointClusterIDs(&PointClusterIDs, rightID, leftID);
					it = allConnections->erase(indexToErease);
					connectionsSize--;
					connectionsSize -= changeClusterDistances(PointClusterIDs, link, allConnections, FeatureVectors, base);
					if(allConnections->empty()){
						break;
					}else{
						it = allConnections->begin();
					}
				}
				else{

					it = allConnections->erase(indexToErease);
					connectionsSize--;
					it = allConnections->begin();
				}
			}
		}

	}
	map<int, data_t>::const_iterator itr;
	itr = PointClusterIDs.begin();
	return ClusterMap[itr->second.value];


}

void Clustering::getAllClusterVector(Node* base, double maxDistance, vector< vector<int> >* allClusters){
	int counter = 0;
	if(base->ID == -1){
		if(maxDistance == -1){
			vector<int>* clusterVector = new vector<int>();
			traverseTree(base, clusterVector);
			allClusters->push_back(*clusterVector);

		}
		else{
			if(base->hierarchyLevel > maxDistance){
				getAllClusterVector(base->left,maxDistance, allClusters);
				getAllClusterVector(base->right, maxDistance, allClusters);
			}
			else{
				vector<int>* clusterVector = new vector<int>();
				traverseTree(base, clusterVector);
				allClusters->push_back(*clusterVector);
			}
		}
	}
	else{
		vector<int>* clusterVector3 = new vector<int>();
		clusterVector3->push_back(base->ID);
		allClusters->push_back(*clusterVector3);
	}

}

void Clustering::traverseTree(Node* base, vector<int>* cluster ){
	if(base->ID != -1){
		cluster->push_back(base->ID);
	}
	else{
		traverseTree(base->left, cluster);
		traverseTree(base->right, cluster);
	}
}

void Clustering::createTestTree(Node* base, int depth ){
	if(depth){
		Node* left = new Node();
		left->hierarchyLevel = base->hierarchyLevel / 2;
		Node* right = new Node();
		right->hierarchyLevel = base->hierarchyLevel / 2;
		base->left = left;
		base->right = right;
		createTestTree(left, depth-1);
		createTestTree(right, depth-1);


	}
	else{
		Node* left = new Node();
		left->ID = 1;
		Node* right = new Node();
		right->ID = 1;
		base->left = left;
		base->right = right;


	}
}
void Clustering::changePointClusterIDs(map<int, data_t>* clusterIds, int oldClusterID, int newClusterID){
	map<int, data_t>::iterator itr;
	for(itr = clusterIds->begin(); itr != clusterIds->end(); ++itr){
		if(itr->second.value == oldClusterID)
			itr->second.value = newClusterID;
	}
}
std::vector<connections>* Clustering::getNeighborDistances(Liver* liver, vtkDoubleArray* distances){

	vtkPolyData * copy2 = vtkPolyData::New();
	copy2->DeepCopy(liver->getData());
	std::vector<connections>* allConnections = new std::vector<connections>();
	for(int i = 0; i < copy2->GetNumberOfPoints(); i++)	{
		set<int>* cellPointIds =	new set<int>();	
		set<int>::const_iterator itr;
		GetConnectedVertices(copy2, i, cellPointIds);
		for(itr = cellPointIds->begin(); itr != cellPointIds->end(); ++itr){

			if(i < *itr){
				connections temp = { abs(distances->GetValue(i) - distances->GetValue(*itr)), i, *itr};
				allConnections->push_back(temp);
			}
		}
	}
	sort(allConnections->begin(), allConnections->end(), compareConnections);
	return allConnections;
}

std::list<connections>* Clustering::getNeighborDistances(Liver* liver, std::map<int, std::vector<double> > SegHistogramData, linkage link){

	vtkPolyData * copy2 = vtkPolyData::New();
	copy2->DeepCopy(liver->getData());
	std::list<connections>* allConnections = new std::list<connections>();
	for(int i = 0; i < copy2->GetNumberOfPoints(); i++)	{
		set<int>* cellPointIds =	new set<int>();	
		set<int>::const_iterator itr;
		GetConnectedVertices(copy2, i, cellPointIds);
		for(itr = cellPointIds->begin(); itr != cellPointIds->end(); ++itr){

			if(i < *itr){
				switch (link)
				{
				case ChebyshevFeatureCentroidLinkage:
					{
						double maxDist = 0.0;
						for(int k = 0; k < SegHistogramData[i].size(); k++){
							double t = abs(SegHistogramData[i].at(k) - SegHistogramData[*itr].at(k));
							if(maxDist < t)
								maxDist = t;
						}
						connections temp = { maxDist, i, *itr};
						allConnections->push_back(temp);


					}

				case ChebyshevFeatureCompleteLinkage:
					{
						double maxDist = 0.0;
						for(int k = 0; k < SegHistogramData[i].size(); k++){
							double t = abs(SegHistogramData[i].at(k) - SegHistogramData[*itr].at(k));
							if(maxDist < t)
								maxDist = t;
						}
						connections temp = { maxDist, i, *itr};
						allConnections->push_back(temp);


					}

				case EuklidFeatureCompleteLinkage:
					{
						double sum = 0.0;
						for(int k = 0; k < SegHistogramData[i].size(); k++){
							double a = abs(SegHistogramData[i].at(k) - SegHistogramData[*itr].at(k));
							sum = sum + a * a;
						}
						sum = sqrt(sum);
						connections temp = { sum, i, *itr};
						allConnections->push_back(temp);


					}
				case EuklidFeatureCentroidLinkage:
					{
						double sum = 0.0;
						for(int k = 0; k < SegHistogramData[i].size(); k++){
							double a = abs(SegHistogramData[i].at(k) - SegHistogramData[*itr].at(k));
							sum = sum + a * a;
						}
						sum = sqrt(sum);
						connections temp = { sum, i, *itr};
						allConnections->push_back(temp);


					}
				}


			}
		}
	}
	return allConnections;
}


void Clustering::GetConnectedVertices(vtkSmartPointer<vtkPolyData> mesh, int seed, set<int>* connectedVertices)
{

	vtkSmartPointer<vtkIdList> cellIdList = vtkSmartPointer<vtkIdList>::New();	//get all connected cells 
	mesh->GetPointCells(seed, cellIdList);
	for(vtkIdType i = 0; i < cellIdList->GetNumberOfIds(); i++){		// loop through each cell using the seed point
		vtkCell* cell = mesh->GetCell(cellIdList->GetId(i));		// get current cell 

		if(cell->GetNumberOfEdges() <= 0){				//if the cell doesn't have any edges, it is a line, so skip 
			continue;
		}

		for(vtkIdType e = 0; e < cell->GetNumberOfEdges(); e++)				// loop through each edge of the cell
		{


			vtkCell* edge = cell->GetEdge(e);					// get current edge
			vtkIdList* pointIdList = edge->GetPointIds();				// get list of points on edge

			if(pointIdList->GetId(0) == seed || pointIdList->GetId(1) == seed) // if one of the points on the edge are the vertex point
			{


				if(pointIdList->GetId(0) == seed)				// if first point on edge is vertex									
				{
					int temp = pointIdList->GetId(1);			// get second point from list						
					connectedVertices->insert(temp);
					// and insert it into connected point list													
				}
				else
				{
					int temp = pointIdList->GetId(0);			// get second point from list		
					connectedVertices->insert(temp);
					// else insert first point into connected point list
				}
			}
		}
	}	
}

std::map<int, int> Clustering::transformToClusterMap(std::vector< std::vector<int> >* allClusters){
	std::map<int, int> clusterMap;
	for(int i = 0; i < allClusters->size(); i++){
		for (int j = 0; j < allClusters->at(i).size(); j++)
		{
			clusterMap[allClusters->at(i).at(j)] = i;
		}
	}
	return clusterMap;
}

std::set<int>* Clustering::getClusterBorders(std::vector< std::vector<int> >* allClusters, Liver* liver){
	std::map<int, int> clusterMap = transformToClusterMap(allClusters);
	std::set<int>* borderPoints = new std::set<int>();
	for(int i = 0; i < allClusters->size(); i++){
		for (int j = 0; j < allClusters->at(i).size(); j++)
		{
			addClusterBorderPoints(allClusters->at(i).at(j), liver->getData(), clusterMap, borderPoints);
		}
	}


	return borderPoints;
}

void Clustering::addClusterBorderPoints(int seed, vtkSmartPointer<vtkPolyData> mesh, std::map<int, int> clusterMap, std::set<int>* borderPoints){
	std::set<int>* connectedVertices = new std::set<int>();
	GetConnectedVertices(mesh,seed, connectedVertices);
	int clusterID = clusterMap[seed];
	std::set<int>::const_iterator itr2;
	for(itr2 = connectedVertices->begin(); itr2 != connectedVertices->end(); ++itr2)
	{

		if(clusterID != clusterMap[*itr2]){
			borderPoints->insert(seed);
			break;
		}

	}
}

std::vector<vtkActor*> Clustering::drawBorders(vtkSmartPointer<vtkPolyData> mesh, std::map<int, int> clusterMap){
	vtkCellArray* Polys = mesh->GetPolys();

	vtkIdType npts, *pts;
	std::vector<vtkActor*> lineVector;

	Polys->InitTraversal();
	vtkSmartPointer<vtkCellArray> lines =
		vtkSmartPointer<vtkCellArray>::New();

	while(Polys->GetNextCell(npts, pts))
	{
		int ID = Polys->GetTraversalLocation();
		//	cout << npts << ", ";
		double a[3],b[3],c[3],ab[3],ac[3],bc[3];


		mesh->GetPoint(pts[0], a);
		mesh->GetPoint(pts[1], b);
		mesh->GetPoint(pts[2], c);

		ab[0] = b[0] + ((a[0] - b[0])/2);
		ab[1] = b[1] + ((a[1] - b[1])/2);
		ab[2] = b[2] + ((a[2] - b[2])/2);

		ac[0] = c[0] + ((a[0] - c[0])/2);
		ac[1] = c[1] + ((a[1] - c[1])/2);
		ac[2] = c[2] + ((a[2] - c[2])/2);

		bc[0] = c[0] + ((b[0] - c[0])/2);
		bc[1] = c[1] + ((b[1] - c[1])/2);
		bc[2] = c[2] + ((b[2] - c[2])/2);

		vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
		cellCentersFilter->SetInput(mesh);
		cellCentersFilter->Update();
		vtkPolyData* centers = cellCentersFilter->GetOutput();
		if(((clusterMap[pts[0]] != clusterMap[pts[1]]) && (clusterMap[pts[0]] != clusterMap[pts[2]])) && ((clusterMap[pts[1]] != clusterMap[pts[0]]) && (clusterMap[pts[1]] != clusterMap[pts[2]])) && ((clusterMap[pts[2]] != clusterMap[pts[1]]) && (clusterMap[pts[2]] != clusterMap[pts[0]]))){
			double center[] = {1,2,3};
			center[0] = (a[0] + b[0] + c[0]) / 3.0;
			center[1] = (a[1] + b[1] + c[1]) / 3.0;
			center[2] = (a[2] + b[2] + c[2]) / 3.0;
			vtkLineSource* line1 = vtkLineSource::New();
			line1->SetPoint1(ab[0],ab[1], ab[2]);
			line1->SetPoint2(center[0],center[1],center[2]);
			vtkPolyDataMapper* mapper1 = vtkPolyDataMapper::New();
			mapper1->SetInput(line1->GetOutput());
			vtkActor* actor1 = vtkActor::New();
			actor1->SetMapper(mapper1);
			actor1->GetProperty()->SetColor(0,0,0);
			lineVector.push_back(actor1);



			vtkLineSource* line2 = vtkLineSource::New();
			line2->SetPoint1(ac[0],ac[1], ac[2]);
			line2->SetPoint2(center[0],center[1],center[2]);
			vtkPolyDataMapper* mapper2 = vtkPolyDataMapper::New();
			mapper2->SetInput(line2->GetOutput());
			vtkActor* actor2 = vtkActor::New();

			actor2->SetMapper(mapper2);
			actor2->GetProperty()->SetColor(0,0,0);
			lineVector.push_back(actor2);


			vtkLineSource* line3 = vtkLineSource::New();
			line3->SetPoint1(bc[0],bc[1], bc[2]);
			line3->SetPoint2(center[0],center[1],center[2]);
			vtkPolyDataMapper* mapper3 = vtkPolyDataMapper::New();
			mapper3->SetInput(line3->GetOutput());
			vtkActor* actor3 = vtkActor::New();
			actor3->SetMapper(mapper3);
			actor3->GetProperty()->SetColor(0,0,0);
			lineVector.push_back(actor3);


		}
		else{
			if((clusterMap[pts[0]] != clusterMap[pts[1]]) && (clusterMap[pts[0]] != clusterMap[pts[2]])){

				vtkLineSource* line = vtkLineSource::New();
				line->SetPoint1(ab[0],ab[1], ab[2]);
				line->SetPoint2(ac[0],ac[1],ac[2]);



				vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
				mapper->SetInput(line->GetOutput());
				vtkActor* actor = vtkActor::New();
				actor->GetProperty()->SetColor(0,0,0);
				actor->SetMapper(mapper);
				lineVector.push_back(actor);
			}

			if((clusterMap[pts[1]] != clusterMap[pts[0]]) && (clusterMap[pts[1]] != clusterMap[pts[2]])){
				vtkLineSource* line = vtkLineSource::New();
				line->SetPoint1(ab[0],ab[1],ab[2]);
				line->SetPoint2(bc[0], bc[1],bc[2]);



				vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
				mapper->SetInput(line->GetOutput());
				vtkActor* actor = vtkActor::New();
				actor->GetProperty()->SetColor(0,0,0);
				actor->SetMapper(mapper);
				lineVector.push_back(actor);
			}

			if((clusterMap[pts[2]] != clusterMap[pts[1]]) && (clusterMap[pts[2]] != clusterMap[pts[0]])){
				vtkLineSource* line = vtkLineSource::New();
				line->SetPoint1(ac[0],ac[1],ac[2]);
				line->SetPoint2(bc[0],bc[1],bc[2]);



				vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
				mapper->SetInput(line->GetOutput());
				vtkActor* actor = vtkActor::New();
				actor->GetProperty()->SetColor(0,0,0);
				actor->SetMapper(mapper);
				lineVector.push_back(actor);
			}

		}
	}

	return lineVector;
}
