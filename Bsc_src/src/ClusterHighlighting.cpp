// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "ClusterHighlighting.h"




bool compareLowQuartilMinToMax(QuartilStruct a,	QuartilStruct b){
	return (a.lowQuartil < b.lowQuartil);
}

bool compareLowQuartilMaxToMin(QuartilStruct a,	QuartilStruct b){
	return (a.lowQuartil > b.lowQuartil);
}
bool compareHighQuartilMinToMax(QuartilStruct a,	QuartilStruct b){
	return (a.highQuartil < b.highQuartil);
}
bool compareHighQuartilMaxToMin(QuartilStruct a,	QuartilStruct b){
	return (a.highQuartil > b.highQuartil);
}
bool compareMedianMinToMax(QuartilStruct a,	QuartilStruct b){
	return (a.median < b.median);
}

bool compareMedianMaxToMin(QuartilStruct a,	QuartilStruct b){
	return (a.median > b.median);
}

bool compareAverageMaxToMin(QuartilStruct a,	QuartilStruct b){
	return (a.avg > b.avg);
}

bool compareAverageMinToMax(QuartilStruct a,	QuartilStruct b){
	return (a.avg < b.avg);
}


clusterHighlighting::clusterHighlighting(QWidget *parent, AverageLiver* avgLiver, double threshold, std::vector<int>* choice )

: QDialog(parent)
{
	PCAselection = *choice;
	parentWidget = parent;
	// setup UI
	ui.setupUi(this);
	QObject::connect(ui.highlightBt, SIGNAL(clicked()), this, SLOT(highlightClusters()));
	QObject::connect(ui.closeBt, SIGNAL(clicked()), this, SLOT(close()));
	QObject::connect(ui.resetBt, SIGNAL(clicked()), this, SLOT(resetGraph()));
	QObject::connect(ui.AbsoluteCB, SIGNAL(clicked()), this, SLOT(paintBoxPlot()));
	QObject::connect(ui.BoxCB, SIGNAL(clicked()), this, SLOT(paintBoxPlot()));
	QObject::connect(ui.MinMixCB, SIGNAL(clicked()), this, SLOT(paintBoxPlot()));
	//QObject::connect(ui.graphicsView, SIGNAL(sendClusterID(int)), this, SLOT(calculateIntraHistogramVariance(int)));
	QObject::connect(ui.graphicsView, SIGNAL(sendClusterID(int)), this, SLOT(calculateIntraHistogramQuartiles(int)));
	QObject::connect(ui.sortBt, SIGNAL(clicked()), this, SLOT(sortGraph()));
	QObject::connect(ui.AverageCB, SIGNAL(clicked()), this, SLOT(parallelCoordinatesSignal()));
	QObject::connect(ui.medianCB, SIGNAL(clicked()), this, SLOT(parallelCoordinatesSignal()));
	QObject::connect(ui.quartilesCB, SIGNAL(clicked()), this, SLOT(parallelCoordinatesSignal()));
	QObject::connect(ui.AbsolutParallelCB, SIGNAL(clicked()), this, SLOT(parallelCoordinatesSignal()));
	QObject::connect(ui.cluster_lw, SIGNAL(sendSortedVector(std::vector<int>)), this, SLOT(SortedParallelCoordinatesSignal(std::vector<int>)));
	QObject::connect(ui.cluster_lw, SIGNAL(	itemDoubleClicked ( QListWidgetItem *)), this, SLOT(changeClusterColor(QListWidgetItem *)));

	ui.sortCriteria_lw->setFocusPolicy(Qt::NoFocus);
	ui.sortValue_lw->setFocusPolicy(Qt::NoFocus);
	ui.cluster_lw->setFocusPolicy(Qt::NoFocus);
	ui.AverageCB->hide();
	ui.AbsolutParallelCB->hide();
	ui.medianCB->hide();
	ui.quartilesCB->hide();
	ui.AbsoluteCB->hide();
	ui.BoxCB->hide();
	ui.MinMixCB->hide();
	this->threshold = threshold;
	mode = 0;
	thresholdGlobalPosition = 0;
	/*this->setFixedHeight(FRAMEHEIGHT + 2*OFFSET);
	this->setFixedWidth(FRAMEWIDTH + 2*OFFSET);*/
	//this->setFixedHeight(700);
	//this->setFixedWidth(FRAMEWIDTH + 2*OFFSET);
	//ui.graphicsView_2->setFixedWidth(VIEWWIDTH+ 2*OFFSET);
	//ui.graphicsView_2->setFixedHeight(VIEWHEIGHT+ 2*OFFSET);
	//ui.graphicsView->setFixedWidth(VIEWWIDTH+ 2*OFFSET);
	//ui.graphicsView->setFixedHeight(VIEWHEIGHT+ 2*OFFSET);
	//
	//this->minimalCluster = minimalCluster;
	//this->avgCluster = avgCluster;
	//this->maximalCluster = maximalCluster;
	this->avgLiver = avgLiver;

}

void clusterHighlighting::resetGraph()
{
	//	ui.graphicsView->gScene->clearSelection();
	paintVCRChart();
	//emit sendThreshold(optimalThresh);

}

void clusterHighlighting::parallelCoordinatesSignal()
{
	paintParallelCoordinates(selectedClusterIDs, sortCluster, doSort, sortValue, sortCriteria, false);

}

void clusterHighlighting::SortedParallelCoordinatesSignal(std::vector<int> sortedVector)
{

	paintParallelCoordinates(sortedVector, sortCluster, doSort, sortValue, sortCriteria, false);

}
//TODO NO STRING COMPARSION!!
void clusterHighlighting::sortGraph(){

	doSort = true;
	QString  sortValueString;
	QString  sortCriteriaString;
	QString str1 = "Median";
	QString str2 = "Low Quartil";
	QString str3 = "High Quartil";
	QString str4 = "Min to Max";
	QString str5 = "Max to Min";
	QString str6 = "Average";


	QListWidgetItem*  clusterIDitem = ui.cluster_lw->currentItem();
	if(clusterIDitem == NULL){
		//cout << "ClusterIDItem NULL!" <<  ", ";
		doSort = false;
	}else{
		sortCluster = clusterIDitem->text().toInt();
	}

	cout << "SortClusterID: " << sortCluster << ", ";

	QListWidgetItem*  sortValueItem = ui.sortValue_lw->currentItem();
	if(sortValueItem == NULL){
		//cout << "ClusterValueItem NULL!" <<  ", ";
		doSort = false;

	}else{
		sortValueString = sortValueItem->text();
		if(sortValueString.contains(str1)){
			sortValue = 1;
		}else if(sortValueString.contains(str2)){
			sortValue = 2;
		}else if(sortValueString.contains(str3)){
			sortValue = 3;
		}else if(sortValueString.contains(str6)){
			sortValue = 4;
		}else{
			//	cout << "no Sort " <<  ", ";
			doSort = false;
		}
	}
	QListWidgetItem*  sortCriteriaItem = ui.sortCriteria_lw->currentItem();
	if(sortCriteriaItem == NULL){
		//	cout << "ClusterCriteriaItem NULL!" <<  ", ";
		doSort = false;
	}else{
		sortCriteriaString = sortCriteriaItem->text();
		if(sortCriteriaString.contains(str4)){
			sortCriteria = 1;
		}else if(sortCriteriaString.contains(str5)){
			sortCriteria = 2;
		}else{
			//cout << "no Sort " <<  ", ";
			doSort = false;
		}
	}


	//sortValue = ui.sortValue_lw->currentIndex();
	//sortCriteria = ui.sortCriteria_lw->currentIndex();

	//if(sortValueString != NULL){
	//
	//}else{
	//	cout << "no Sort at Value" <<  ", ";
	//	doSort = false;
	//}
	/*if(sortCriteriaString == NULL){

	}else{
	cout << "no Sort Criteria" <<  ", ";
	doSort = false;
	}*/
	cout << "sortValue " << sortValue << ", ";
	cout << "sortCriteria" << sortCriteria << ", ";
	paintParallelCoordinates(selectedClusterIDs, sortCluster, doSort, sortValue, sortCriteria, false);
}
void clusterHighlighting::highlightClusters()
{
	std::set<int>* clusterChoice = new std::set<int>();
	for(int i = 0; i < ui.graphicsView->gScene->selectedItems().size(); i++){
		for(int j = 0; j < coordsVec->size(); j++){
			QGraphicsEllipseItem* v = dynamic_cast<QGraphicsEllipseItem*>(ui.graphicsView->gScene->selectedItems().at(i));
			QRectF rect = v->rect();
			if((rect.x() + 2* RADIUS + 2 > coordsVec->at(j).x) && (rect.x() + RADIUS * 2 - 2 < coordsVec->at(j).x))
				clusterChoice->insert(j);
		}
	}
	//if(ui.graphicsView->frontRect.right() < 0){
	//	left = 0;
	//}
	//else{
	//	left = ui.graphicsView->frontRect.right() ;
	//}

	////if(ui.graphicsView->rearRect.left() > 600 + OFFSET){
	////	right = 600 ;
	////}
	////else{
	//	right = ui.graphicsView->rearRect.left();
	////}
	//for(int i = 0; i < coordsVec->size(); i++){
	//	//double left = ui.graphicsView->frontRect.left();
	//	//double right = ui.graphicsView->rearRect.left();
	//	if(left < coordsVec->at(i).x){
	//		if(coordsVec->at(i).x  < right){
	//		clusterChoice->insert(i);
	//		}
	//	}
	//}

	std::vector<int> test;
	std::set<int>::const_iterator itr;
	for(itr = clusterChoice->begin(); itr != clusterChoice->end(); ++itr){
		test.push_back(*itr);

	}

	selectedClusterIDs = test;


	sortValue = 0;
	sortCluster = 0;
	sortCriteria = 0;
	doSort = false;

	paintParallelCoordinates(test, 0 ,false, 0 , 0, true);
	emit chosenCluster(clusterChoice);
}

void clusterHighlighting::highlightClusters(std::set<int>* clusterChoice)
{
	ui.graphicsView->gScene->clearSelection();
	std::vector<int> test;
	std::set<int>::const_iterator itr;
	for(itr = clusterChoice->begin(); itr != clusterChoice->end(); ++itr){
		test.push_back(*itr);
		ui.graphicsView->ellipses->at(*itr)->setSelected(true);
	}
	ui.graphicsView->markSelected();
	selectedClusterIDs = test;


	sortValue = 0;
	sortCluster = 0;
	sortCriteria = 0;
	doSort = false;

	paintParallelCoordinates(test, 0 ,false, 0 , 0, true);
	emit chosenCluster(clusterChoice);
}

void clusterHighlighting::paintVRCindex(){
	std::vector<std::vector<double> > centroidVector = calculateClusterCentroids(ui.graphicsView->allClusters);
	double dataPoints = avgLiver->clusteringLiver->getData()->GetNumberOfPoints();
	double clusterCount = ui.graphicsView->allClusters->size();
	double VRC = (calculateIntraClusterVarianceMaxDistance(ui.graphicsView->allClusters, centroidVector) / calculateInterClusterVarianceMaxDistance( centroidVector)) * ((dataPoints - clusterCount) / (clusterCount - 1));
	QString str;

	//TODO BACK IN FOR VCR INDEX
	//ui.graphicsView->gScene->addText(str.setNum(VRC),QFont("Times",7))->setPos(QPointF(OFFSET + VIEWWIDTH - 50,5 ));
}

void clusterHighlighting::findBestThreshold(){
	emit sendThreshold(optimalThresh);
}


void clusterHighlighting::paintVCRChart(){
	mode = 1;
	VCRVector = runClusterSimulation(ui.ClusterCountspinBox->value());
	QBrush brush(Qt::SolidPattern);
	brush.setColor(QColor ( 255, 0, 0));

	QBrush brush2(Qt::SolidPattern);
	brush2.setColor(QColor (0 , 255, 0));

	QBrush brush3(Qt::SolidPattern);
	brush3.setColor(QColor ( 0, 0, 255));

	QGraphicsScene* gScene = new QGraphicsScene(this);
	ui.graphicsView_2->setScene(gScene);
	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, VIEWHEIGHT + 2*OFFSET);
	QPen Axispen = QPen(QBrush(Qt::gray),1.5,Qt::DashLine);
	//X-Axis
	gScene->addLine( OFFSET, OFFSET, OFFSET, VIEWHEIGHT + OFFSET, Axispen);
	//Y-Axis
	gScene->addLine( OFFSET, VIEWHEIGHT + OFFSET, VIEWWIDTH + OFFSET, VIEWHEIGHT + OFFSET, Axispen);
	//gScene->addRect(0,VIEWHEIGHT + OFFSET + 19, 10, 10, QPen(), brush);
	//gScene->addRect(0,VIEWHEIGHT + OFFSET + 36, 10, 10, QPen(), brush2);
	//gScene->addRect(0,VIEWHEIGHT + OFFSET + 51, 10, 10, QPen(), brush3);
	QGraphicsTextItem*  textItem1 =  gScene->addText("SD-Index",QFont("Times",14));
	textItem1->setPos(QPointF(15, -10));
	//textItem1->setDefaultTextColor(Qt::gray);

	QGraphicsTextItem*  textItem2 =  gScene->addText("Threshold (Nr. Clusters)",QFont("Times",14));
	textItem2->setPos(QPointF((VIEWWIDTH / 2) - 150, VIEWHEIGHT + OFFSET + 5));
	//textItem2->setDefaultTextColor(Qt::gray);

	double maxY = 0;
	//TODO BAD HARD CODED
	double minVRC = 1000000;
	//double maxX = VCRVector.at(VCRVector.size() - 1).first;
	maxX = VCRVector.back().clusterSize;
	double minX = VCRVector.at(0).clusterSize;
	for(int i = 0; i < VCRVector.size(); i++){
		if(VCRVector.at(i).VCR > maxY)
			maxY = VCRVector.at(i).VCR;

		/*if(VCRVector.at(i).density > maxY)
		maxY = VCRVector.at(i).density;

		if(VCRVector.at(i).separation > maxY)
		maxY = VCRVector.at(i).separation;*/

		if(VCRVector.at(i).VCR < minVRC){
			minVRC = VCRVector.at(i).VCR;
			optimalThresh = VCRVector.at(i).threshold;
			cout << "newOPtimal: " << threshold << " , ";
		}
	}

	QString str;
	//y-axis description
	//gScene->addText(str.setNum(maxY),QFont("Times",7))->setPos(QPointF(0,OFFSET - 18));

	double maxMinDifference = maxX;

	//for x-axis description
	//gScene->addText(str.setNum((int) 0),QFont("Times",7))->setPos(0 + OFFSET,OFFSET + VIEWHEIGHT );
	//gScene->addText(str.setNum((int) 1),QFont("Times",7))->setPos((1/maxX * VIEWWIDTH) + OFFSET,OFFSET + VIEWHEIGHT );

	int despriptionCount = 7;
	for(int i = 0; i < VCRVector.size() - 1; i++){
		double x1 = (VCRVector.at(i).clusterSize/maxX * VIEWWIDTH) + OFFSET;
		double x2 = (VCRVector.at(i + 1).clusterSize/maxX * VIEWWIDTH) + OFFSET;

		//Text for x-Axis, Cluster Numbers
		//gScene->addText(str.setNum((int) VCRVector.at(i).clusterSize),QFont("Times",7))->setPos(x1,OFFSET + VIEWHEIGHT );
		double y1V =	((1.0 - VCRVector.at(i).VCR/maxY) * VIEWHEIGHT) + OFFSET;
		double y2V =	((1.0 - VCRVector.at(i+ 1).VCR/maxY) * VIEWHEIGHT) + OFFSET;
		gScene->addLine(x1, y1V , x2, y2V,QPen(QColor ( 255, 0, 0)));
	}
	double actualClusterSize = ui.graphicsView->allClusters->size();
	//double tempX = (((threshold + activeCluster->hierarchyLevel)/maxX) * VIEWWIDTH) + OFFSET;
	double tempX = ((activeCluster->hierarchyLevel - threshold + 1)/maxX * VIEWWIDTH) + OFFSET;
	thresholdGlobalPosition = tempX;

	threshLine = gScene->addLine(tempX, OFFSET , tempX, OFFSET + VIEWHEIGHT,QPen(QColor ( 0, 0, 0)));
	

	//Draw Minimum
	/*
	double minLineY = ( (1.0 - (minVRC/maxY)) * VIEWHEIGHT) + OFFSET;
	gScene->addLine(OFFSET - 5, minLineY , OFFSET + 5, minLineY,QPen(QColor ( 0, 0, 0)));
	gScene->addText("Minimum:",QFont("Times",7))->setPos(QPointF(0,minLineY - 10));
	gScene->addText(str.setNum((double) minVRC),QFont("Times",7))->setPos(QPointF(0,minLineY));
	*/
}
void clusterHighlighting::repaintThreshline(int thresh){
	if(mode == 1) {
		cout << "\nThreshold:" << thresh << "\n";
		double threshdouble = thresh + 1;
		double tempX = (threshdouble/maxX * VIEWWIDTH) + OFFSET;
		double moveX =   tempX - thresholdGlobalPosition;
		thresholdGlobalPosition = tempX;
		//	double t1 = activeCluster->hierarchyLevel-thresh;
		//cout << "\nThreshold position:" << (threshdouble/maxX * VIEWWIDTH) + OFFSET<< "\n";
		//cout << "\nmove position:" << moveX << "\n";
		//cout << "\nThreshold Global position:" << thresholdGlobalPosition << "\n";
		//cout << "\nt1 position:" << t1 << "\n";
		threshLine->moveBy(moveX,0);
		ui.graphicsView_2->repaint();
	}
}
//Function with Hard Coded BlockSize
//void clusterHighlighting::paintCandleChart(){
//	QPen pen = QPen(QBrush(Qt::black),1.5,Qt::DashLine);
//	QPen pen2 = QPen(QBrush(Qt::black),1.5,Qt::SolidLine);
//	QBrush brush(Qt::SolidPattern);
//	brush.setColor(QColor ( 0, 255, 0));
//
//	QGraphicsScene* gScene = new QGraphicsScene(this);
//	ui.graphicsView_2->setScene(gScene);
//	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, VIEWHEIGHT + 2*OFFSET);
//	QPen Axispen = QPen(QBrush(Qt::gray),1.5,Qt::DashLine);
//	
//	gScene->addLine( OFFSET, OFFSET, OFFSET, VIEWHEIGHT + OFFSET, Axispen);
//	std::map<int, varianceStruct>::const_iterator itr2;
//		double varMax = 0;
//		double valueMax = 0;
//		double valueMin = 0;
//	for(itr2 = intraVariance.begin(); itr2 != intraVariance.end(); ++itr2){
//		if(varMax < itr2->second.variance)
//			varMax = itr2->second.variance;
//		if(valueMax < itr2->second.max)
//			valueMax = itr2->second.max;
//		if(valueMin > itr2->second.min)
//			valueMin = itr2->second.min;
//	}
//	double posHeight = 0;
//	double negHeight = 0;
//	if(valueMax < 0){
//		negHeight = 0;
//		posHeight = VIEWHEIGHT;
////	gScene->addLine( OFFSET, (VIEWHEIGHT + (2*OFFSET)) / 2, OFFSET + VIEWWIDTH, (VIEWHEIGHT + (2*OFFSET)) / 2, Axispen);
//	}else if(valueMin > 0){
//		negHeight = VIEWHEIGHT;
//		posHeight = 0;
//	}else if(abs(valueMax) > abs(valueMin)){
//		cout << "Max bigger then Min";
//	double ratio = 1.0 - abs(valueMin / valueMax);
//	posHeight = (VIEWHEIGHT * ratio);
//	negHeight = VIEWHEIGHT - posHeight;
//	}else{
//		cout << "Min bigger then Max";
//		double ratio = valueMax / valueMin;
//		posHeight = (ratio * VIEWHEIGHT );
//		negHeight = VIEWHEIGHT - posHeight;
//	}
//	gScene->addLine( OFFSET , OFFSET + posHeight, OFFSET + VIEWWIDTH, OFFSET + posHeight, Axispen);
//	
//	double offset = (VIEWWIDTH / intraVariance.size()) - BLOCKSIZE;
//	double x = OFFSET;
//
//	for(itr2 = intraVariance.begin(); itr2 != intraVariance.end(); ++itr2){
//	x = x + offset;
//	double blockHeight = (itr2->second.variance / varMax) * BLOCKHEIGHT;
//	double y = 0;
//	if(itr2->second.avg > 0){
//	y = ((1.0 - abs(itr2->second.avg/valueMax)) * posHeight) + OFFSET;
//	}
//	else{
//	y = ((abs(itr2->second.avg/valueMin)) * negHeight) + OFFSET + posHeight;
//	}
//	gScene->addRect(x - (BLOCKSIZE/2), y - (blockHeight/2),BLOCKSIZE, blockHeight, QPen(), brush);
//
//	double botLine = 0;
//	double topLine = 0;
//	if(itr2->second.min > 0){
//		botLine = (1.0 - (itr2->second.min/valueMax)) * posHeight;
//	}
//	else{
//		botLine = (abs(itr2->second.min/valueMin) * negHeight) + posHeight;
//	}
//
//	if(itr2->second.max < 0){
//		topLine = (abs(itr2->second.max/valueMin) * negHeight) + posHeight;
//	}
//	else{
//		topLine = (1.0 - abs(itr2->second.max/valueMax)) * posHeight;
//	}
//
//	//double topLine = (itr2->second.max/valueMax) * VIEWHEIGHT;
//	//double botLine = (itr2->second.min/valueMax) * VIEWHEIGHT;
//	//
//	//
//	gScene->addLine(x - (BLOCKSIZE/2), topLine, x + (BLOCKSIZE/2), topLine, pen2);
//	gScene->addLine(x - (BLOCKSIZE/2), botLine, x + (BLOCKSIZE/2), botLine, pen2);
//	gScene->addLine(x, topLine, x, botLine, pen);
//	
//	x = x + BLOCKSIZE;
//	}
//}

//Function with relative Blocksizes
void clusterHighlighting::paintCandleChart(int clusterID){
	QString str;
	QPen pen = QPen(QBrush(Qt::black),1.5,Qt::DashLine);
	QPen pen2 = QPen(QBrush(Qt::black),1.5,Qt::SolidLine);
	QPen pen3 = QPen(QBrush(Qt::gray),1.0,Qt::SolidLine);
	QPen pen4 = QPen(QBrush(Qt::red),2.0,Qt::SolidLine);
	QBrush brush(Qt::SolidPattern);
	brush.setColor(QColor ( 0, 255, 0));

	QGraphicsScene* gScene = new QGraphicsScene(this);
	ui.graphicsView_2->setScene(gScene);
	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, VIEWHEIGHT + 2*OFFSET);
	QPen Axispen = QPen(QBrush(Qt::gray),1.5,Qt::DashLine);

	//Y-axis
	gScene->addLine( OFFSET, OFFSET, OFFSET, VIEWHEIGHT + OFFSET, Axispen);
	std::map<int, varianceStruct>::const_iterator itr2;
	double varMax = 0;
	double valueMax = 0;
	double valueMin = 0;
	for(itr2 = intraVariance.begin(); itr2 != intraVariance.end(); ++itr2){
		if(varMax < itr2->second.variance)
			varMax = itr2->second.variance;
		if(valueMax < itr2->second.max)
			valueMax = itr2->second.max;
		if(valueMin > itr2->second.min)
			valueMin = itr2->second.min;
	}
	double posHeight = 0;
	double negHeight = 0;
	if(valueMax < 0){
		negHeight = 0;
		posHeight = VIEWHEIGHT;
		//	gScene->addLine( OFFSET, (VIEWHEIGHT + (2*OFFSET)) / 2, OFFSET + VIEWWIDTH, (VIEWHEIGHT + (2*OFFSET)) / 2, Axispen);
	}else if(valueMin > 0){
		negHeight = VIEWHEIGHT;
		posHeight = 0;
	}else if(abs(valueMax) > abs(valueMin)){
		cout << "Max bigger then Min";
		double ratio = 1.0 - abs(valueMin / valueMax);
		posHeight = (VIEWHEIGHT * ratio);
		negHeight = VIEWHEIGHT - posHeight;
	}else{
		cout << "Min bigger then Max";
		double ratio = valueMax / valueMin;
		posHeight = (ratio * VIEWHEIGHT );
		negHeight = VIEWHEIGHT - posHeight;
	}
	//X-Axis
	gScene->addLine( OFFSET , OFFSET + posHeight, OFFSET + VIEWWIDTH, OFFSET + posHeight, Axispen);

	double offset = (VIEWWIDTH / intraVariance.size()) - BLOCKSIZE;
	double x = OFFSET;
	double scale =  VIEWHEIGHT / (valueMax+valueMin);
	cout << "\n scale:" << scale << "\n";
	int liverID = 0;
	bool low = true;
	for(itr2 = intraVariance.begin(); itr2 != intraVariance.end(); ++itr2){	
		x = x + offset;
		//double blockHeight = (itr2->second.variance / varMax) * BLOCKHEIGHT;
		double blockHeight = itr2->second.variance  * scale;
		double y = 0;
		if(itr2->second.avg > 0){
			y = ((1.0 - abs(itr2->second.avg/valueMax)) * posHeight) + OFFSET;
		}
		else{
			y = ((abs(itr2->second.avg/valueMin)) * negHeight) + OFFSET + posHeight;
		}
		//QRectF newRect = QRectF(QPoint(x, ), QPoint(x + BLOCKSIZE));
		gScene->addRect(x - (BLOCKSIZE/2), y - (blockHeight/2),BLOCKSIZE, blockHeight, QPen(), brush);

		gScene->addText(QString::number(itr2->second.variance,'f', 2),QFont("Times",7))->setPos(QPointF(x - (BLOCKSIZE/2)- 3,OFFSET + 10 + VIEWHEIGHT + (low * 10)));
		//	gScene->addText(str.setNum(itr2->second.variance),QFont("Times",7))->setPos(QPointF(x - (BLOCKSIZE/2),OFFSET + 10 + VIEWHEIGHT + (low * 10)));
		double botLine = 0;
		double topLine = 0;
		if(itr2->second.min > 0){
			botLine = (1.0 - (itr2->second.min/valueMax)) * posHeight;
		}
		else{
			botLine = (abs(itr2->second.min/valueMin) * negHeight) + posHeight;
		}

		if(itr2->second.max < 0){
			topLine = (abs(itr2->second.max/valueMin) * negHeight) + posHeight;
		}
		else{
			topLine = (1.0 - abs(itr2->second.max/valueMax)) * posHeight;
		}

		//double topLine = (itr2->second.max/valueMax) * VIEWHEIGHT;
		//double botLine = (itr2->second.min/valueMax) * VIEWHEIGHT;
		//
		//

		//y-axis description
		gScene->addText(str.setNum(valueMax),QFont("Times",7))->setPos(QPointF(0,OFFSET - 10));
		gScene->addText(str.setNum(valueMin),QFont("Times",7))->setPos(QPointF(0,OFFSET + 10 + VIEWHEIGHT));


		for(int i = 0; i < ui.graphicsView->allClusters->at(clusterID).size(); i++){
			double pointValue = avgLiver->SegHistogramData[ui.graphicsView->allClusters->at(clusterID).at(i)].at(liverID);
			double y1 = 0;
			if(pointValue > 0){
				y1 = (1.0 - (pointValue/valueMax)) * posHeight;
			}
			else{
				y1 = (abs(pointValue/valueMin) * negHeight) + posHeight;
			}
			gScene->addLine(x - (BLOCKSIZE/2), y1, x + (BLOCKSIZE/2), y1, pen3);
		}

		gScene->addLine(x - (BLOCKSIZE/2), topLine, x + (BLOCKSIZE/2), topLine, pen2);
		gScene->addLine(x - (BLOCKSIZE/2), botLine, x + (BLOCKSIZE/2), botLine, pen2);
		gScene->addLine(x, topLine, x, botLine, pen);
		gScene->addLine(x - (BLOCKSIZE/2), y, x + (BLOCKSIZE/2), y, pen4);
		liverID++;
		x = x + BLOCKSIZE;
		low = !low;
	}
}

//TODO SORT CRITERIA SHOULD BE ENUMS
//Table:
//SortValue 1: Median
//SortValue 2: Low Quartil
//SortValue 3: Highquartil
//SortCriteria 1: Min To Max
//SortCriteria 2: Max To Min
void clusterHighlighting::changeClusterColor(QListWidgetItem * item){
	QColorDialog* colorDialog = new QColorDialog();
	if(QFileDialog::Accepted == colorDialog->exec()){
		int clusterIndex = item->text().toInt();
		QColor col = colorDialog->currentColor();
		col.setAlpha(128);
		QBrush brush3(Qt::SolidPattern);
		brush3.setColor(col);
		item->setBackground(brush3);
		emit sendColorChangeCluster(clusterIndex, col);
		colorDialog->close();
	}
}


void clusterHighlighting::paintParallelCoordinates(std::vector<int> Clusters, int clusterSortID, bool doSort, int sortValue, int sortCriteria, bool newLists){
	ui.AverageCB->show();
	ui.medianCB->show();
	ui.quartilesCB->show();
	ui.AbsolutParallelCB->show();
	ui.AbsoluteCB->hide();
	ui.BoxCB->hide();
	ui.MinMixCB->hide();
	std::vector<std::map<int, QuartilStruct > > allQuartils;
	std::vector<int> tempClusters;
	if(newLists){
		ui.cluster_lw->clear();
		ui.sortValue_lw->clear();
		ui.sortCriteria_lw->clear();
		//SortValue 1: Median
		//SortValue 2: Low Quartil
		//SortValue 1: Highquartil
		//SortCriteria 1: Min To Max
		//SortCriteria 1: Max To Min
		new QListWidgetItem(tr("Median"), ui.sortValue_lw), true;
		new QListWidgetItem(tr("Low Quartil"), ui.sortValue_lw);
		new QListWidgetItem(tr("High Quartil"), ui.sortValue_lw);
		new QListWidgetItem(tr("Average"), ui.sortValue_lw);

		new QListWidgetItem(tr("Min to Max"), ui.sortCriteria_lw), true;
		new QListWidgetItem(tr("Max to Min"), ui.sortCriteria_lw);
		for(int i = (int)Clusters.size() - 1; i >= 0; i-- ){
			tempClusters.push_back(Clusters.at(i));
		}
		//Clusters = tempClusters;
	}
	//TODO NOT GOOD
	double maxValue = -5000.0;
	double minValue = 5000.0;
	bool first = true;
	std::vector<QuartilStruct> quartilVector;
	for(int i = 0; i < Clusters.size(); i++){
		std::map<int, QuartilStruct > tempQuartil = calculateIntraHistogramQuartilesOut(Clusters.at(i));

		std::map<int, QuartilStruct>::const_iterator itr2;
		for(itr2 = tempQuartil.begin(); itr2 != tempQuartil.end(); ++itr2){
			if(doSort){
				if(itr2->second.clusterID == clusterSortID){
					quartilVector.push_back(itr2->second);
				}
			}else{
				if(first){
					quartilVector.push_back(itr2->second);
				}
			}
			if(ui.AbsolutParallelCB->isChecked()){
				if(itr2->second.max > maxValue){
					maxValue = itr2->second.max;
				}
				if(itr2->second.min < minValue){
					minValue = itr2->second.min;
				}
			} else{
				if(itr2->second.highQuartil > maxValue){
					maxValue = itr2->second.highQuartil;
				}
				if(itr2->second.lowQuartil < minValue){
					minValue = itr2->second.lowQuartil;
				}
			}
		}
		first = false;
		allQuartils.push_back(tempQuartil);
	}

	//SortValue 1: Median
	//SortValue 2: Low Quartil
	//SortValue 1: Highquartil
	//SortCriteria 1: Min To Max
	//SortCriteria 1: Max To Min
	if(doSort){
		switch(sortValue)
		{
		case 1:{
			switch(sortCriteria)
			{
			case 1:{
				sort(quartilVector.begin(), quartilVector.end(), compareMedianMinToMax);
				break;
				   }
			case 2:{
				sort(quartilVector.begin(), quartilVector.end(), compareMedianMaxToMin);
				break;
				   }
			}
			break;
			   }
		case 2:{
			switch(sortCriteria)
			{
			case 1:{
				sort(quartilVector.begin(), quartilVector.end(), compareLowQuartilMinToMax);
				break;
				   }
			case 2:{
				sort(quartilVector.begin(), quartilVector.end(), compareLowQuartilMaxToMin);
				break;
				   }
			}
			break;
			   }

		case 3:{
			switch(sortCriteria)
			{
			case 1:{
				sort(quartilVector.begin(), quartilVector.end(), compareHighQuartilMinToMax);
				break;
				   }
			case 2:{
				sort(quartilVector.begin(), quartilVector.end(), compareHighQuartilMaxToMin);
				break;
				   }
			}
			break;
			   }
		case 4:{
			switch(sortCriteria)
			{
			case 1:{
				sort(quartilVector.begin(), quartilVector.end(), compareAverageMinToMax);
				break;
				   }
			case 2:{
				sort(quartilVector.begin(), quartilVector.end(), compareAverageMaxToMin);
				break;
				   }
			}
			break;
			   }



		}
	}
	std::vector<int> order;
	for(int i = 0; i < quartilVector.size(); i++){
		order.push_back(quartilVector.at(i).liverID);
	}
	QString str;
	QPen pen = QPen(QBrush(Qt::black),1.5,Qt::DashLine);
	QPen pen2 = QPen(QBrush(Qt::black),1.5,Qt::SolidLine);
	QPen pen3 = QPen(QBrush(Qt::gray),1.0,Qt::SolidLine);
	QPen pen4 = QPen(QBrush(Qt::red),2.0,Qt::SolidLine);
	QBrush brush(Qt::SolidPattern);


	QGraphicsScene* gScene = new QGraphicsScene(this);
	ui.graphicsView_2->setScene(gScene);
	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, VIEWHEIGHT + 2*OFFSET);
	QPen Axispen = QPen(QBrush(Qt::gray),1.5,Qt::DashLine);

	//write Max and Min to graph
	QString minText;
	QString maxText;
	minText.append(str.setNum(minValue));
	minText.append(" mm");
	maxText.append(str.setNum(maxValue));
	maxText.append(" mm");
	gScene->addText(maxText,QFont("Times",7))->setPos(QPointF(OFFSET , 0  ));
	gScene->addText(minText,QFont("Times",7))->setPos(QPointF(OFFSET , OFFSET + VIEWHEIGHT  ));

	//Y-axis
	gScene->addLine( OFFSET, OFFSET, OFFSET, VIEWHEIGHT + OFFSET, Axispen);

	double posHeight = 0;
	double negHeight = 0;
	if(maxValue <= 0){
		negHeight = VIEWHEIGHT;
		posHeight = 0;
		//	gScene->addLine( OFFSET, (VIEWHEIGHT + (2*OFFSET)) / 2, OFFSET + VIEWWIDTH, (VIEWHEIGHT + (2*OFFSET)) / 2, Axispen);
	}else if(minValue >= 0){
		negHeight = 0;
		posHeight = VIEWHEIGHT;
	}else if(abs(maxValue) > abs(minValue)){
		cout << "Max bigger then Min";
		double ratio = abs(maxValue) / (abs(maxValue) + abs(minValue));
		posHeight = (VIEWHEIGHT * ratio);
		negHeight = VIEWHEIGHT - posHeight;
	}else{
		if(minValue == 0){
			cout << "BOTH ZERO!";
			posHeight =  VIEWHEIGHT / 2;
			negHeight = VIEWHEIGHT - posHeight;
		}else{
			cout << "Min bigger then Max";
			double ratio = abs(maxValue) / (abs(maxValue) + abs(minValue));
			posHeight = (ratio * VIEWHEIGHT );
			negHeight = VIEWHEIGHT - posHeight;
			/*posHeight = VIEWHEIGHT - posHeight;
			negHeight = (ratio * VIEWHEIGHT );*/

		}
	}
	
	//		double offset = (VIEWWIDTH / allQuartils.at(0).size()) - BLOCKSIZE;
	double offset = VIEWWIDTH / 2;
	if(allQuartils.at(0).size() - 1 != 0)
		offset = (VIEWWIDTH / (allQuartils.at(0).size() - 1));

	/*double scale =  VIEWHEIGHT / (valueMax+valueMin);
	cout << "\n scale:" << scale << "\n";*/
	int liverID = 0;
	bool low = true;
	bool paintLiverIDs = true;
	std::map<int, QuartilStruct>::const_iterator itr2;
	QPen myPen = QPen(QBrush(Qt::gray),2.0,Qt::SolidLine);
	QPen myPen2 = QPen(QBrush(Qt::gray),1.0,Qt::SolidLine);
	//TODO BAD HARD CODED SIZE!!
	std::pair<double, double> QuartilCoords[20];
	double MedianCoords[20];
	double averageCoords[20];
	std::vector<double> absolute[20];

	for(int i = 0; i < allQuartils.size(); i++){

		bool first = true;


		double x = OFFSET;

		myPen.setColor(QColor ( coordsVec->at(Clusters.at(i)).rgb[0] * 255, coordsVec->at(Clusters.at(i)).rgb[1] * 255, coordsVec->at(Clusters.at(i)).rgb[2] * 255));
		myPen2.setColor(QColor ( coordsVec->at(Clusters.at(i)).rgb[0] * 255, coordsVec->at(Clusters.at(i)).rgb[1] * 255, coordsVec->at(Clusters.at(i)).rgb[2] * 255));
		brush.setColor(QColor ( coordsVec->at(Clusters.at(i)).rgb[0] * 255, coordsVec->at(Clusters.at(i)).rgb[1] * 255, coordsVec->at(Clusters.at(i)).rgb[2] * 255, 128));
		if(newLists){
			QBrush brush3(Qt::SolidPattern);
			QString clustLWStr; //= "ClusterID : ";
			clustLWStr.append(QString::number(tempClusters.at(i)));
			QListWidgetItem* widgetItem = new QListWidgetItem(clustLWStr, ui.cluster_lw);
			//	ui.cluster_lw->setItemSelected(widgetItem,true);
			brush3.setColor(QColor ( coordsVec->at(tempClusters.at(i)).rgb[0] * 255, coordsVec->at(tempClusters.at(i)).rgb[1] * 255, coordsVec->at(tempClusters.at(i)).rgb[2] * 255, 128));
			widgetItem->setBackground(brush3);



		}

		int counter = -1;
		for(itr2 = allQuartils.at(i).begin(); itr2 != allQuartils.at(i).end(); ++itr2){	
			counter++;
			int position = 0;
			for(int j = 0; j < order.size(); j++){
				if(order.at(j) == itr2->second.liverID){
					x = OFFSET + (j * offset);
					position = j;
					break;
				}

			}
			if(ui.AbsolutParallelCB->isChecked()){
				std::vector<double> absoluteValues;
				for(int k = 0; k < ui.graphicsView->allClusters->at(Clusters.at(i)).size(); k++){


					double tempValue = avgLiver->SegHistogramData[ui.graphicsView->allClusters->at(Clusters.at(i)).at(k)].at(counter);
					//	double tempValue = avgLiver->SegHistogramData[ui.graphicsView->allClusters->at(Clusters.at(i)).at(k)].at(itr2->second.liverID);
					double yLeft = 0;

					if(tempValue > 0){
						yLeft = ((1.0 - abs(tempValue/maxValue)) * posHeight) + OFFSET;
					}
					else{
						if(tempValue == 0){
							if(maxValue == 0){
								yLeft = OFFSET;
							}else{
								if(minValue == 0){
									yLeft = OFFSET +VIEWHEIGHT;
								}
							}

						}
						else{
							yLeft = ((abs(tempValue/minValue)) * negHeight) + OFFSET + posHeight;
						}
					}
					absoluteValues.push_back(yLeft);

				}
				absolute[position] = absoluteValues;
			}	

			//leftX1 = rightX1 + BLOCKSIZE;
			//leftX2 = rightX2 + BLOCKSIZE;
			//leftY1 = rightY1;
			//leftY2 = rightY2;
			if(paintLiverIDs){
				QString str = "ID ";
				//str.append(QString::number(itr2->second.clusterID));
				str.append(QString::number(itr2->second.liverID));

				//	str.append(itr2->second.liverID);
				gScene->addText(str,QFont("Times",7))->setPos(QPointF(x - (BLOCKSIZE/2)- 12,OFFSET + 10 + VIEWHEIGHT + (low * 10)));
			}
			//cout << "liverID: " <<  itr2->second.liverID;

			//double blockHeight = (itr2->second.variance / varMax) * BLOCKHEIGHT;
			//	double blockHeight = itr2->second.variance  * scale;
			double yTop = 0;
			if(itr2->second.highQuartil > 0){
				yTop = ((1.0 - abs(itr2->second.highQuartil/maxValue)) * posHeight) + OFFSET;
			}
			else{
				if(itr2->second.highQuartil == 0){
					if(maxValue == 0){
						yTop = OFFSET;
					}else{
						if(minValue == 0){
							yTop = OFFSET +VIEWHEIGHT;
						}
					}
				}else{
					yTop = ((abs(itr2->second.highQuartil/minValue)) * negHeight) + OFFSET + posHeight;
				}
			}
			double yBot = 0;
			if(itr2->second.lowQuartil > 0){
				yBot = ((1.0 - abs(itr2->second.lowQuartil/maxValue)) * posHeight) + OFFSET;
			}
			else{
				if(itr2->second.lowQuartil == 0){
					if(maxValue == 0){
						yBot = OFFSET;
					}else{
						if(minValue == 0){
							yBot = OFFSET +VIEWHEIGHT;
						}
					}
				}
				else{
					yBot = ((abs(itr2->second.lowQuartil/minValue)) * negHeight) + OFFSET + posHeight;
				}
			}

			double yMid = 0;
			if(itr2->second.median > 0){
				yMid = ((1.0 - abs(itr2->second.median/maxValue)) * posHeight) + OFFSET;
			}
			else{
				if(itr2->second.median == 0){
					if(maxValue == 0){
						yMid = OFFSET;
					}else{
						if(minValue == 0){
							yMid = OFFSET +VIEWHEIGHT;
						}
					}
				}
				else{
					yMid = ((abs(itr2->second.median/minValue)) * negHeight) + OFFSET + posHeight;
				}
			}

			double yAvg = 0;
			if(itr2->second.avg > 0){
				yAvg = ((1.0 - abs(itr2->second.avg/maxValue)) * posHeight) + OFFSET;
			}
			else{
				if(itr2->second.avg == 0){
					if(maxValue == 0){
						yAvg = OFFSET;
					}else{
						if(minValue == 0){
							yAvg = OFFSET +VIEWHEIGHT;
						}
					}
				}
				else{
					yAvg = ((abs(itr2->second.avg/minValue)) * negHeight) + OFFSET + posHeight;
				}
			}



			QRectF newRect(QRectF(QPoint(x - (BLOCKSIZE/2), yTop), QPoint(x + (BLOCKSIZE/2), yBot)));
			//cout << "position: " << position << ", ";
			MedianCoords[position] = yMid;
			averageCoords[position] = yAvg;
			QuartilCoords[position] =  std::pair<double, double> (yTop, yBot);
			if(ui.quartilesCB->isChecked()){
				gScene->addRect(newRect, myPen, brush);
			}



			gScene->addLine( x, OFFSET, x, VIEWHEIGHT + OFFSET, Axispen);
			//X-Axis



			//draw an ellipse
			//The setRenderHint() call enables antialiasing, telling QPainter to use different
			//color intensities on the edges to reduce the visual distortion that normally
			//occurs when the edges of a shape are converted into pixels

			first = false;
			low = !low;
		}
		for(int k = 0; k < allQuartils.at(0).size() - 1; k++){



			double leftX = 0;
			double leftXFlat = 0;
			double leftY1 = 0;
			double leftY2 = 0;
			double rightX = 0;
			double rightXFlat = 0;
			double rightY1 = 0;
			double rightY2 = 0;
			leftXFlat = OFFSET + (offset * k);
			rightXFlat = OFFSET + (offset * (k + 1));
			leftX = leftXFlat + (BLOCKSIZE/2);
			rightX = rightXFlat - (BLOCKSIZE/2);
			leftY1 = QuartilCoords[k].first;
			leftY2 = QuartilCoords[k].second;
			rightY1 = QuartilCoords[k + 1].first;
			rightY2 = QuartilCoords[k+ 1].second;
			//draw a polygon
			if(ui.AverageCB->isChecked()){
				gScene->addLine(leftXFlat, averageCoords[k], rightXFlat, averageCoords[k + 1], myPen);
			}
			if(ui.AbsolutParallelCB->isChecked()){
				for(int z = 0; z < absolute[k].size(); z++){
					gScene->addLine(leftXFlat, absolute[k].at(z), rightXFlat, absolute[k + 1].at(z), myPen2);
				}
			}
			if(ui.medianCB->isChecked()){
				gScene->addLine(leftXFlat, MedianCoords[k], rightXFlat, MedianCoords[k + 1], myPen);
			}
			if(ui.quartilesCB->isChecked()){
				QPolygon polygon;
				polygon << QPoint(leftX, leftY1) << QPoint(rightX, rightY1)
					<< QPoint(rightX, rightY2) << QPoint(leftX, leftY2);
				gScene->addPolygon(polygon,myPen, brush);
			}

		}
		paintLiverIDs = false;
	}
	Axispen = QPen(QBrush(Qt::gray),2.5,Qt::SolidLine);
	gScene->addLine( OFFSET , OFFSET + posHeight, OFFSET + VIEWWIDTH, OFFSET + posHeight, Axispen);
	str = "0";
	//gScene->addText(str,QFont("Times",20))->setPos(QPointF(OFFSET - 25, OFFSET + posHeight - 20));
	//QFont qfont = QFont("Times",20))->setPos(QPointF(OFFSET - 25, OFFSET + posHeight - 20);
	QGraphicsTextItem* texItem = gScene->addText(str,QFont("Times",20));
	texItem->setPos(QPointF(OFFSET - 25, OFFSET + posHeight - 20));
	texItem->setDefaultTextColor(Qt::gray);

}
void clusterHighlighting::paintBoxPlot(){
	ui.AverageCB->hide();
	ui.medianCB->hide();
	ui.quartilesCB->hide();
	ui.AbsolutParallelCB->hide();
	ui.AbsoluteCB->show();
	ui.BoxCB->show();
	ui.MinMixCB->show();
	QString str;
	QPen pen = QPen(QBrush(Qt::black),1.5,Qt::DashLine);
	QPen pen2 = QPen(QBrush(Qt::black),1.5,Qt::SolidLine);
	QPen pen3 = QPen(QBrush(Qt::gray),1.0,Qt::SolidLine);
	QPen pen4 = QPen(QBrush(Qt::red),2.0,Qt::SolidLine);
	QBrush brush(Qt::SolidPattern);
	brush.setColor(QColor ( 0, 255, 0));

	QGraphicsScene* gScene = new QGraphicsScene(this);
	ui.graphicsView_2->setScene(gScene);
	gScene->setSceneRect(0,0,VIEWWIDTH + 2*OFFSET, VIEWHEIGHT + 2*OFFSET);
	QPen Axispen = QPen(QBrush(Qt::gray),1.5,Qt::DashLine);

	//Y-axis
	gScene->addLine( OFFSET, OFFSET, OFFSET, VIEWHEIGHT + OFFSET, Axispen);
	std::map<int, QuartilStruct>::const_iterator itr2;
	double varMax = 0;
	double valueMax = 0;
	double valueMin = 0;
	for(itr2 = intraQuartiles.begin(); itr2 != intraQuartiles.end(); ++itr2){
		if(valueMax < itr2->second.max)
			valueMax = itr2->second.max;
		if(valueMin > itr2->second.min)
			valueMin = itr2->second.min;
	}
	double posHeight = 0;
	double negHeight = 0;
	if(valueMax < 0){
		negHeight = VIEWHEIGHT;
		posHeight = 0;
		//	gScene->addLine( OFFSET, (VIEWHEIGHT + (2*OFFSET)) / 2, OFFSET + VIEWWIDTH, (VIEWHEIGHT + (2*OFFSET)) / 2, Axispen);
	}else if(valueMin > 0){
		negHeight = 0;
		posHeight = VIEWHEIGHT;
	}else if(abs(valueMax) > abs(valueMin)){
		cout << "Max bigger then Min";
		double ratio = 1.0 - abs(valueMin / valueMax);
		posHeight = (VIEWHEIGHT * ratio);
		negHeight = VIEWHEIGHT - posHeight;
	}else{
		if(valueMin == 0){
			cout << "Min bigger then Max";
			posHeight = VIEWHEIGHT / 2;
			negHeight = VIEWHEIGHT - posHeight;
		}else{
			cout << "Min bigger then Max";
			double ratio = abs(valueMax / valueMin);
			posHeight = (ratio * VIEWHEIGHT );
			negHeight = VIEWHEIGHT - posHeight;
		}
	}
	//X-Axis
	gScene->addLine( OFFSET , OFFSET + posHeight, OFFSET + VIEWWIDTH, OFFSET + posHeight, Axispen);

	double offset = (VIEWWIDTH / intraQuartiles.size()) - BLOCKSIZE;
	double x = OFFSET;
	/*double scale =  VIEWHEIGHT / (valueMax+valueMin);
	cout << "\n scale:" << scale << "\n";*/
	int liverID = 0;
	bool low = true;
	for(itr2 = intraQuartiles.begin(); itr2 != intraQuartiles.end(); ++itr2){	
		x = x + offset;
		//double blockHeight = (itr2->second.variance / varMax) * BLOCKHEIGHT;
		//	double blockHeight = itr2->second.variance  * scale;
		double yTop = 0;
		if(itr2->second.highQuartil > 0){
			yTop = ((1.0 - abs(itr2->second.highQuartil/valueMax)) * posHeight) + OFFSET;
		}
		else{
			if(itr2->second.highQuartil == 0){
				if(valueMax == 0){
					yTop = OFFSET;
				}else{
					if(valueMin == 0){
						yTop = OFFSET +VIEWHEIGHT;
					}
				}
			}else{
				yTop = ((abs(itr2->second.highQuartil/valueMin)) * negHeight) + OFFSET + posHeight;
			}
		}
		double yBot = 0;
		if(itr2->second.lowQuartil > 0){
			yBot = ((1.0 - abs(itr2->second.lowQuartil/valueMax)) * posHeight) + OFFSET;
		}
		else{
			if(itr2->second.lowQuartil == 0){
				if(valueMax == 0){
					yBot = OFFSET;
				}else{
					if(valueMin == 0){
						yBot = OFFSET +VIEWHEIGHT;
					}
				}
			}else{
				yBot = ((abs(itr2->second.lowQuartil/valueMin)) * negHeight) + OFFSET + posHeight;
			}
		}

		double yMid = 0;
		if(itr2->second.median > 0){
			yMid = ((1.0 - abs(itr2->second.median/valueMax)) * posHeight) + OFFSET;
		}
		else{
			if(itr2->second.median == 0){
				if(valueMax == 0){
					yMid = OFFSET;
				}else{
					if(valueMin == 0){
						yMid = OFFSET +VIEWHEIGHT;
					}
				}
			}else{
				yMid = ((abs(itr2->second.median/valueMin)) * negHeight) + OFFSET + posHeight;
			}
		}
		if(ui.BoxCB->isChecked()){
			QRectF newRect(QRectF(QPoint(x - (BLOCKSIZE/2), yTop), QPoint(x + (BLOCKSIZE/2), yBot)));
			gScene->addRect(newRect, QPen(), brush);
		}
		//	gScene->addText(QString::number(itr2->second.variance,'f', 2),QFont("Times",7))->setPos(QPointF(x - (BLOCKSIZE/2)- 3,OFFSET + 10 + VIEWHEIGHT + (low * 10)));
		//	gScene->addText(str.setNum(itr2->second.variance),QFont("Times",7))->setPos(QPointF(x - (BLOCKSIZE/2),OFFSET + 10 + VIEWHEIGHT + (low * 10)));
		double botLine = 0;
		double topLine = 0;
		if(itr2->second.min > 0){
			botLine = (1.0 - (itr2->second.min/valueMax)) * posHeight;
		}
		else{
			if(itr2->second.min == 0){
				botLine = OFFSET +VIEWHEIGHT;
			}else{
				botLine = (abs(itr2->second.min/valueMin) * negHeight) + posHeight;
			}
		}

		if(itr2->second.max < 0){
			topLine = (abs(itr2->second.max/valueMin) * negHeight) + posHeight;
		}
		else{
			if(itr2->second.max == 0){
				topLine = OFFSET;
			}else{
				topLine = (1.0 - abs(itr2->second.max/valueMax)) * posHeight;
			}
		}

		//double topLine = (itr2->second.max/valueMax) * VIEWHEIGHT;
		//double botLine = (itr2->second.min/valueMax) * VIEWHEIGHT;
		//
		//

		//y-axis description
		gScene->addText(str.setNum(valueMax),QFont("Times",7))->setPos(QPointF(0,OFFSET - 10));
		gScene->addText(str.setNum(valueMin),QFont("Times",7))->setPos(QPointF(0,OFFSET + 10 + VIEWHEIGHT));

		if( ui.AbsoluteCB->isChecked()){
			for(int i = 0; i < ui.graphicsView->allClusters->at(clusterID).size(); i++){
				double pointValue = avgLiver->SegHistogramData[ui.graphicsView->allClusters->at(clusterID).at(i)].at(liverID);
				double y1 = 0;
				if(pointValue > 0){
					y1 = (1.0 - (pointValue/valueMax)) * posHeight;
				}
				else{
					if(pointValue == 0){
						if(valueMax == 0){
							y1 = OFFSET;
						}else{
							if(valueMin == 0){
								y1 = OFFSET +VIEWHEIGHT;
							}
						}
					}else{
						y1 = (abs(pointValue/valueMin) * negHeight) + posHeight;
					}
				}
				gScene->addLine(x - (BLOCKSIZE/2), y1, x + (BLOCKSIZE/2), y1, pen3);
			}
		}
		if(ui.MinMixCB->isChecked()){
			gScene->addLine(x - (BLOCKSIZE/2), topLine, x + (BLOCKSIZE/2), topLine, pen2);
			gScene->addLine(x - (BLOCKSIZE/2), botLine, x + (BLOCKSIZE/2), botLine, pen2);
			gScene->addLine(x, topLine, x, botLine, pen);
		}
		gScene->addLine(x - (BLOCKSIZE/2), yMid, x + (BLOCKSIZE/2), yMid, pen4);
		liverID++;
		x = x + BLOCKSIZE;
		low = !low;
	}
}

void clusterHighlighting::calculateIntraHistogramVariance(int cluster){
	std::vector<int> clusterPoints = ui.graphicsView->allClusters->at(cluster);

	//map with Liver IDs and a pair(first element is the average Distance, second element is the variance)
	//std::map<int, std::pair<double, double> > variance;


	for(int j = 0; j < avgLiver->SegHistogramData[0].size(); j++){
		double avgDist = 0.0;
		double min = 0.0;
		double max = 0.0;
		for(int i = 0; i < clusterPoints.size(); i++){
			//TODO WRONG SHOULD BE SOMETHING LIKE: double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			//	double value = avgLiver->SegHistogramData[i].at(j);
			avgDist = avgDist + value;
			if(min > value)
				min = value;

			if(max < value)
				max = value;
		}
		double var = 0.0;
		avgDist = avgDist / clusterPoints.size();
		for(int i = 0; i < clusterPoints.size(); i++){
			var = var +	pow((avgLiver->SegHistogramData[clusterPoints.at(i)].at(j) - avgDist), 2);
		}

		var = var / clusterPoints.size();
		varianceStruct temp;
		temp.avg = avgDist;
		temp.variance = var;
		temp.max = max;
		temp.min = min;
		intraVariance[j] = temp;
	}
	paintCandleChart(cluster);
}

void clusterHighlighting::calculateIntraHistogramQuartiles(int cluster){
	clusterID = cluster;
	std::vector<int> clusterPoints = ui.graphicsView->allClusters->at(cluster);

	//map with Liver IDs and a pair(first element is the average Distance, second element is the variance)
	//std::map<int, std::pair<double, double> > variance;


	for(int j = 0; j < avgLiver->SegHistogramData[0].size(); j++){
		std::vector<double> valueVector;
		double min = 0.0;
		double max = 0.0;
		for(int i = 0; i < clusterPoints.size(); i++){

			//TODO WRONG SHOULD BE SOMETHING LIKE: double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			valueVector.push_back(value);
			if(min > value)
				min = value;

			if(max < value)
				max = value;
		}
		sort(valueVector.begin(), valueVector.end());
		double size = valueVector.size();
		int lower = (size * 0.25) + 0.5;
		int median = (size * 0.5) + 0.5;
		int higher = (size * 0.75) + 0.5;
		QuartilStruct temp;
		temp.max = max;
		temp.min = min;
		temp.lowQuartil =  valueVector.at(lower);
		temp.median = valueVector.at(median);;
		temp.highQuartil = valueVector.at(higher);
		temp.clusterID = cluster;
		temp.liverID = PCAselection.at(j);
		cout << "j : " << j;
		intraQuartiles[j] = temp;
	}

	paintBoxPlot();
}

std::map<int, QuartilStruct > clusterHighlighting::calculateIntraHistogramQuartilesOut(int cluster){
	std::map<int, QuartilStruct > intraQuartilesOut;
	clusterID = cluster;
	std::vector<int> clusterPoints = ui.graphicsView->allClusters->at(cluster);

	//map with Liver IDs and a pair(first element is the average Distance, second element is the variance)
	//std::map<int, std::pair<double, double> > variance;


	for(int j = 0; j < avgLiver->SegHistogramData[0].size(); j++){
		std::vector<double> valueVector;
		double min = 0.0;
		double max = 0.0;
		double avg = 0.0;
		for(int i = 0; i < clusterPoints.size(); i++){

			//TODO WRONG SHOULD BE SOMETHING LIKE: double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			double value = avgLiver->SegHistogramData[clusterPoints.at(i)].at(j);
			valueVector.push_back(value);
			avg += value;
			if(min > value)
				min = value;

			if(max < value)
				max = value;
		}
		sort(valueVector.begin(), valueVector.end());
		double size = valueVector.size();
		//TODO: DOnt know if rounding is good
		//int lower = (size * 0.25) + 0.5;
		//int median = (size * 0.5) + 0.5;
		//int higher = (size * 0.75) + 0.5;
		int lower = (size * 0.25) ;
		int median = (size * 0.5);
		int higher = (size * 0.75) ;
		QuartilStruct temp;
		temp.avg = avg / clusterPoints.size();
		temp.max = max;
		temp.min = min;
		temp.lowQuartil =  valueVector.at(lower);
		temp.median = valueVector.at(median);;
		temp.highQuartil = valueVector.at(higher);
		temp.clusterID = cluster;
		temp.liverID = PCAselection.at(j);
		intraQuartilesOut[j] = temp;
	}
	return intraQuartilesOut;
}


std::vector<std::vector<double> > clusterHighlighting::calculateClusterCentroids(std::vector< std::vector<int> >* ClusterVector){
	std::vector<std::vector<double> > centroidVector;
	for(int i = 0; i < ClusterVector->size(); i++){
		std::vector<double> centroid(avgLiver->SegHistogramData[0].size(), 0.0);
		for(int j = 0; j < ClusterVector->at(i).size(); j++){
			for(int k = 0; k < avgLiver->SegHistogramData[0].size(); k++){
				centroid.at(k) = centroid.at(k) + avgLiver->SegHistogramData[ClusterVector->at(i).at(j)].at(k);
			}
		}
		for(int k = 0; k < avgLiver->SegHistogramData[0].size(); k++){
			centroid.at(k) = centroid.at(k) / ClusterVector->at(i).size();
		}
		centroidVector.push_back(centroid);

	}
	return centroidVector;
}
//Max histogram Distance
double clusterHighlighting::calculateInterClusterVarianceMaxDistance(std::vector<std::vector<double> > centroidVector){
	std::vector<double> centroid(avgLiver->SegHistogramData[0].size(),0.0);
	for(int i = 0; i < centroidVector.size(); i++){
		for(int k = 0; k < centroidVector.at(i).size(); k++){
			centroid.at(k) = centroid.at(k) + centroidVector.at(i).at(k);		
		}
	}
	for(int k = 0; k < centroid.size(); k++){
		centroid.at(k) = centroid.at(k)/centroidVector.size();		
	}
	double sum = 0;
	for(int i = 0; i < centroidVector.size(); i++){
		double max = 0;
		for(int k = 0; k < centroidVector.at(i).size(); k++){
			double temp = abs(centroid.at(k) - centroidVector.at(i).at(k));
			if(temp > max)
				max = temp;
		}
		sum += max * max;
	}
	return sum / centroidVector.size();
}

//double clusterHighlighting::calculateInterClusterVarianceEucDistance(std::vector<std::vector<double> > centroidVector){
//	std::vector<double> centroid(avgLiver->SegHistogramData[0].size(),0.0);
//	for(int i = 0; i < centroidVector.size(); i++){
//		for(int k = 0; k < centroidVector.at(i).size(); k++){
//			centroid.at(k) = centroid.at(k) + centroidVector.at(i).at(k);		
//		}
//	}
//	for(int k = 0; k < centroid.size(); k++){
//		centroid.at(k) = centroid.at(k)/centroidVector.size();		
//	}
//	double sum = 0;
//	double avg = 0;
//	for(int i = 0; i < centroidVector.size(); i++){
//		double max = 0;
//		for(int k = 0; k < centroidVector.at(i).size(); k++){
//			avg += abs(centroid.at(k) - centroidVector.at(i).at(k));
//		
//		}
//		avg = avg / centroidVector.at(i).size();
//		sum += avg * avg;
//	}
//	return sum / centroidVector.size();
//}
std::vector<double> clusterHighlighting::calculateDataCentroid( std::vector<std::vector<double> > &centroidVector ) 
{
	std::vector<double> centroid(avgLiver->SegHistogramData[0].size(),0.0);
	for(int i = 0; i < centroidVector.size(); i++){
		for(int k = 0; k < centroidVector.at(i).size(); k++){
			centroid.at(k) = centroid.at(k) + centroidVector.at(i).at(k);		
		}
	}
	for(int k = 0; k < centroid.size(); k++){
		centroid.at(k) = centroid.at(k)/centroidVector.size();		
	}
	return centroid;
}
double clusterHighlighting::calculateInterClusterVarianceEucDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector, std::vector<double> centroid, std::vector<std::set<int> > neighboringClusters){
	//std::vector<double> centroid = calculateDataCentroid(centroidVector);

	double innerSum = 0;
	double outerSum = 0;
	//TODO BAD CHANGE WITH MAX AND MIN VALUE
	double max = -1000000;
	double min = 10000000;
	double singleDistance;
	for(int i = 0; i < centroidVector.size(); i++){
		innerSum = 0.0;
		for(int j = 0; j < centroidVector.size(); j++){
			singleDistance = 0;

			if(i != j){
				if(neighboringClusters.at(i).find(j) != neighboringClusters.at(i).end()){
					for(int k = 0; k < centroidVector.at(i).size(); k++){
						singleDistance += pow(centroidVector.at(j).at(k) - centroidVector.at(i).at(k), 2) ;
					}
					singleDistance = sqrt(singleDistance);
					if(singleDistance > max)
						max = singleDistance;

					if(singleDistance < min)
						min = singleDistance;
				}

				innerSum +=  singleDistance;
			}
		}
		outerSum += 1/innerSum;

		//	avg = avg / centroidVector.at(i).size();

	}
	//cout << "\n MIN:" << min <<"\n";
	double interClusterVariance = (max / min)*outerSum;
	//return sum / centroidVector.size();
	return interClusterVariance;
}
//Max Histogram Distance
double clusterHighlighting::calculateIntraClusterVarianceMaxDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector){
	double globalVariance = 0;
	for(int i = 0; i < ClusterVector->size(); i++){
		double singleVariance = 0;

		for(int j = 0; j < ClusterVector->at(i).size(); j++){
			double max = 0;
			for(int k = 0; k < centroidVector.at(i).size(); k++){
				double temp = abs(centroidVector.at(i).at(k) - avgLiver->SegHistogramData[ClusterVector->at(i).at(j)].at(k));
				if(temp > max)
					max = temp;	
			}
			singleVariance += max * max;
		}
		globalVariance += singleVariance /ClusterVector->at(i).size();
	}
	return globalVariance/ ClusterVector->size();
}


//double clusterHighlighting::calculateIntraClusterVarianceEucDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector){
//	double globalVariance = 0;
//	for(int i = 0; i < ClusterVector->size(); i++){
//		double singleVariance = 0;
//		double avg = 0;
//		for(int j = 0; j < ClusterVector->at(i).size(); j++){
//			
//			for(int k = 0; k < centroidVector.at(i).size(); k++){
//				avg += abs(centroidVector.at(i).at(k) - avgLiver->SegHistogramData[ClusterVector->at(i).at(j)].at(k));
//			}
//			avg = avg / centroidVector.at(i).size();
//			singleVariance += avg * avg;
//		}
//		globalVariance += singleVariance /ClusterVector->at(i).size();
//	}
//	return globalVariance/ ClusterVector->size();
//}

double clusterHighlighting::calculateIntraClusterVarianceEucDistance(std::vector< std::vector<int> >* ClusterVector, std::vector<std::vector<double> > centroidVector, double totalVariance){
	double globalVariance = 0;
	//std::vector<double> varianceVector(centroidVector.at(0).size(),0.0);
	for(int i = 0; i < ClusterVector->size(); i++){
		double singleVariance = 0;

		for(int k = 0; k < centroidVector.at(i).size(); k++){
			double avg = 0;
			for(int j = 0; j < ClusterVector->at(i).size(); j++){


				avg += pow(abs(centroidVector.at(i).at(k) - avgLiver->SegHistogramData[ClusterVector->at(i).at(j)].at(k)),2);
			}
			//avg = avg / centroidVector.at(i).size();
			//avg = sqrt(avg); 
			//	varianceVector.at(k) = (avg/centroidVector.at(i).size());
			singleVariance += pow(avg/ClusterVector->at(i).size(),2) ;
		}
		globalVariance += sqrt(singleVariance) / totalVariance;
	}
	//return globalVariance/ ClusterVector->size();
	return globalVariance / ClusterVector->size();
}

double clusterHighlighting::calculateRSMSSTD(std::vector< std::vector<int> >* ClusterVector,  std::vector<double>* dataCentroid){
	double normalizationfactor = 0;
	//std::vector<double> varianceVector(centroidVector.at(0).size(),0.0);
	double outerSum = 0;
	for(int i = 0; i < dataCentroid->size(); i++){
		double innerSum = 0;


		for(int k = 0; k < ClusterVector->size(); k++){
			normalizationfactor += (ClusterVector->at(k).size() - 1);

			for(int j = 0; j < ClusterVector->at(k).size(); j++){


				innerSum += pow(avgLiver->SegHistogramData[ClusterVector->at(k).at(j)].at(i) - dataCentroid->at(i),2);
			}
			//avg = avg / centroidVector.at(i).size();
			//avg = sqrt(avg); 
			//	varianceVector.at(k) = (avg/centroidVector.at(i).size());

		}
		outerSum += innerSum;

	}
	outerSum = outerSum / normalizationfactor;
	//return globalVariance/ ClusterVector->size();
	return sqrt(outerSum);
}

std::vector<std::set<int> >  clusterHighlighting::getNeighboringClusters(std::vector< std::vector<int> >* allClusters){
	std::map<int, int> clusterMap = Clustering::transformToClusterMap(allClusters);
	std::vector<std::set<int> > clusterNeighbors;
	int counter = 0;
	for(int i = 0; i < allClusters->size(); i++){
		counter += (int) allClusters->at(i).size();
	}
	//cout << "\n Points in Cluster: " << counter << "\t Pointnumber: " << avgLiver->clusteringLiver->getData()->GetNumberOfPoints();
	for(int i = 0; i < allClusters->size(); i++){
		std::set<int> neighborSet;
		for(int j = 0; j <allClusters->at(i).size(); j++){
			std::set<int>* cellPointIds =	new std::set<int>();	
			std::set<int>::const_iterator itr;
			Clustering::GetConnectedVertices(avgLiver->clusteringLiver->getData(), allClusters->at(i).at(j), cellPointIds);
			for(itr = cellPointIds->begin(); itr != cellPointIds->end(); ++itr){

				//cout << "two\n";
				int clusterNeighbor = clusterMap.find(*itr)->second;
				if(i != clusterNeighbor){
					neighborSet.insert(clusterNeighbor);
				}
			}	
		}
		clusterNeighbors.push_back(neighborSet);
	}
	return clusterNeighbors;
}
std::vector<SDStruct >  clusterHighlighting::runClusterSimulation(int maxClusterCount){
	//VCR Vector first double is the threshold, 2nd is the index
	std::vector<SDStruct > SDVector;
	double dataPoints = avgLiver->clusteringLiver->getData()->GetNumberOfPoints();
	//std::vector<double> densityVector;
	//std::vector<double> seperationVector;
	double totalVariance = 0;
	std::vector<double> centroid(avgLiver->SegHistogramData[0].size(),0.0);
	for (int k = 0; k < avgLiver->SegHistogramData[0].size(); k++){
		double avgTemp = 0;
		for (int j = 0; j < avgLiver->SegHistogramData.size(); j++){
			avgTemp += avgLiver->SegHistogramData[j].at(k);
		}
		centroid.at(k) = avgTemp / avgLiver->SegHistogramData.size();
	}
	for (int k = 0; k < avgLiver->SegHistogramData[0].size(); k++){
		double singleVariance = 0;
		for (int j = 0; j < avgLiver->SegHistogramData.size(); j++){
			singleVariance += pow(avgLiver->SegHistogramData[j].at(k) - centroid.at(k),2);
		}
		totalVariance += pow(singleVariance/avgLiver->SegHistogramData.size(),2);
	}
	totalVariance = sqrt(totalVariance);
	int merges = activeCluster->hierarchyLevel + 1;
	cout << "\nnumber Of Merges: " << merges << "\n";
	//for(int i = 99; i > 0; i = i - SAMPLEDISTANCE){
	for(int i = merges; i > 0; --i){
		std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
		double index = i;
		//double threshold = (index/100.0) * activeCluster->hierarchyLevel;
		Clustering::getAllClusterVector(activeCluster, i, allClusters);
		if(allClusters->size() > 1){
			if(allClusters->size() > maxClusterCount)
				break;
			//liverNums.find(i) != liverNums.end()
			std::vector<std::set<int> > neighboringClusters = getNeighboringClusters(allClusters);
			std::vector<std::vector<double> > centroidVector = calculateClusterCentroids(allClusters);





			//double density = calculateIntraClusterVarianceMaxDistance(allClusters, centroidVector);
			//double separation = calculateInterClusterVarianceMaxDistance(centroidVector) ;

			double clusterCount = allClusters->size();

			//double normalization = ((dataPoints - clusterCount) / (clusterCount - 1.0));
			//cout << normalization <<" ,";
			//densityVector.push_back(calculateIntraClusterVarianceEucDistance(allClusters, centroidVector, totalVariance));
			double density =  calculateIntraClusterVarianceEucDistance(allClusters, centroidVector, totalVariance);
			double separation = calculateInterClusterVarianceEucDistance(allClusters, centroidVector, centroid, neighboringClusters);
			//cout << "Density: " << density << " , ";
			//seperationVector.push_back(calculateInterClusterVarianceEucDistance(allClusters, centroidVector, centroid)) ;
			//cout << "Seperation: " << separation << " , ";
			SDStruct SDstruct = {(int)allClusters->size(), i, density, separation, 0};
			SDVector.push_back(SDstruct);


		}
	}
	double alpha = SDVector.back().separation;
	cout << "\nalpha: " << alpha << "\n";
	for(int i = 0; i < SDVector.size(); i++){
		double SD = (alpha * SDVector.at(i).density) + SDVector.at(i).separation;
		cout << "C:" << SDVector.at(i).clusterSize <<"\tDensity: "<< SDVector.at(i).density <<  "\tSeparation:"<< SDVector.at(i).separation <<   "\tSD: " << SD << "\n";
		SDVector.at(i).VCR = SD;
	}

	return SDVector;
}

//std::vector<SDStruct >  clusterHighlighting::runClusterSimulation(int maxClusterCount){
//	//VCR Vector first double is the threshold, 2nd is the index
//	std::vector<SDStruct > SDVector;
//	double dataPoints = avgLiver->clusteringLiver->getData()->GetNumberOfPoints();
//	//std::vector<double> densityVector;
//	//std::vector<double> seperationVector;
//	std::vector<double> centroid(avgLiver->SegHistogramData[0].size(),0.0);
//	
//	for(int i = 99; i > 0; i = i - SAMPLEDISTANCE){
//		std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
//		double index = i;
//		double threshold = (index/100.0) * activeCluster->distance;
//		Clustering::getAllClusterVector(activeCluster, threshold, allClusters);
//		if(allClusters->size() > 1){
//			if(allClusters->size() > maxClusterCount)
//				break;
//			//liverNums.find(i) != liverNums.end()
//		//	std::vector<std::set<int> > neighboringClusters = getNeighboringClusters(allClusters);
//			std::vector<std::vector<double> > centroidVector = calculateClusterCentroids(allClusters);
//			std::vector<double> dataCentroid = calculateDataCentroid(centroidVector);
//
//
//
//
//			//double density = calculateIntraClusterVarianceMaxDistance(allClusters, centroidVector);
//			//double separation = calculateInterClusterVarianceMaxDistance(centroidVector) ;
//
//			double clusterCount = allClusters->size();
//
//			//double normalization = ((dataPoints - clusterCount) / (clusterCount - 1.0));
//			//cout << normalization <<" ,";
//			//densityVector.push_back(calculateIntraClusterVarianceEucDistance(allClusters, centroidVector, totalVariance));
//			double density =  calculateRSMSSTD(allClusters,&dataCentroid);
//			double separation = 0 ;//calculateInterClusterVarianceEucDistance(allClusters, centroidVector, centroid, neighboringClusters);
//			cout << "Density: " << density << "\n";
//			//seperationVector.push_back(calculateInterClusterVarianceEucDistance(allClusters, centroidVector, centroid)) ;
//			//cout << "Seperation: " << separation << " , ";
//			SDStruct SDstruct = {allClusters->size(), i, density, separation, density};
//			SDVector.push_back(SDstruct);
//
//
//		}
//	}
//
//
//	return SDVector;
//}
void clusterHighlighting::setupGraph( Node* cluster, double thresh, vtkDoubleArray* distances, std::vector<int>* choice, vtkLookupTable* lut )
{
	PCAselection = *choice;
	threshold = cluster->hierarchyLevel - thresh;
	activeCluster = cluster;
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
	coordsVec = new std::vector<graphStruct>();
	Clustering::getAllClusterVector(cluster, threshold, allClusters);
	Clustering::getClusterBorders(allClusters, avgLiver->clusteringLiver);
	ui.graphicsView->allClusters = allClusters;
	ui.graphicsView->distances = distances;
	//TODO BAD! Hard Coded
	ui.graphicsView->maxSize = 0;
	for(int i = 0; i < allClusters->size(); i++){
		//double temp = log( (double) allClusters->at(i).size());
		double temp = allClusters->at(i).size();
		if(ui.graphicsView->maxSize < temp)
			ui.graphicsView->maxSize = temp;
		//	maxSize = log((double) allClusters->at(i).size());


	}

	for(int i = 0; i < allClusters->size(); i++){
		double avgDistance = 0;
		for (int j = 0; j < allClusters->at(i).size(); j++)
		{
			avgDistance = avgDistance + distances->GetValue(allClusters->at(i).at(j));
		}

		graphStruct temp;
		temp.x = avgDistance / allClusters->at(i).size();
		//	double a = allClusters->at(i).size();
		double a = allClusters->at(i).size();
		double b = ((a / ui.graphicsView->maxSize) * 9) + 1.0;
		

		temp.y = log10((double) b);
		ColorSource cs;
		//vtkLookupTable* lut = cs.getRainbowLookupTable((int) allClusters->size());
		lut->GetColor(i, temp.rgb);
		//temp.rgb = 0.0;
		//	avgLiver->clusteringLiver->getColorOpacityLookupTable()->GetColor(avgLiver->segAverage->getScalars()->GetValue(allClusters->at(i).at(0)), temp.rgb);
		//	 pair <double,double> temp (avgDistance / allClusters->at(i).size(),0.5);
		coordsVec->push_back(temp);
	}
	ui.graphicsView->paintGraph(coordsVec);
	paintVRCindex();
	//paintVCRChart();
}





