// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "mainWindow.h"
#include "vtkDataSetAttributes.h"
#include <QFuture>
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkViewport.h"
#include "vtkActor2DCollection.h"
#include "MouseInteractorRubberBand.h"

#define PCASCALE 1

struct  pickerStruct
{
	std::vector<RenderTupel*> renderTupelVec;
	std::vector<int>* pickedList;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkTextActor> PCAtextActor;


};

mainWindow::mainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{

	// setup UI
	ui.setupUi(this);
	QObject::connect(ui.actionLoad_Data, SIGNAL(triggered()), this, SLOT(openFileDialog()));
	QObject::connect(ui.actionLoad_Data_with_GettoDistances, SIGNAL(triggered()), this, SLOT(openFileDialog()));
	QObject::connect(ui.actionLoad_Data_with_Total_Correspondence, SIGNAL(triggered()), this, SLOT(openFileDialog()));
	QObject::connect(ui.actionConvert_mhd_to_vtk, SIGNAL(triggered()), this, SLOT(openFileDialogForConversion()));
	QObject::connect(ui.actionSyntheticDataTube, SIGNAL(triggered()), this, SLOT(startSyntheticDataTube()));
	QObject::connect(ui.actionSyntheticDatasphere, SIGNAL(triggered()), this, SLOT(startSyntheticDataSphere()));
	QObject::connect(ui.actionShow_Sidebar, SIGNAL(triggered()), this, SLOT(showSideBar()));
	QObject::connect(ui.RenderButton, SIGNAL(clicked()), this, SLOT(insertTestWindows()));
	QObject::connect(ui.ExpertCB, SIGNAL(pressed()), this, SLOT(changeVisibilityActor1()));
	QObject::connect(ui.IracCB, SIGNAL(pressed()), this, SLOT(changeVisibilityActor2()));
	QObject::connect(ui.ExpertShowColorBut, SIGNAL(pressed()), this, SLOT(showExpertColor()));
	QObject::connect(ui.SegShowColorBut, SIGNAL(pressed()), this, SLOT(showSegColor()));
	QObject::connect(ui.ExpertSlider, SIGNAL(sliderReleased()), this, SLOT(changeOpacityActor1()));
	QObject::connect(ui.IracSlider, SIGNAL(sliderReleased()), this, SLOT(changeOpacityActor2()));
	QObject::connect(ui.SyncRB, SIGNAL(clicked()), this, SLOT(cameraSync()));
	QObject::connect(ui.UnsyncRB, SIGNAL(clicked()), this, SLOT(unSyncCamera()));
	QObject::connect(ui.ResetBut, SIGNAL(clicked()), this, SLOT(resetCamera()));
	QObject::connect(ui.ExpertThresholdSlider, SIGNAL(sliderReleased()), this, SLOT(changeExpertScalars()));
	QObject::connect(ui.SegThresholdSlider, SIGNAL(sliderReleased()), this, SLOT(changeSegScalars()));
	QObject::connect(ui.avgThresholdSlider, SIGNAL(sliderReleased()), this, SLOT(changeAvgScalars()));
	QObject::connect(ui.pcaScalingSlider, SIGNAL(sliderReleased()), this, SLOT(scalePCA()));
	QObject::connect(ui.globalQuartRad, SIGNAL(clicked()), this, SLOT(changeScalarsToGlobalQuartiles()));
	QObject::connect(ui.globalRad, SIGNAL(clicked()), this, SLOT(changeScalarsToGlobal()));
	QObject::connect(ui.localrad, SIGNAL(clicked()), this, SLOT(changeScalarsToLocal()));
	QObject::connect(ui.blackBG, SIGNAL(clicked()), this, SLOT(changeBackgroundColor()));
	QObject::connect(ui.greyBG, SIGNAL(clicked()), this, SLOT(changeBackgroundColor()));
	QObject::connect(ui.whiteBG, SIGNAL(clicked()), this, SLOT(changeBackgroundColor()));
	QObject::connect(ui.actionGrid, SIGNAL(triggered()), this, SLOT(showGrid()));
	QObject::connect(ui.actionPCA, SIGNAL(triggered()), this, SLOT(showPCA()));
	QObject::connect(ui.actionAverage_Liver, SIGNAL(triggered()), this, SLOT(showAverage()));
	QObject::connect(this, SIGNAL(prepareForRenderingSignal(int)), this, SLOT(prepareForRendering(int)));
	QObject::connect(ui.clusteringThresholdSlider, SIGNAL(sliderReleased()), this, SLOT(showClusterColor()));
	QObject::connect(ui.clusterHighlightingBt, SIGNAL(clicked()), this, SLOT(showClusterGraph()));
	QObject::connect(ui.edal_bt, SIGNAL(pressed()), this, SLOT(changeAvgLiverToHisEukAverageClustering()));
	QObject::connect(ui.edml_Bt, SIGNAL(pressed()), this, SLOT(changeAvgLiverToHisEukMaxClustering()));
	QObject::connect(ui.mdal_bt, SIGNAL(pressed()), this, SLOT(changeAvgLiverToHisMaxAvgClustering()));
	QObject::connect(ui.mdml_bt, SIGNAL(pressed()), this, SLOT(changeAvgLiverToHisMaxMaxClustering()));
	QObject::connect(this, SIGNAL(enablePCAButtonSignal()), this, SLOT(enablePCAButton()));
	QObject::connect(this, SIGNAL(showProgressBarsSignal(int)), this, SLOT(showProgressBars(int)));
	QObject::connect(this, SIGNAL(hideProgressBarsSignal(int)), this, SLOT(hideProgressBars(int)));
	QObject::connect(this, SIGNAL(enableClusterButtonsSignal(int)), this, SLOT(enableClusterButtons(int)));
	QObject::connect(this, SIGNAL(enableClusterViewSignal()), this, SLOT(enableClusterViewBt()));
	QObject::connect(ui.selClusterHighlightingBt, SIGNAL(clicked()), this, SLOT(newReducedClustering()));
	QObject::connect(ui.noSelectionBt, SIGNAL(clicked()), this, SLOT(resetLiverSelection()));
	QObject::connect(ui.AllSampleCB, SIGNAL(clicked()), this, SLOT(renderAverageGrid()));
	QObject::connect(ui.PCAchoiceCB, SIGNAL(clicked()), this, SLOT(renderAverageGrid()));
	QObject::connect(ui.ClusteringCB, SIGNAL(clicked()), this, SLOT(renderAverageGrid()));
	QObject::connect(ui.allSampleBorderCB, SIGNAL(clicked()), this, SLOT(showAverage()));
	QObject::connect(ui.PCAchoiceBorderCB, SIGNAL(clicked()), this, SLOT(showAverage()));
	QObject::connect(ui.clusteringBorderCB, SIGNAL(clicked()), this, SLOT(showAverage()));
	QObject::connect(ui.showSelectionButton, SIGNAL(clicked()), this, SLOT(showChoice()));
	QObject::connect(ui.MaxClusterNumberSB, SIGNAL(valueChanged(int)), this, SLOT(changeClusteringThresholdScale(int)));
	QObject::connect(ui.expSelectionBt, SIGNAL(clicked()), this, SLOT(exportSelection()));
	QObject::connect(ui.showAllBtn, SIGNAL(clicked()), this, SLOT(checkAll()));
	QObject::connect(ui.hideAllBtn, SIGNAL(clicked()), this, SLOT(uncheckAll()));
	QObject::connect(ui.pcaLabelsCB, SIGNAL(stateChanged(int)), this, SLOT(changePCAlabelState(int)));

	viewmode = 1;
	//ui.InteractionGB->hide();
	ui.actionPCA->setEnabled(false);
	ui.mdal_bt->setEnabled(false);
	ui.mdml_bt->setEnabled(false);
	ui.edml_Bt->setEnabled(false);
	ui.edal_bt->setEnabled(false);
	ui.clusterHighlightingBt->setEnabled(false);
	ui.actionAverage_Liver->setEnabled(false);
	ui.mdal_PB->hide();
	ui.mdml_PB->hide();
	ui.edal_PB->hide();
	ui.edml_PB->hide();
	clusterChoice = new std::set<int>();
	sideBarActivated = true;
	toggleSideBarActive();
	QIntValidator* val = new QIntValidator(this);
	ui.ColumnsLEdit->setValidator(val);
	ui.TestFramesLEdit->setValidator(val);
	//ui.VisualizationGB->hide();
	//hide color legend
	ui.colorLegendFrame->hide();
	ui.progressBarBig->hide();
	ui.progressBarSmall->hide();
	ui.pcaGB->hide();
	ui.ClusteringThresholdFrame->hide();
	fastTestCluster = NULL;
	PCAselectedClusters = new std::vector<int>();


}

mainWindow::~mainWindow(){

}

void mainWindow::createGrid(int max ){
	// clear Data
	ui.treeWidget->clear();
	viewmode = 1;
	delete ui.RenderWidget->layout();
	delete ui.RenderWidget;
	QSplitter *qsplit = new QSplitter(Qt::Vertical, ui.mainWidget);
	ui.RenderWidget = qsplit;
	//ui.RenderWidget = new QWidget(ui.mainWidget);
	ui.RenderWidget->setObjectName(QString::fromUtf8("RenderWidget"));
	QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(ui.RenderWidget->sizePolicy().hasHeightForWidth());
	ui.RenderWidget->setSizePolicy(sizePolicy1);
	//	ui.RenderWidget->setMinimumSize(QSize(2000,2000));
	//	ui.RenderWidget->setMaximumSize(QSize(5000, 5000));
	ui.verticalLayout_12->addWidget(ui.RenderWidget);
	ui.colorLegendFrame->show();
	ui.smallBarWidget->hide();
	ui.progressBarBig->hide();
	ui.RenderWidget->update();
	QWidget* frame = new QWidget();
	QGridLayout* layout = new QGridLayout();
	layout->setMargin(1);
	int x = 0;
	int y = 0;
	int lines = (int) widgets.size()/ max;
	if(lines * max < widgets.size())
		lines++;
	
	std::vector<QSplitter*> splitvec(lines);
	
	QTreeWidgetItem * topLevel = new QTreeWidgetItem();
	topLevel->setText(0,"Toggle Visibilty");
	for(int i = 0; i < lines; i++){
		splitvec.at(i) = new QSplitter(Qt::Horizontal);
	}
	//QSplitter* s1 = new QSplitter(Qt::Horizontal);
	for(int i = 0; i < widgets.size() ; i++){

		if(x == max){
			qsplit->addWidget(splitvec.at(y));
			//	s1 = new QSplitter(Qt::Horizontal);
			x = 0;
			y++;
		}
		splitvec.at(y)->addWidget(widgets.at(i));
		x++;

		//Widget List
	QTreeWidgetItem* newTreeItem = new QTreeWidgetItem();
	QString str = QString("Sample ");
	str.append(QString::number(i));
	newTreeItem->setText(0,str);
	newTreeItem->setBackgroundColor(0,QColor(128,229,136));
	newTreeItem->setCheckState(0, Qt::Checked);
	topLevel->addChild(newTreeItem);
	};

	//Check if somethings already in the List
	if(ui.treeWidget->topLevelItemCount() == 0){
		ui.treeWidget->setHeaderHidden(true);
		ui.treeWidget->addTopLevelItem(topLevel);
		QObject::connect(ui.treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(toggleGridWidget(QTreeWidgetItem*, int)));
		topLevel->setExpanded(true);
	}
	if(x != 0)	
		qsplit->addWidget(splitvec.at(y));

	ui.RenderWidget->update();
	activateSideBar();
	changeBackgroundColor();
}


void mainWindow::checkAll(){
	QTreeWidgetItem* treeItem = ui.treeWidget->topLevelItem(0);
	for(int i = 0; i < treeItem->childCount(); i++){
		QTreeWidgetItem* treeSubItem = treeItem->child(i);
		treeSubItem->setCheckState(0, Qt::Checked);
	}
}

void mainWindow::uncheckAll(){
	QTreeWidgetItem* treeItem = ui.treeWidget->topLevelItem(0);
	for(int i = 0; i < treeItem->childCount(); i++){
		QTreeWidgetItem* treeSubItem = treeItem->child(i);
		treeSubItem->setCheckState(0, Qt::Unchecked);
	}
}

void mainWindow::toggleGridWidget(QTreeWidgetItem* item, int column){
	if(item->parent() != NULL){
	int index = item->parent()->indexOfChild(item);
	if(item->checkState(0) == Qt::Checked){
		item->setBackgroundColor(0,QColor(128,229,136));
		widgets.at(index)->show();
	}else{
		item->setBackgroundColor(0,QColor(223,52,52));
		widgets.at(index)->hide();
	}
	}
}

void mainWindow::setWidgets(std::vector<QVTKWidget*> vec){
	widgets = vec;
}


void mainWindow::createAverageGrid(){
	// clear Data
	viewmode = 3;
	delete ui.RenderWidget->layout();
	delete ui.RenderWidget;
	ui.RenderWidget = new QWidget(ui.mainWidget);
	ui.RenderWidget->setObjectName(QString::fromUtf8("RenderWidget"));
	QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(ui.RenderWidget->sizePolicy().hasHeightForWidth());
	ui.RenderWidget->setSizePolicy(sizePolicy1);
	//	ui.RenderWidget->setMinimumSize(QSize(2000,2000));
	//	ui.RenderWidget->setMaximumSize(QSize(5000, 5000));
	ui.verticalLayout_12->addWidget(ui.RenderWidget);
	ui.colorLegendFrame->show();
	ui.smallBarWidget->hide();
	ui.progressBarBig->hide();
	ui.RenderWidget->update();
	QWidget* frame = new QWidget();
	QGridLayout* layout = new QGridLayout();
	layout->setMargin(1);
	int x = 0;
	int y = 0;

	if(ui.AllSampleCB->isChecked()){
		layout->addWidget(widgets.at(0),1,1);
	}
	if(ui.PCAchoiceCB->isChecked()){
		layout->addWidget(widgets.at(1),1,2);
	}
	if(ui.ClusteringCB->isChecked()){
		layout->addWidget(widgets.at(2),1,3);
	}

	ui.treeWidget->clear();
	QTreeWidgetItem * topLevel = new QTreeWidgetItem();
	topLevel->setText(0,"Toggle Visibilty");

		//Widget List
		QTreeWidgetItem* newTreeItem = new QTreeWidgetItem();
		QString str = QString("All Average");
		newTreeItem->setText(0,str);
		newTreeItem->setBackgroundColor(0,QColor(128,229,136));
		newTreeItem->setCheckState(0, Qt::Checked);
		topLevel->addChild(newTreeItem);

		QTreeWidgetItem* newTreeItem2 = new QTreeWidgetItem();
		QString str2 = QString("PCA Choice Average");
		newTreeItem2->setText(0,str2);
		newTreeItem2->setBackgroundColor(0,QColor(128,229,136));
		newTreeItem2->setCheckState(0, Qt::Checked);
		topLevel->addChild(newTreeItem2);

		QTreeWidgetItem* newTreeItem3 = new QTreeWidgetItem();
		QString str3 = QString("Clustering");
		newTreeItem3->setText(0,str3);
		newTreeItem3->setBackgroundColor(0,QColor(128,229,136));
		newTreeItem3->setCheckState(0, Qt::Checked);
		topLevel->addChild(newTreeItem3);
	

	//Check if somethings already in the List
	if(ui.treeWidget->topLevelItemCount() == 0){
		ui.treeWidget->setHeaderHidden(true);
		ui.treeWidget->addTopLevelItem(topLevel);
		QObject::connect(ui.treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(toggleGridWidget(QTreeWidgetItem*, int)));
		topLevel->setExpanded(true);
	}
	//for(int i = 0; i < widgets.size() ; i++){
	//	if(x == max){
	//		x = 0;
	//		y++;
	//	}
	//	layout->addWidget(widgets.at(i),y,x);
	//	x++;
	//}

	frame->setLayout(layout);
	ui.RenderWidget->setLayout(layout);
	ui.RenderWidget->update();
	activateSideBar();
	changeBackgroundColor();
}



void paintSync(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData){

	std::vector<QVTKWidget*>* widgets = static_cast<std::vector<QVTKWidget*>*> (clientData);
	for(int i = 0; i < widgets->size() ; i++)
		widgets->at(i)->repaint();
}


void mainWindow::cameraSync(){
	if(!widgets.empty()){
		vtkSmartPointer<vtkCallbackCommand> sync =  vtkSmartPointer<vtkCallbackCommand>::New();
		sync->SetCallback(paintSync);
		sync->SetClientData(&widgets);
		vtkCamera *cam = vtkCamera::New();

		cam->AddObserver(vtkCommand::ModifiedEvent, sync);
		for(int i = 0; i < widgets.size(); i++){
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(cam);
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
			widgets.at(i)->update();

		}
	}
}

void mainWindow::unSyncCamera(){
	if(!widgets.empty()){

		for(int i = 0; i < widgets.size(); i++){
			vtkCamera *cam = vtkCamera::New();
			cam->DeepCopy(widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera());	
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(cam);
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
			widgets.at(i)->update();

		}
	}
}

void mainWindow::resetCamera(){
	if(!widgets.empty()){
		for(int i = 0; i < widgets.size(); i++){
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->ResetCamera();
			widgets.at(i)->GetRenderWindow()->Modified();
			widgets.at(i)->update();

		}
		repaintAll();
	}
}


void mainWindow::openFileDialog(){

	QAction* senderButton = dynamic_cast<QAction*>(sender());
	bool withDistances = false;
	bool totalDistance = false;
	if(senderButton == ui.actionLoad_Data_with_GettoDistances)
		withDistances = true;

	if(senderButton == ui.actionLoad_Data_with_Total_Correspondence)
		withDistances = totalDistance;

	//      textLabel->setText(text);
	//	clusterHighlighting* highlightning = new clusterHighlighting(this,0);
	//	highlightning->show();
	//	showHeatmap();


	ui.progressBarBig->setValue(0);
	QStringList mhdFiles;
	QStringList vtkFiles;
	bool first = true;
	QFileDialog dialog(this);
	dialog.setWindowTitle("Select Expert Files");
	//dialog.setNameFilter("MHD-Files (*.mhd)");
	dialog.setNameFilter("vtk-Files (*.vtk);;off-Files (*.off);; mhd-Files (*.mhd)");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	while(QFileDialog::Accepted != dialog.exec()){
		return;
	}
	mhdFiles = dialog.selectedFiles();
	do{
		if(!first){
			QMessageBox msgBox;
			msgBox.setText("Selected Number of vtk and mhd Files don't match!");
			msgBox.exec();
		}
		QFileDialog dialog2(this);
		dialog2.setWindowTitle("Select IRCAD Files");
		dialog2.setNameFilter("vtk-Files (*.vtk);;off-Files (*.off)");
		dialog2.setFileMode(QFileDialog::ExistingFiles);
		while(QFileDialog::Accepted != dialog2.exec()){
			return;
		}
		vtkFiles = dialog2.selectedFiles();
		first = false;
	} while(vtkFiles.size() != mhdFiles.size());
	bool ok;
	QString text = QInputDialog::getText(this, tr("Columns"),
		tr("Columns:"), QLineEdit::Normal,"4", &ok);
	if (ok && !text.isEmpty()){
		ui.colorLegendFrame->hide();
		ui.progressBarBig->show();
		columns = atoi(text.toStdString().c_str());
		QtConcurrent::run(this, &mainWindow::prepareLivers, mhdFiles, vtkFiles, totalDistance,withDistances);

	}

}

void mainWindow::openFileDialogForConversion(){




	ui.progressBarBig->setValue(0);
	QStringList mhdFiles;
	QFileDialog dialog(this);
	dialog.setWindowTitle("Select Expert Files");
	//dialog.setNameFilter("MHD-Files (*.mhd)");
	dialog.setNameFilter("mhd-Files (*.mhd)");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.exec();
	//while(QFileDialog::Accepted != dialog.exec()){
	//	return;
	//}
	mhdFiles = dialog.selectedFiles();
	QtInterface* inter = new QtInterface();
	QtConcurrent::run(inter,&QtInterface::convertMHDs, mhdFiles,false,false);

}

void mainWindow::startSyntheticDataTube(){
	QtInterface* inter = new QtInterface();
	//QFutureWatcher<std::vector<RenderTupel*> > future = new QFutureWatcher<std::vector<RenderTupel*> >();
	QObject::connect(inter, SIGNAL(progressUpdate(int)), this, SLOT(setBigBarProgress(int)));
	QFutureSynchronizer<std::vector<RenderTupel*> >* synchronizer = new QFutureSynchronizer<std::vector<RenderTupel*> >();

	synchronizer->addFuture(QtConcurrent::run(inter, &QtInterface::loadSingleSyntheticDataTube));	
	synchronizer->waitForFinished();



	setRenderTupels(synchronizer->futures().back().result());
	setupScalars();

	for (int i = 0; i < renderTupels.size(); i++)
	{
		PCAselectedClusters->push_back(i);
	}
	fsynchronizer = new QFutureSynchronizer<void>();
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateAverage));
	fsynchronizer->waitForFinished();

	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculatePCA));

	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucAvgClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucMaxClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxMaxClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxAverageClustering));


	//	synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateMinimumClustering));

	//	synchronizer2->waitForFinished();
	columns = 4;
	emit prepareForRenderingSignal(columns);


}

void mainWindow::startSyntheticDataSphere(){
	QtInterface* inter = new QtInterface();
	//QFutureWatcher<std::vector<RenderTupel*> > future = new QFutureWatcher<std::vector<RenderTupel*> >();
	QObject::connect(inter, SIGNAL(progressUpdate(int)), this, SLOT(setBigBarProgress(int)));
	QFutureSynchronizer<std::vector<RenderTupel*> >* synchronizer = new QFutureSynchronizer<std::vector<RenderTupel*> >();
	synchronizer->addFuture(QtConcurrent::run(inter, &QtInterface::loadSingleSyntheticDataSphere));	
	synchronizer->waitForFinished();



	setRenderTupels(synchronizer->futures().back().result());
	setupScalars();

	for (int i = 0; i < renderTupels.size(); i++)
	{
		PCAselectedClusters->push_back(i);
	}
	fsynchronizer = new QFutureSynchronizer<void>();
	futureList = new QList<QFuture<void > >();

	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateAverage));
	fsynchronizer->waitForFinished();

	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculatePCA));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucAvgClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucMaxClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxMaxClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxAverageClustering));
	columns = 3;
	emit prepareForRenderingSignal(columns);


}


void mainWindow::prepareLivers(QStringList listMhd, QStringList listVtk, bool decimate, bool withDistances){
	QtInterface* inter = new QtInterface();
	//QFutureWatcher<std::vector<RenderTupel*> > future = new QFutureWatcher<std::vector<RenderTupel*> >();
	QObject::connect(inter, SIGNAL(progressUpdate(int)), this, SLOT(setBigBarProgress(int)));
	QFutureSynchronizer<std::vector<RenderTupel*> >* synchronizer = new QFutureSynchronizer<std::vector<RenderTupel*> >();
	synchronizer->addFuture(QtConcurrent::run(inter, &QtInterface::loadLivers, listMhd, listVtk, decimate, withDistances));	
	synchronizer->waitForFinished();



	setRenderTupels(synchronizer->futures().back().result());
	setupScalars();

	for (int i = 0; i < renderTupels.size(); i++)
	{
		PCAselectedClusters->push_back(i);
	}

	QFutureSynchronizer<void>* synchronizer2 = new QFutureSynchronizer<void>();
	//TODO Back In
	synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateAverage));
	synchronizer2->waitForFinished();
	synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculatePCA));

	synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucAvgClustering));
	synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucMaxClustering));
	//synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxMaxClustering));
	//synchronizer2->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxAverageClustering));
	emit prepareForRenderingSignal(columns);
}

void mainWindow::prepareForRendering(int columns){


	ui.actionGrid->setChecked(true);
	ui.menuView_Mode->setEnabled(true);
	showGrid();
	//showPCA();
	ui.SegShowColorBut->setEnabled(true);

	ui.colorLegendFrame->show();
}

void mainWindow::calculateAverage(){
	std::set<int> temp;
	averageLiver = new AverageLiver(renderTupels, temp, false);
	averageBaseLiver = new AverageLiver(renderTupels, temp, false);
	//cout <<"Average Liver Ready! \n";
}

void mainWindow::calculatePCA(){	
	PCA::calculatePCA(&renderTupels);
	emit enablePCAButtonSignal();
	cout << "\n PCA READY! \n" ;

}

void PickCallbackFunction(vtkObject* caller,
						  long unsigned int vtkNotUsed(eventId),
						  void* clientData,
						  void* vtkNotUsed(callData))
{
	std::cout << "Pick." << std::endl;
	vtkAreaPicker* areaPicker = static_cast<vtkAreaPicker*>(caller);

	vtkProp3DCollection* props = areaPicker->GetProp3Ds();
	props->InitTraversal();
	pickerStruct* pickdata = static_cast<pickerStruct*>(clientData);
	//std::pair< std::vector<RenderTupel*> , std::vector<int>* >* workingPair = static_cast<std::pair< std::vector<RenderTupel*> , std::vector<int>* >* >(clientData);
	std::vector<RenderTupel*> renderTupels = pickdata->renderTupelVec;
	vtkSmartPointer<vtkTextActor> PCAtextActor = pickdata->PCAtextActor;
	//  std::set<int>* selectedCluster = workingPair->second;
	pickdata->pickedList->clear();
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor()->GetProperty()->EdgeVisibilityOff();
	}
	std::string text= "Picked Sample: ";
	for(vtkIdType i = 0; i < props->GetNumberOfItems(); i++)
	{

		vtkProp3D* prop = props->GetNextProp3D();

		// vtkProp* prop = props->GetNextProp();
		vtkActor* actor = static_cast<vtkActor*>(prop);
		actor->GetProperty()->EdgeVisibilityOn();
		vtkPropCollection * actorProp = vtkPropCollection::New();
		std::cout << "Picked prop: " << prop << std::endl;
		//	prop->VisibilityOff();
		//	prop->GetActors(actorProp);
		//	vtkProp* actor = actorProp->GetNextProp();

		for(int i = 0; i < renderTupels.size(); i++){


			if(prop == renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor() ){
				std::cout << "Picked Sample: " << i << std::endl;
				pickdata->pickedList->push_back(i);
				text.append(QString::number(i).toStdString());
				text.append(", ");
			}
		}
		std::cout << "No Actor Found!" <<std::endl;
	}

	PCAtextActor = vtkSmartPointer<vtkTextActor>::New();
	//  PCAtextActor = static_cast<vtkTextActor* >(pickdata->renderer->GetActors2D()->GetLastActor2D());
	// pickdata->renderer->GetActors2D()->GetLastActor2D()->Delete();
	pickdata->renderer->RemoveActor2D(pickdata->renderer->GetActors2D()->GetLastActor2D());
	PCAtextActor->GetTextProperty()->SetFontSize ( 24 );
	PCAtextActor->SetPosition (200, 0 );
	//  pickdata->renderer->GetActors2D()->GetLastActor2D()->Delete();

	pickdata->renderer->AddActor2D ( PCAtextActor );
	PCAtextActor->SetInput ( text.c_str() );
	PCAtextActor->GetTextProperty()->SetColor ( 1.0,1.0,1.0 );
}


void mainWindow::changeViewMode(){
	for(int i = 0; i < widgets.size(); i++){
		vtkSmartPointer<QVTKInteractor> interactor =
			vtkSmartPointer<QVTKInteractor>::New();

		widgets.at(i)->GetRenderWindow()->SetInteractor(interactor);
		widgets.at(i)->GetRenderWindow()->Render();
	}
	//	repaintAll();
}

void mainWindow::changeSelectMode(){
	for(int i = 0; i < widgets.size(); i++){
		vtkSmartPointer<QVTKInteractor> interactor =
			vtkSmartPointer<QVTKInteractor>::New();
		interactor->SetDesiredUpdateRate(30);
		//qRegisterMetaType<MouseInteractorStyle2>();
		MouseInteractorStyle2* style =
			MouseInteractorStyle2::New();
		connect(style, SIGNAL(sendPointID(int, bool )), this, SLOT(highightClusters(int, bool)));
		interactor->SetInteractorStyle(style);
		style->SetDefaultRenderer(widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		widgets.at(i)->GetRenderWindow()->SetInteractor(interactor);
		widgets.at(i)->GetRenderWindow()->Render();
	}
	//	repaintAll();
}

void mainWindow::changeRectangularMode(){
	for(int i = 0; i < widgets.size(); i++){
		vtkSmartPointer<QVTKInteractor> interactor =
			vtkSmartPointer<QVTKInteractor>::New();
		vtkSmartPointer<vtkAreaPicker> areaPicker1 =
			vtkSmartPointer<vtkAreaPicker>::New();
		vtkSmartPointer<HighlightInteractor> style =
			vtkSmartPointer<HighlightInteractor>::New();
		//	style->SelectedMapper = actors.at(i).first->GetMapper();
		//	style->SelectedActor = actors.at(i).first;
		style->SetPoints(dataInputs.at(i).first);
		interactor->SetInteractorStyle(style);
		interactor->SetPicker(areaPicker1);

		//vtkSmartPointer<vtkAreaPicker> areaPicker = 
		//  vtkSmartPointer<vtkAreaPicker>::New();
		//interactor->SetPicker(areaPicker);
		//interactor->SetDesiredUpdateRate(30);
		//
		//// For vtkInteractorStyleRubberBandPick - use 'r' and left-mouse to draw a selection box used to pick
		//vtkSmartPointer<vtkInteractorStyleRubberBandPick> style =
		//  vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();

		//vtkSmartPointer<vtkCallbackCommand> pickCallback = 
		//  vtkSmartPointer<vtkCallbackCommand>::New();
		//pickCallback->SetCallback ( PickCallbackFunction );
		//areaPicker->AddObserver ( vtkCommand::EndPickEvent, pickCallback );
		//interactor->SetInteractorStyle(style);


		widgets.at(i)->GetRenderWindow()->SetInteractor(interactor);
		widgets.at(i)->GetRenderWindow()->Render();
	}
	//	repaintAll();
}

void mainWindow::showSideBar(){
	if(!ui.actionShow_Sidebar->isChecked()){
		ui.SideBarWidget->setVisible(false);
	}
	else{
		ui.SideBarWidget->setVisible(true);
	}
}

void mainWindow::repaintAll(){
	for(int i = 0; i < widgets.size() ; i++)
		widgets.at(i)->repaint();
}

void mainWindow::isSideBarVisible(){
	if(ui.SideBarWidget->isVisible()){
		ui.actionShow_Sidebar->setChecked(true);
	}
	else{
		ui.actionShow_Sidebar->setChecked(false);
	}
}

void mainWindow::insertTestWindows(){
	//PCA a;
	//a.calculatePCA(renderTupels);
	//// These would be all of your "y" values.
	//const char m1Name[] = "M1";
	//vtkSmartPointer<vtkDoubleArray> dataset2Arr =vtkSmartPointer<vtkDoubleArray>::New();
	//dataset2Arr->SetNumberOfComponents(1);
	//dataset2Arr->SetName( m1Name );
	//dataset2Arr->InsertNextValue(4.0);
	//dataset2Arr->InsertNextValue(4.2);
	//dataset2Arr->InsertNextValue(3.9);
	//dataset2Arr->InsertNextValue(4.3);
	//dataset2Arr->InsertNextValue(4.1);


	//// These would be all of your "z" values.
	//const char m2Name[] = "M2";
	//vtkSmartPointer<vtkDoubleArray> dataset3Arr =vtkSmartPointer<vtkDoubleArray>::New();
	//dataset3Arr->SetNumberOfComponents(1);
	//dataset3Arr->SetName( m2Name );
	//dataset3Arr->InsertNextValue(2.0);
	//dataset3Arr->InsertNextValue(2.1);
	//dataset3Arr->InsertNextValue(2.0);
	//dataset3Arr->InsertNextValue(2.1);
	//dataset3Arr->InsertNextValue(2.2);

	//const char m3Name[] = "M3";
	//vtkSmartPointer<vtkDoubleArray> dataset1Arr =vtkSmartPointer<vtkDoubleArray>::New();
	//dataset1Arr->SetNumberOfComponents(1);
	//dataset1Arr->SetName( m3Name );
	//dataset1Arr->InsertNextValue(0.60);
	//dataset1Arr->InsertNextValue(0.59);
	//dataset1Arr->InsertNextValue(0.58);
	//dataset1Arr->InsertNextValue(0.62);
	//dataset1Arr->InsertNextValue(0.63);


	//vtkSmartPointer<vtkTable> datasetTable = vtkSmartPointer<vtkTable>::New();
	//datasetTable->AddColumn(dataset2Arr);
	//datasetTable->AddColumn(dataset3Arr);
	//datasetTable->AddColumn(dataset1Arr);


	//vtkSmartPointer<vtkTable> dataset2Table =
	//	vtkSmartPointer<vtkTable>::New();
	//vtkSmartPointer<vtkPCAStatistics> pcaStat = vtkSmartPointer<vtkPCAStatistics>::New();
	////pcaStat->GetOutput();
	//vtkSmartPointer<vtkMultiCorrelativeStatistics> multCorr = vtkSmartPointer<vtkMultiCorrelativeStatistics>::New();
	//	//multCorr->SetInput( vtkStatisticsAlgorithm::INPUT_DATA, datasetTable);
	//	multCorr->SetInput(vtkStatisticsAlgorithm::INPUT_DATA,datasetTable);
	//	multCorr->SetColumnStatus( m1Name, 1 );
	//	
	////	multCorr->SetColumnStatus()
	//	multCorr->SetColumnStatus( m2Name, 1 );
	//	multCorr->SetColumnStatus( m3Name, 1 );
	//	multCorr->RequestSelectedColumns();
	//	multCorr->Update();
	//	vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast( multCorr->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
	//	dataset2Table = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 1) );

	//cout <<  dataset2Table->GetValue(0,2).ToFloat() << "\t" << dataset2Table->GetValue(0,3).ToFloat() << "\t" << dataset2Table->GetValue(0,4).ToFloat();


	setWidgets(testWindows(atoi(ui.TestFramesLEdit->text().toStdString().c_str())));
	createGrid(atoi(ui.ColumnsLEdit->text().toStdString().c_str()));
	cameraSync();
}

std::vector<QVTKWidget*> mainWindow::testWindows(int a){
	// Create a sphere
	std::vector<QVTKWidget* > widgets;
	actors.clear();


	for(int i = 0; i < a; i++){
		vtkSmartPointer<vtkCylinderSource> cylinderSource =
			vtkSmartPointer<vtkCylinderSource>::New();
		cylinderSource->SetCenter(0.0, 0.0, 0.0);
		cylinderSource->SetRadius(5.0);
		cylinderSource->SetHeight(7.0);
		cylinderSource->SetResolution(100);

		// Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(cylinderSource->GetOutputPort());
		mapper->Update();
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();

		actor->SetMapper(mapper);
		vtkSmartPointer<vtkCubeSource> cubeSource =
			vtkSmartPointer<vtkCubeSource>::New();


		// Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper2 =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper2->SetInputConnection(cubeSource->GetOutputPort());
		mapper2->Update();
		vtkSmartPointer<vtkActor> actor2 =
			vtkSmartPointer<vtkActor>::New();
		actor2->SetMapper(mapper2);


		//Create a renderer, render window, and interactor
		vtkSmartPointer<vtkRenderer> renderer =
			vtkSmartPointer<vtkRenderer>::New();

		vtkSmartPointer<vtkRenderWindow> renderWindow =
			vtkSmartPointer<vtkRenderWindow>::New();
		renderWindow->AddRenderer(renderer);

		//	renderWindow->StereoCapableWindowOn();
		//	renderWindow->SetStereoTypeToCrystalEyes();
		//	renderWindow->FullScreenOn();
		//renderWindow->Start();

		actor->GetProperty()->SetOpacity(((double)ui.ExpertSlider->value())/100.00f);
		actor2->GetProperty()->SetOpacity(((double)ui.IracSlider->value())/100.00f);
		// Add the actor to the scene
		renderer->AddActor(actor);
		renderer->AddActor(actor2);
		actors.push_back(std::pair<vtkSmartPointer<vtkActor>,vtkSmartPointer<vtkActor> >(actor,actor2));
		dataInputs.push_back(std::pair<vtkSmartPointer<vtkPolyData>,vtkSmartPointer<vtkPolyData> >(cylinderSource->GetOutput(),cubeSource->GetOutput()));
		cylinderSource->Update();
		cubeSource->Update();
		renderer->ResetCamera();
		renderer->SetBackground(.1, .3,.2); // Background color dark green
		QVTKWidget *widget1 = new QVTKWidget();
		//		vtkSmartPointer<QVTKInteractor> interactor =
		//	vtkSmartPointer<QVTKInteractor>::New();
		//		interactor->SetDesiredUpdateRate(30);



		//	renderWindow->SetInteractor(interactor);
		mapper->Update();
		mapper2->Update();
		cylinderSource->Update();
		cubeSource->Update();
		renderWindow->WaitForCompletion();
		widget1->SetRenderWindow(renderWindow);
		widget1->update();
		//	renderWindow->Render();
		renderWindow->WaitForCompletion();
		renderWindow->Start();

		widgets.push_back(widget1);
	}
	return widgets;
}

void mainWindow::changeVisibilityActor1(){
	if(!ui.ExpertCB->isChecked()){
		ui.ExpertSlider->setDisabled(false);
		ui.ExpertThresholdSlider->setDisabled(false);
		for(int i = 0; i < actors.size(); i++){
			actors.at(i).first->SetVisibility(1);

		}
	} else{
		ui.ExpertSlider->setDisabled(true);
		ui.ExpertThresholdSlider->setDisabled(true);
		for(int i = 0; i < actors.size(); i++){
			actors.at(i).first->SetVisibility(0);

		}
	}
	repaintAll();

}

void mainWindow::changeVisibilityActor2(){
	if(!ui.IracCB->isChecked()){
		ui.IracSlider->setDisabled(false);
		ui.SegThresholdSlider->setDisabled(false);
		for(int i = 0; i < actors.size(); i++){
			actors.at(i).second->SetVisibility(1);
		}
	} else{
		ui.IracSlider->setDisabled(true);
		ui.SegThresholdSlider->setDisabled(true);
		for(int i = 0; i < actors.size(); i++){
			actors.at(i).second->SetVisibility(0);
		}
	}
	repaintAll();
}

void mainWindow::changeOpacityActor1(){
	for(int i = 0; i < actors.size(); i++){
		actors.at(i).first->GetProperty()->SetOpacity(((double)ui.ExpertSlider->value())/100.00f);
	}
	repaintAll();

}

void mainWindow::changeOpacityActor2(){
	for(int i = 0; i < actors.size(); i++){
		actors.at(i).second->GetProperty()->SetOpacity(((double)ui.IracSlider->value())/100.00f);
	}
	repaintAll();

}

void mainWindow::toggleSideBarActive(){
	sideBarActivated = !sideBarActivated;
	ui.VisualizationGB->setDisabled(!sideBarActivated);
	ui.CamerGB->setDisabled(!sideBarActivated);
}

void mainWindow::activateSideBar(){
	sideBarActivated = true;
	ui.VisualizationGB->setDisabled(!sideBarActivated);
	ui.CamerGB->setDisabled(!sideBarActivated);
}


void mainWindow::setRenderTupels(std::vector<RenderTupel*> tupels){
	renderTupels = tupels;
}

void mainWindow::changePCAlabelState(int state){
	showPCA();
}

void mainWindow::createPCAWidgets(){
	ui.treeWidget->clear();
	viewmode = 2;
	actors.clear();
	dataInputs.clear();
	widgets.clear();
	vtkSmartPointer<vtkRenderer> renderer2D =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow2D =
		vtkSmartPointer<vtkRenderWindow>::New();



	/*vtkSmartPointer<vtkRenderer> renderer3D =
	vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow3D =
	vtkSmartPointer<vtkRenderWindow>::New();*/
	for(int i = 0; i < renderTupels.size(); i++){
		//TODO Back In
		//renderTupels.at(i)->movePCA2D(100);
		//renderTupels.at(i)->movePCA3D(100);
		renderTupels.at(i)->getSegmentationLiverPCA2D()->setScalars(localScalarsSeg.at(i));
		
		if(ui.pcaLabelsCB->isChecked()){
		// Some points to plot.
		vtkPoints* polyLinePoints = vtkPoints::New();
		polyLinePoints->InsertPoint(i,renderTupels.at(i)->labelPosition[0], renderTupels.at(i)->labelPosition[1], renderTupels.at(i)->labelPosition[2]);

		vtkSmartPointer<vtkPolyData> test =
			vtkSmartPointer<vtkPolyData>::New();
		
		//test->GetPoints()->SetPoint(0,renderTupels.at(i)->getDistanceSeg2Expert()->labelPosition);
		//test->Update();
		test->SetPoints(polyLinePoints);
		vtkSmartPointer<vtkLabeledDataMapper> ldm =
			vtkSmartPointer<vtkLabeledDataMapper>::New();
		
		ldm->SetInput(test);
		vtkSmartPointer<vtkActor2D> labelActor =
			vtkSmartPointer<vtkActor2D>::New();
		labelActor->SetMapper(ldm);
		renderer2D->AddActor2D ( labelActor);
		}	
		//	renderTupels.at(i)->getSegmentationLiverPCA3D()->setScalars(localScalarsSeg.at(i));


		//TODO BACK IN
		vtkActor* actor2D = renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor();





		vtkSmartPointer<vtkTextActor> textActor = 
			vtkSmartPointer<vtkTextActor>::New();
		textActor->GetTextProperty()->SetFontSize ( 24 );
		textActor->SetPosition2 ( 10, 40 );
		renderer2D->AddActor2D ( textActor );
		textActor->SetInput ( "PCA View" );
		textActor->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );

		renderWindow2D->AddRenderer(renderer2D);
		////	renderWindow3D->AddRenderer(renderer3D);


		//FOR COORDINATE SYSTEM
		//	// Create two points, P0 and P1
		//	double p0Y[3] = {0.0, 0.0, 0.0};
		//	double p1Y[3] = {0.0 , renderTupels.at(i)->pcaY * ui.pcaScalingSlider->value() * ui.scalefactorSB->value(), 0.0};
		//	vtkSmartPointer<vtkLineSource> lineSourceY = 
		//		vtkSmartPointer<vtkLineSource>::New();
		//	lineSourceY->SetPoint1(p0Y);
		//	lineSourceY->SetPoint2(p1Y);
		//	lineSourceY->Update();

		//	// Visualize
		//	vtkSmartPointer<vtkPolyDataMapper> mapperY = 
		//		vtkSmartPointer<vtkPolyDataMapper>::New();
		//	mapperY->SetInputConnection(lineSourceY->GetOutputPort());
		//	vtkSmartPointer<vtkActor> actorY = 
		//		vtkSmartPointer<vtkActor>::New();
		//	actorY->SetMapper(mapperY);
		//	actorY->GetProperty()->SetLineWidth(1);
		//	actorY->GetProperty()->SetColor(255,0, 0);

		//	// Create two points, P0 and P1
		//	double p0X[3] = {0.0, 0.0, 0.0};
		//	double p1X[3] = {renderTupels.at(i)->pcaX * ui.pcaScalingSlider->value() * ui.scalefactorSB->value() , 0.0, 0.0};
		//	vtkSmartPointer<vtkLineSource> lineSourceX = 
		//		vtkSmartPointer<vtkLineSource>::New();
		//	lineSourceX->SetPoint1(p0X);
		//	lineSourceX->SetPoint2(p1X);
		//	lineSourceX->Update();

		//	// Visualize
		//	vtkSmartPointer<vtkPolyDataMapper> mapperX = 
		//		vtkSmartPointer<vtkPolyDataMapper>::New();
		//	mapperX->SetInputConnection(lineSourceX->GetOutputPort());
		//	vtkSmartPointer<vtkActor> actorX = 
		//		vtkSmartPointer<vtkActor>::New();
		//	actorX->SetMapper(mapperX);
		//	actorX->GetProperty()->SetLineWidth(1);
		//	actorX->GetProperty()->SetColor(0 ,0, 255);


		//Enable Picking
		vtkSmartPointer<vtkRenderWindowInteractor> interactor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
		interactor->SetDesiredUpdateRate(30);


		//vtkSmartPointer<vtkInteractorStyleRubberBandPick> style = 
		//	vtkSmartPointer<vtkInteractorStyleRubberBandPick>::New();
		vtkSmartPointer<MouseInteractorRubberBand> style = 
			vtkSmartPointer<MouseInteractorRubberBand>::New();

		//vtkSmartPointer<vtkInteractorStyleTrackballActor> style = 
		//	vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();

		/*	vtkSmartPointer<vtkInteractorStyleRubberBandZoom> style = 
		vtkSmartPointer<vtkInteractorStyleRubberBandZoom>::New();*/


		style->SetCurrentRenderer(renderer2D);
		interactor->SetInteractorStyle( style );

		vtkSmartPointer<vtkAreaPicker> areaPicker = 
			vtkSmartPointer<vtkAreaPicker>::New();

		areaPicker->SetRenderer(renderer2D);
		vtkSmartPointer<vtkCallbackCommand> pickCallback = 
			vtkSmartPointer<vtkCallbackCommand>::New();
		pickCallback->SetCallback ( PickCallbackFunction );

		//	std::pair< std::vector<RenderTupel*> , std::vector<int>* >* temp = new std::pair< std::vector<RenderTupel*> , std::vector<int>* >();
		//	pickerStruct pickData = {this->renderTupels, PCAselectedClusters, renderer2D};
		pickerStruct* pickData = new pickerStruct();
		pickData->renderTupelVec = this->renderTupels;
		pickData->renderer = renderer2D;
		pickData->pickedList = PCAselectedClusters;
		pickData->PCAtextActor = this->PCAtextActor;
		//	temp->first = this->renderTupels;
		//	temp->second = PCAselectedClusters;

		//		pickCallback->SetClientData(&this->renderTupels);
		pickCallback->SetClientData( pickData);
		areaPicker->AddObserver ( vtkCommand::EndPickEvent, pickCallback );
		interactor->SetPicker(areaPicker);
		// Begin mouse interaction

		renderWindow2D->SetInteractor(interactor);


		/*	renderer2D->AddActor(actorX);
		renderer2D->AddActor(actorY);*/
		renderer2D->AddActor(actor2D);
		//	renderer2D->AddActor(textActor);
		//	renderer3D->AddActor(actor3D);

		renderer2D->SetBackground(0,0,0);
		//	renderer3D->SetBackground(0,0,0);

//		actors.push_back(std::pair<vtkActor*,vtkActor*>(NULL,NULL));
		//	actors.push_back(std::pair<vtkActor*,vtkActor*>(actor2D,NULL));
		//	actors.push_back(std::pair<vtkActor2D*,vtkActor2D*>(actor2D,NULL));
		dataInputs.push_back(std::pair<vtkPolyData*,vtkPolyData*>(renderTupels.at(i)->getSegmentationLiverPCA2D()->getData(),renderTupels.at(i)->getSegmentationLiverPCA3D()->getData()));
		renderer2D->ResetCamera();
		//	renderer3D->ResetCamera();


		//renderer->SetBackground(.1, .3,.2); // Background color dark green

	}




	QVTKWidget *widget2D = new QVTKWidget();
	//	QVTKWidget *widget3D = new QVTKWidget();




	/*vtkSmartPointer<QVTKInteractor> interactor3D =
	vtkSmartPointer<QVTKInteractor>::New();
	interactor3D->SetDesiredUpdateRate(30);*/


	//	vtkSmartPointer<QVTKInteractor> interactor2D =
	//		vtkSmartPointer<QVTKInteractor>::New();
	////	interactor3D->SetDesiredUpdateRate(30);
	//
	//	renderWindow2D->SetInteractor(interactor2D);
	//	renderWindow3D->SetInteractor(interactor3D);

	widget2D->SetRenderWindow(renderWindow2D);
	widget2D->update();
	//widget3D->SetRenderWindow(renderWindow3D);
	//widget3D->update();

	renderWindow2D->Render();
	//	renderWindow3D->Render();

	widgets.push_back(widget2D);
	//widgets.push_back(widget3D);

	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiverPCA2D()->getData()->GetPointData()->SetScalars(localScalarsSeg.at(i));
		//	renderTupels.at(i)->getSegmentationLiverPCA3D()->getData()->GetPointData()->SetScalars(localScalarsSeg.at(i));
	}

	for(int i = 0; i < widgets.size(); i++){
		widgets.at(i)->update();
	}
}

void mainWindow::createAverageWidget(){
	actors.clear();
	dataInputs.clear();
	widgets.clear();


	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();

	vtkSmartPointer<vtkRenderer> renderer2 =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow2 =
		vtkSmartPointer<vtkRenderWindow>::New();

	vtkSmartPointer<vtkRenderer> renderer3 =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow3 =
		vtkSmartPointer<vtkRenderWindow>::New();

	QString str;
	ui.overSegmentationLabel->setText(str.setNum(averageBaseLiver->segMaxAverageDistance));
	ui.underSegmentationLabel->setText(str.setNum(averageBaseLiver->segMinAverageDistance));
	renderWindow->AddRenderer(renderer);
	renderWindow2->AddRenderer(renderer2);
	renderWindow3->AddRenderer(renderer3);
	//	Clustering a;
	//a.hierarchicalClusteringMinimalL(averageLiver->segAverage,averageLiver->SegAverageDistances);
	//a.hierarchicalClusteringAverageL(averageLiver->segAverage,averageLiver->SegAverageDistances);
	// Add the actor to the scene
	renderer->AddActor(averageBaseLiver->segAverage->getActor());
	renderer->SetBackground(0,0,0);

	renderer2->AddActor(averageLiver->segAverage->getActor());
	//	averageLiver->segAverage->getActor()->SetVisibility(0);
	renderer2->SetBackground(0,0,0);

	renderer3->AddActor(averageLiver->clusteringLiver->getActor());
	renderer3->SetBackground(0,0,0);

	// Setup the text and add it to the window
	vtkSmartPointer<vtkTextActor> textActor = 
		vtkSmartPointer<vtkTextActor>::New();
	textActor->GetTextProperty()->SetFontSize ( 24 );
	textActor->SetPosition2 ( 10, 40 );
	renderer->AddActor2D ( textActor );
	textActor->SetInput ( "All Average" );
	textActor->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );


	vtkSmartPointer<vtkTextActor> textActor2 = 
		vtkSmartPointer<vtkTextActor>::New();
	textActor2->GetTextProperty()->SetFontSize ( 24 );
	textActor2->SetPosition2 ( 10, 40 );
	renderer2->AddActor2D ( textActor2 );
	textActor2->SetInput ( "PCA Choice Average" );
	textActor2->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );

	vtkSmartPointer<vtkTextActor> textActor3 = 
		vtkSmartPointer<vtkTextActor>::New();
	textActor3->GetTextProperty()->SetFontSize ( 24 );
	textActor3->SetPosition2 ( 10, 40 );
	renderer3->AddActor2D ( textActor3 );
	textActor3->SetInput ( "Clustering" );
	textActor3->GetTextProperty()->SetColor ( 1.0,0.0,0.0 );

	renderer->ResetCamera();
	renderer2->ResetCamera();
	renderer3->ResetCamera();



	QVTKWidget *widget1 = new QVTKWidget();
	QVTKWidget *widget2 = new QVTKWidget();
	QVTKWidget *widget3 = new QVTKWidget();

	vtkSmartPointer<QVTKInteractor> interactor =
		vtkSmartPointer<QVTKInteractor>::New();
	//interactor->SetDesiredUpdateRate(30);

	vtkSmartPointer<QVTKInteractor> interactor2 =
		vtkSmartPointer<QVTKInteractor>::New();
	//interactor2->SetDesiredUpdateRate(30);

	vtkSmartPointer<QVTKInteractor> interactor3 =
		vtkSmartPointer<QVTKInteractor>::New();
	//	interactor3->SetDesiredUpdateRate(30);

	MouseInteractorStyle2* style = MouseInteractorStyle2::New();
	connect(style, SIGNAL(sendPointID(int, bool )), this, SLOT(highightClusters(int, bool)));
	MouseInteractorStyle2* style2 = MouseInteractorStyle2::New();
	connect(style2, SIGNAL(sendPointID(int, bool )), this, SLOT(highightClusters(int, bool)));
	MouseInteractorStyle2* style3 = MouseInteractorStyle2::New();
	connect(style3, SIGNAL(sendPointID(int, bool )), this, SLOT(highightClusters(int, bool)));
	interactor->SetInteractorStyle(style);
	interactor2->SetInteractorStyle(style2);
	interactor3->SetInteractorStyle(style3);
	style->SetDefaultRenderer(renderer);
	style2->SetDefaultRenderer(renderer2);
	style3->SetDefaultRenderer(renderer3);

	renderWindow->SetInteractor(interactor);
	renderWindow2->SetInteractor(interactor2);
	renderWindow3->SetInteractor(interactor3);


	widget1->SetRenderWindow(renderWindow);
	widget1->update();

	widget2->SetRenderWindow(renderWindow2);
	widget2->update();

	widget3->SetRenderWindow(renderWindow3);
	widget3->update();

	renderWindow->Render();
	renderWindow2->Render();
	renderWindow3->Render();

	widgets.push_back(widget1);

	widgets.push_back(widget2);

	widgets.push_back(widget3);



}



void mainWindow::createWidgets(std::set<int> choice){

	actors.clear();
	dataInputs.clear();
	widgets.clear();
	for(int i = 0; i < renderTupels.size(); i++){
		if(choice.find(i) != choice.end()){
			vtkActor* actor = renderTupels.at(i)->getExpertLiver()->getActor();
			vtkActor* actor2 = renderTupels.at(i)->getSegmentationLiver()->getActor();

			//vtkActor* actor = renderTupels.at(i)->getExpertLiver()->getActor();
			//vtkActor* actor2 = renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor();
			vtkSmartPointer<vtkRenderer> renderer =
				vtkSmartPointer<vtkRenderer>::New();
			vtkSmartPointer<vtkRenderWindow> renderWindow =
				vtkSmartPointer<vtkRenderWindow>::New();


			renderWindow->AddRenderer(renderer);


			//actor->GetProperty()->SetOpacity(((double)ui.ExpertSlider->value())/100.00f);
			//actor2->GetProperty()->SetOpacity(((double)ui.IracSlider->value())/100.00f);



			renderer->SetBackground(0,0,0);
			actors.push_back(std::pair<vtkActor*,vtkActor*>(actor,actor2));
			dataInputs.push_back(std::pair<vtkPolyData*,vtkPolyData*>(renderTupels.at(i)->getExpertLiver()->getData(),renderTupels.at(i)->getSegmentationLiver()->getData()));
			renderer->ResetCamera();

			//renderer->SetBackground(.1, .3,.2); // Background color dark green
			QVTKWidget *widget1 = new QVTKWidget();


			// Add the actor to the scene
			renderer->AddActor(actor);
			renderer->AddActor(actor2);

			// Setup the text and add it to the window
			vtkSmartPointer<vtkTextActor> textActor = 
				vtkSmartPointer<vtkTextActor>::New();
			textActor->GetTextProperty()->SetFontSize ( 100 );
			textActor->SetPosition2 ( 100, 100 );
			std::string text = "ID: ";
			text.append(QString::number(i).toStdString());
			renderer->AddActor2D ( textActor );
			textActor->SetInput ( text.c_str());
			textActor->GetTextProperty()->SetColor ( 255.0,0.0,0.0 );



			widget1->SetRenderWindow(renderWindow);
			widget1->update();
			renderWindow->Render();
			renderWindow->Start();

			//interactor->Start();
			widgets.push_back(widget1);
		}
	}
}


void mainWindow::createWidgets(){

	actors.clear();
	dataInputs.clear();
	widgets.clear();
	for(int i = 0; i < renderTupels.size(); i++){
		vtkActor* actor = renderTupels.at(i)->getExpertLiver()->getActor();
		vtkActor* actor2 = renderTupels.at(i)->getSegmentationLiver()->getActor();

		//vtkActor* actor = renderTupels.at(i)->getExpertLiver()->getActor();
		//vtkActor* actor2 = renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor();
		vtkSmartPointer<vtkRenderer> renderer =
			vtkSmartPointer<vtkRenderer>::New();
		vtkSmartPointer<vtkRenderWindow> renderWindow =
			vtkSmartPointer<vtkRenderWindow>::New();


		renderWindow->AddRenderer(renderer);


		//actor->GetProperty()->SetOpacity(((double)ui.ExpertSlider->value())/100.00f);
		//actor2->GetProperty()->SetOpacity(((double)ui.IracSlider->value())/100.00f);

		cout << "test";

		renderer->SetBackground(0,0,0);
		actors.push_back(std::pair<vtkActor*,vtkActor*>(actor,actor2));
		dataInputs.push_back(std::pair<vtkPolyData*,vtkPolyData*>(renderTupels.at(i)->getExpertLiver()->getData(),renderTupels.at(i)->getSegmentationLiver()->getData()));
		renderer->ResetCamera();

		//renderer->SetBackground(.1, .3,.2); // Background color dark green
		QVTKWidget *widget1 = new QVTKWidget();


		// Begin mouse interaction

		// Add the actor to the scene
		renderer->AddActor(actor);
		renderer->AddActor(actor2);

		// Setup the text and add it to the window
		vtkSmartPointer<vtkTextActor> textActor = 
			vtkSmartPointer<vtkTextActor>::New();
		textActor->GetTextProperty()->SetFontSize ( 100 );
		//textActor->SetPosition2 ( 20, 40 );
		textActor->SetPosition2 ( 0, 0 );
		std::string text = "ID: ";
		text.append(QString::number(i).toStdString());
		renderer->AddActor2D ( textActor );
		textActor->SetInput ( text.c_str());
		textActor->GetTextProperty()->SetColor ( 255.0,0.0,0.0 );

		widget1->SetRenderWindow(renderWindow);
		widget1->update();
		renderWindow->Render();
		renderWindow->Start();

		//interactor->Start();
		widgets.push_back(widget1);
	}
}


void mainWindow::createWidgetsClustering(){
	//	actors.clear();
	//	dataInputs.clear();
	//	widgets.clear();
	//
	//	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
	//
	//	linkage clusterLinkage;
	//	clusterLinkage = fastTest;
	//	if(fastTestCluster == NULL)
	//		fastTestCluster= Clustering::hierarchicalClustering(renderTupels.at(0)->getExpertLiver(), renderTupels.at(0)->getDistanceExpert2Seg()->getDistanceArray(), clusterLinkage);
	////	minimalCluster = Clustering::hierarchicalClustering(averageLiver->segAverage, renderTupels.at(0)->getDistanceSeg2Expert()->getDistanceArray(), clusterLinkage);
	////	clusters = clustering.hierarchicalClusteringAverageL(renderTupels.at(0)->getSegmentationLiver(), renderTupels.at(0)->getDistanceSeg2Expert()->getDistanceArray());
	//	double temp = ui.clusteringThresholdSlider->value();
	//	Clustering::getAllClusterVector(fastTestCluster, (temp/100) * fastTestCluster->distance, allClusters);
	//	QString qStr = QString::number(allClusters->size());
	////	QString number( allClusters->size(), 10);
	//	ui.clusterCountLabel->setText(qStr);
	//	QPalette palette = ui.clusterCountLabel->palette();
	////	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	//	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	//	ui.clusterCountLabel->setPalette(palette);
	//
	//		ColorSource cs;
	//	renderTupels.at(0)->getExpertLiver()->setLookupTable(cs.getRainbowLookupTable(allClusters->size()), 0, allClusters->size() - 1);
	//		vtkPolyData* data = renderTupels.at(0)->getExpertLiver()->getData();
	//		data->GetPointData()->SetScalars(cs.getScalars(allClusters, 0));
	////	renderTupels.at(0)->getSegmentationLiver()->setScalars(cs.getScalars(allClusters));
	//		vtkPolyDataMapper * mapper;
	//		vtkActor * actor;
	//		
	//		mapper = vtkPolyDataMapper::New();
	//		mapper->SetLookupTable(cs.getRainbowLookupTable(allClusters->size()));
	//		mapper->SetScalarRange(0, allClusters->size());
	//	//	mapper->SetInput(renderTupels.at(0)->getSegmentationLiver()->getData());
	//		mapper->SetInput(data);
	//		actor = vtkActor::New();
	//		actor->SetMapper(mapper);
	//	//	actor->GetProperty()->BackfaceCullingOn();
	//		actor->Modified();
	//	//	vtkActor* actor = renderTupels.at(0)->getSegmentationLiver()->getActor();
	//		
	//		//vtkActor* actor = renderTupels.at(i)->getExpertLiver()->getActor();
	//		//vtkActor* actor2 = renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor();
	//		vtkSmartPointer<vtkRenderer> renderer =
	//			vtkSmartPointer<vtkRenderer>::New();
	//		vtkSmartPointer<vtkRenderWindow> renderWindow =
	//			vtkSmartPointer<vtkRenderWindow>::New();
	//
	//
	//		renderWindow->AddRenderer(renderer);
	//
	//
	//		//actor->GetProperty()->SetOpacity(((double)ui.ExpertSlider->value())/100.00f);
	//		//actor2->GetProperty()->SetOpacity(((double)ui.IracSlider->value())/100.00f);
	//
	//
	//		// Add the actor to the scene
	//		renderer->AddActor(actor);
	//
	//		renderer->SetBackground(0,0,0);
	//	//	actors.push_back(std::pair<vtkActor*,vtkActor*>(actor,actor2));
	//		dataInputs.push_back(std::pair<vtkPolyData*,vtkPolyData*>(renderTupels.at(0)->getExpertLiver()->getData(),renderTupels.at(0)->getSegmentationLiver()->getData()));
	//		renderer->ResetCamera();
	//
	//		//renderer->SetBackground(.1, .3,.2); // Background color dark green
	//		QVTKWidget *widget1 = new QVTKWidget();
	//
	//		vtkSmartPointer<QVTKInteractor> interactor =
	//			vtkSmartPointer<QVTKInteractor>::New();
	//		interactor->SetDesiredUpdateRate(30);
	//		//vtkSmartPointer<InteractorStyleMoveGlyph> style =
	//		//vtkSmartPointer<InteractorStyleMoveGlyph>::New();
	//		//style->Data = renderTupels.at(0)->getSegmentationLiverPCA2D()->getData();
	//		//style->GlyphData = renderTupels.at(0)->getSegmentationLiverPCA2D()->getData();
	//		//interactor->SetInteractorStyle(style);
	//
	//
	//		renderWindow->SetInteractor(interactor);
	//
	//		widget1->SetRenderWindow(renderWindow);
	//		widget1->update();
	//		renderWindow->Render();
	//		widgets.push_back(widget1);

}

void mainWindow::changeExpertScalars(){
	//#pragma omp parallel for
	if(ui.globalRad->isChecked()){
		for(int i = 0; i < renderTupels.size(); i++){
			vtkDoubleArray* newScalar = vtkDoubleArray::New();

			for(int j = 0; j <renderTupels.at(i)->getExpertLiver()->getData()->GetNumberOfPoints(); j++){
				if((abs(renderTupels.at(i)->getDistanceExpert2Seg()->getDistanceTupelForIndexOfSetA(j).getDistance())/maxdistExp) < ((double) ui.ExpertThresholdSlider->value()/100.0)){
					//	double slider = ui.ExpertThresholdSlider->value()/100.0;
					//		double tuple = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j);
					//		scalars.at(i)->GetTuple1(j) ;

					//	double a = scalars.at(i)->GetTuple1(j) + 20.0;//renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j); + 20.0L;
					double a = globalScalarsExp.at(i)->GetTuple1(j) + 21.0L;
					//	double a = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j) + 20.0L;
					newScalar->InsertTuple1(j, a);
					//	const double* tmp = new double();
					//		renderTupels.at(i)->getExpertLiver()->getScalars()->InsertTuple1(j, a);
				}
				else{
					//		newScalar->InsertTuple1(j, scalars.at(i)->GetTuple1(j));
					newScalar->InsertTuple1(j, globalScalarsExp.at(i)->GetTuple1(j));
				}
				renderTupels.at(i)->getExpertLiver()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getExpertLiver()->getActor()->GetMapper()->Modified();
				widgets.at(i)->update();
			}
		}
	}
	else{

		for(int i = 0; i < renderTupels.size(); i++){
			vtkDoubleArray* newScalar = vtkDoubleArray::New();

			for(int j = 0; j <renderTupels.at(i)->getExpertLiver()->getData()->GetNumberOfPoints(); j++){
				if((abs(renderTupels.at(i)->getDistanceExpert2Seg()->getDistanceTupelForIndexOfSetA(j).getDistance())/renderTupels.at(i)->getDistanceExpert2Seg()->getMaximalRelativeDistance()) < ((double) ui.ExpertThresholdSlider->value()/100.0)){
					//	double slider = ui.ExpertThresholdSlider->value()/100.0;
					//		double tuple = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j);
					//		scalars.at(i)->GetTuple1(j) ;

					//	double a = scalars.at(i)->GetTuple1(j) + 20.0;//renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j); + 20.0L;
					double a = localScalarsExp.at(i)->GetTuple1(j) + 21.0L;
					newScalar->InsertTuple1(j, a);
					//	const double* tmp = new double();
					//		renderTupels.at(i)->getExpertLiver()->getScalars()->InsertTuple1(j, a);
				}
				else{
					//		newScalar->InsertTuple1(j, scalars.at(i)->GetTuple1(j));
					newScalar->InsertTuple1(j, localScalarsExp.at(i)->GetTuple1(j));
				}
				renderTupels.at(i)->getExpertLiver()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getExpertLiver()->getActor()->GetMapper()->Modified();
				widgets.at(i)->update();
			}
		}


	}

}


void mainWindow::changeSegScalars(){
	//	#pragma omp parallel for
	if(ui.globalRad->isChecked()){
		for(int i = 0; i < renderTupels.size(); i++){
			vtkDoubleArray* newScalar = vtkDoubleArray::New();

			for(int j = 0; j <renderTupels.at(i)->getSegmentationLiver()->getData()->GetNumberOfPoints(); j++){
				if((abs(renderTupels.at(i)->getDistanceSeg2Expert()->getDistanceTupelForIndexOfSetA(j).getDistance())/maxdistSeg) < ((double) ui.SegThresholdSlider->value()/100.0)){
					//	double slider = ui.ExpertThresholdSlider->value()/100.0;
					//		double tuple = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j);
					//		scalars.at(i)->GetTuple1(j) ;

					double a = globalScalarsSeg.at(i)->GetTuple1(j) + 21.0L;
					newScalar->InsertTuple1(j, a);
					//	const double* tmp = new double();
					//		renderTupels.at(i)->getExpertLiver()->getScalars()->InsertTuple1(j, a);
				}
				else{
					newScalar->InsertTuple1(j, globalScalarsSeg.at(i)->GetTuple1(j));
				}
				renderTupels.at(i)->getSegmentationLiver()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();
				renderTupels.at(i)->getSegmentationLiverPCA2D()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor()->GetMapper()->Modified();
				renderTupels.at(i)->getSegmentationLiverPCA3D()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiverPCA3D()->getActor()->GetMapper()->Modified();

			}
		}
		for(int i = 0; i < widgets.size(); i++)
			widgets.at(i)->update();

	}

	else{
		for(int i = 0; i < renderTupels.size(); i++){
			vtkDoubleArray* newScalar = vtkDoubleArray::New();

			for(int j = 0; j <renderTupels.at(i)->getSegmentationLiver()->getData()->GetNumberOfPoints(); j++){
				if((abs(renderTupels.at(i)->getDistanceSeg2Expert()->getDistanceTupelForIndexOfSetA(j).getDistance())/renderTupels.at(i)->getDistanceSeg2Expert()->getMaximalRelativeDistance()) < ((double) ui.SegThresholdSlider->value()/100.0)){
					//	double slider = ui.ExpertThresholdSlider->value()/100.0;
					//		double tuple = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j);
					//		scalars.at(i)->GetTuple1(j) ;

					double a = localScalarsSeg.at(i)->GetTuple1(j) + 21.0L;
					newScalar->InsertTuple1(j, a);
					//	const double* tmp = new double();
					//		renderTupels.at(i)->getExpertLiver()->getScalars()->InsertTuple1(j, a);
				}
				else{
					newScalar->InsertTuple1(j, localScalarsSeg.at(i)->GetTuple1(j));
				}
				renderTupels.at(i)->getSegmentationLiver()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();
				renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();
				renderTupels.at(i)->getSegmentationLiverPCA2D()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiverPCA2D()->getActor()->GetMapper()->Modified();
				renderTupels.at(i)->getSegmentationLiverPCA3D()->getData()->GetPointData()->SetScalars(newScalar);
				renderTupels.at(i)->getSegmentationLiverPCA3D()->getActor()->GetMapper()->Modified();

			}
		}
		for(int i = 0; i < widgets.size(); i++)
			widgets.at(i)->update();
	}
}

void mainWindow::changeAvgScalars(){
	//	#pragma omp parallel for

	vtkDoubleArray* newScalar = vtkDoubleArray::New();
	double absMax = averageLiver->segMaxAverageDistance;
	if(abs(averageLiver->segMinAverageDistance) > abs(absMax))
		absMax = abs(averageLiver->segMinAverageDistance);
	for(int j = 0; j < averageLiver->segAverage->getData()->GetNumberOfPoints(); j++){

		if((abs(averageLiver->SegAverageDistances->GetValue(j))/absMax) < ((double) ui.avgThresholdSlider->value()/100.0)){
			//	double slider = ui.ExpertThresholdSlider->value()/100.0;
			//		double tuple = renderTupels.at(i)->getExpertLiver()->getScalars()->GetTuple1(j);
			//		scalars.at(i)->GetTuple1(j) ;

			double a = averageLiver->scalars->GetValue(j) + 21.0L;
			newScalar->InsertValue(j, a);
			//	const double* tmp = new double();
			//		renderTupels.at(i)->getExpertLiver()->getScalars()->InsertTuple1(j, a);
		}
		else{
			double test = averageLiver->scalars->GetValue(j);
			newScalar->InsertValue(j, averageLiver->scalars->GetValue(j));
		}

	}
	//	averageLiver->scalars = newScalar;
	averageLiver->segAverage->setScalars(newScalar);
	averageLiver->segAverage->getActor()->GetMapper()->Modified();

	for(int i = 0; i < widgets.size(); i++)
		widgets.at(i)->update();
}



void mainWindow::changeScalarsToGlobal(){
	//	#pragma omp parallel for
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiver()->getData()->GetPointData()->SetScalars(globalScalarsSeg.at(i));
		renderTupels.at(i)->getExpertLiver()->getData()->GetPointData()->SetScalars(globalScalarsExp.at(i));
		/*	renderTupels.at(i)->getExpertLiver()->getActor()->GetMapper()->Modified();
		renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();*/
		widgets.at(i)->update();
	}
}

void mainWindow::changeScalarsToGlobalQuartiles(){
	//	#pragma omp parallel for
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiver()->getData()->GetPointData()->SetScalars(globalQuartileScalarsSeg.at(i));
		renderTupels.at(i)->getExpertLiver()->getData()->GetPointData()->SetScalars(globalQuartileScalarsExp.at(i));
		/*	renderTupels.at(i)->getExpertLiver()->getActor()->GetMapper()->Modified();
		renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();*/
		widgets.at(i)->update();
	}
}



void mainWindow::changeAvgLiverToHisEukAverageClustering(){
	ui.clusterHighlightingBt->setEnabled(true);
	//if(viewmode != 3){
	changeToAverageGui();
	renderAverageGrid();
	//}
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();
	double thresh = ui.clusteringThresholdSlider->value();
	double temp = histogramEuklidAverageLinkage->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(histogramEuklidAverageLinkage, temp, allClusters);
	//Clustering::getAllClusterVector(histogramEuklidAverageLinkage, -1, allClusters);
	int counter = 0;
	for(int i = 0; i < allClusters->size(); i++){
		counter += (int) allClusters->at(i).size();
	}

	cout << "\n Points in Cluster: " << counter << "\n";

	QString qStr = QString::number(allClusters->size());
	//	QString number( allClusters->size(), 10);
	ui.clusterCountLabel->setText(qStr);
	QPalette palette = ui.clusterCountLabel->palette();
	//	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	ui.clusterCountLabel->setPalette(palette);
	ColorSource cs;
	averageLiver->clusteringLiver->setLookupTable(cs.getRainbowLookupTable((int) allClusters->size()), 0, (allClusters->size() * 2) - 1);
	averageLiver->clusteringLiver->setScalars(cs.getScalars(allClusters, 0));

	//TODO CHANGE TO HISTOGRAM DISTANCES!
	emit sendClusteringData(histogramEuklidAverageLinkage, thresh, averageLiver->SegAverageDistances,  PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());
	std::vector<vtkActor*> actors = Clustering::drawBorders(averageLiver->clusteringLiver->getData(), Clustering::transformToClusterMap(allClusters));
	for(int i = 0; i < actors.size(); i++){
		if(ui.allSampleBorderCB->isChecked())
			widgets.at(0)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.PCAchoiceBorderCB->isChecked())
			widgets.at(1)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.clusteringBorderCB->isChecked())
			widgets.at(2)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));
	}
	repaintAll();

}

void mainWindow::changeAvgLiverToHisEukMaxClustering(){
	ui.clusterHighlightingBt->setEnabled(true);
	//if(viewmode != 3){
	changeToAverageGui();
	renderAverageGrid();
	//}
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

	double temp = histogramEuklidMaximumLinkage->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(histogramEuklidMaximumLinkage, temp, allClusters);
	QString qStr = QString::number(allClusters->size());
	//	QString number( allClusters->size(), 10);
	ui.clusterCountLabel->setText(qStr);
	QPalette palette = ui.clusterCountLabel->palette();
	//	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	ui.clusterCountLabel->setPalette(palette);
	ColorSource cs;
	averageLiver->clusteringLiver->setLookupTable(cs.getRainbowLookupTable((int) allClusters->size()), 0, (allClusters->size() * 2) - 1);
	//averageLiver->clusteringLiver->getData()->GetPointData()->SetScalars(cs.getScalars(allClusters, 0));
	averageLiver->clusteringLiver->setScalars(cs.getScalars(allClusters, 0));

	//TODO CHANGE TO HISTOGRAM DISTANCES!
	emit sendClusteringData(histogramEuklidMaximumLinkage, ui.clusteringThresholdSlider->value(), averageLiver->SegAverageDistances,  PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());
	std::vector<vtkActor*> actors = Clustering::drawBorders(averageLiver->clusteringLiver->getData(), Clustering::transformToClusterMap(allClusters));
	for(int i = 0; i < actors.size(); i++){
		if(ui.allSampleBorderCB->isChecked())
			widgets.at(0)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.PCAchoiceBorderCB->isChecked())
			widgets.at(1)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.clusteringBorderCB->isChecked())
			widgets.at(2)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));
	}
	repaintAll();

}

void mainWindow::changeAvgLiverToHisMaxMaxClustering(){
	ui.clusterHighlightingBt->setEnabled(true);
	//if(viewmode != 3){
	changeToAverageGui();
	renderAverageGrid();
	//}
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

	double temp = histogramMaxmimumMaximumLinkage->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(histogramMaxmimumMaximumLinkage, temp, allClusters);
	QString qStr = QString::number(allClusters->size());
	//	QString number( allClusters->size(), 10);
	ui.clusterCountLabel->setText(qStr);
	QPalette palette = ui.clusterCountLabel->palette();
	//	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	ui.clusterCountLabel->setPalette(palette);
	ColorSource cs;
	averageLiver->clusteringLiver->setLookupTable(cs.getRainbowLookupTable((int) allClusters->size()), 0, (allClusters->size() * 2) - 1);
	//averageLiver->clusteringLiver->getData()->GetPointData()->SetScalars(cs.getScalars(allClusters, 0));
	averageLiver->clusteringLiver->setScalars(cs.getScalars(allClusters, 0));

	//TODO CHANGE TO HISTOGRAM DISTANCES!
	emit sendClusteringData(histogramMaxmimumMaximumLinkage,ui.clusteringThresholdSlider->value(), averageLiver->SegAverageDistances,  PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());
	std::vector<vtkActor*> actors = Clustering::drawBorders(averageLiver->clusteringLiver->getData(), Clustering::transformToClusterMap(allClusters));
	for(int i = 0; i < actors.size(); i++){
		if(ui.allSampleBorderCB->isChecked())
			widgets.at(0)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.PCAchoiceBorderCB->isChecked())
			widgets.at(1)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.clusteringBorderCB->isChecked())
			widgets.at(2)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));
	}
	repaintAll();

}

void mainWindow::changeAvgLiverToHisMaxAvgClustering(){
	ui.clusterHighlightingBt->setEnabled(true);
	//if(viewmode != 3){
	changeToAverageGui();
	renderAverageGrid();
	//}
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

	double temp = histogramMaximumAvgLinkage->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(histogramMaximumAvgLinkage, temp, allClusters);
	//vtkActor actor = Clustering::drawBorders(averageLiver->clusteringLiver, Clustering::transformToClusterMap(allClusters));
	QString qStr = QString::number(allClusters->size());
	//	QString number( allClusters->size(), 10);
	ui.clusterCountLabel->setText(qStr);
	QPalette palette = ui.clusterCountLabel->palette();
	//	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	ui.clusterCountLabel->setPalette(palette);
	ColorSource cs;
	averageLiver->clusteringLiver->setLookupTable(cs.getRainbowLookupTable((int) allClusters->size()), 0, (allClusters->size() * 2) - 1);
	//averageLiver->clusteringLiver->getData()->GetPointData()->SetScalars(cs.getScalars(allClusters, 0));
	averageLiver->clusteringLiver->setScalars(cs.getScalars(allClusters, 0));

	//TODO CHANGE TO HISTOGRAM DISTANCES!
	emit sendClusteringData(histogramMaximumAvgLinkage, ui.clusteringThresholdSlider->value(), averageLiver->SegAverageDistances, PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());
	std::vector<vtkActor*> actors = Clustering::drawBorders(averageLiver->clusteringLiver->getData(), Clustering::transformToClusterMap(allClusters));
	for(int i = 0; i < actors.size(); i++){
		if(ui.allSampleBorderCB->isChecked())
			widgets.at(0)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.PCAchoiceBorderCB->isChecked())
			widgets.at(1)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));

		if(ui.clusteringBorderCB->isChecked())
			widgets.at(2)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actors.at(i));
	}
	repaintAll();

}

void mainWindow::changeScalarsToLocal(){
	//	#pragma omp parallel for
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiver()->getData()->GetPointData()->SetScalars(localScalarsSeg.at(i));
		renderTupels.at(i)->getExpertLiver()->getData()->GetPointData()->SetScalars(localScalarsExp.at(i));
		//renderTupels.at(i)->getExpertLiver()->getActor()->GetMapper()->Modified();
		//renderTupels.at(i)->getSegmentationLiver()->getActor()->GetMapper()->Modified();
		widgets.at(i)->update();
	}
}


void mainWindow::setupScalars(){
	//	#pragma omp parallel for

	localScalarsExp.clear();
	localScalarsSeg.clear();
	globalScalarsExp.clear();
	globalScalarsSeg.clear();
	maxdistSeg = 0;
	maxdistExp = 0;
	mindistSeg = 0;
	mindistExp = 0;
	std::vector<double> positivDistancesSeg;
	std::vector<double> positivDistancesExp;
	std::vector<double> negativeDistancesSeg;
	std::vector<double> negativeDistancesExp;
	for(int i = 0; i < renderTupels.size(); i++){

		localScalarsSeg.push_back(renderTupels.at(i)->getSegmentationLiver()->getScalars());
		localScalarsExp.push_back(renderTupels.at(i)->getExpertLiver()->getScalars());
		for(int j = 0; j < renderTupels.at(i)->getExpertLiver()->getData()->GetNumberOfPoints(); j++){
			//if(renderTupels.at(i).getDistanceExpert2Seg()->getDistanceArray()[j] < 0)
			positivDistancesExp.push_back(renderTupels.at(i)->getDistanceExpert2Seg()->getDistanceArray()->GetValue(j));
		}

		for(int j = 0; j < renderTupels.at(i)->getSegmentationLiver()->getData()->GetNumberOfPoints(); j++){
			//if(renderTupels.at(i).getDistanceExpert2Seg()->getDistanceArray()[j] < 0)
			positivDistancesSeg.push_back(renderTupels.at(i)->getDistanceSeg2Expert()->getDistanceArray()->GetValue(j));
		}

		//if(maxdistSeg < renderTupels.at(i)->getDistanceSeg2Expert()->getMaximalRelativeDistance()){
		//			maxdistSeg = renderTupels.at(i)->getDistanceSeg2Expert()->getMaximalRelativeDistance();
		//			}
		//if(maxdistExp < renderTupels.at(i)->getDistanceExpert2Seg()->getMaximalRelativeDistance()){
		//			maxdistExp = renderTupels.at(i)->getDistanceExpert2Seg()->getMaximalRelativeDistance();
		//}
		//if(mindistSeg > renderTupels.at(i)->getDistanceSeg2Expert()->getMinimalRelativeDistance()){
		//	mindistSeg = renderTupels.at(i)->getDistanceSeg2Expert()->getMinimalRelativeDistance();
		//}
		//if(mindistExp > renderTupels.at(i)->getDistanceExpert2Seg()->getMinimalRelativeDistance()){
		//	mindistExp = renderTupels.at(i)->getDistanceExpert2Seg()->getMinimalRelativeDistance();
		//}

	}
	std::sort(positivDistancesExp.begin(), positivDistancesExp.end());
	std::sort(positivDistancesSeg.begin(), positivDistancesSeg.end());
	maxdistExp =	positivDistancesExp.at(positivDistancesExp.size() - 1);
	mindistExp =	positivDistancesExp.at(0);
	maxdistSeg =	positivDistancesSeg.at(positivDistancesSeg.size() - 1);
	mindistSeg =	positivDistancesSeg.at(0);
	ColorSource cs;
	for(int i = 0; i < renderTupels.size(); i++){
		globalScalarsExp.push_back(cs.getScalars(renderTupels.at(i)->getDistanceExpert2Seg(), maxdistExp, mindistExp));
		globalScalarsSeg.push_back(cs.getScalars(renderTupels.at(i)->getDistanceSeg2Expert(), maxdistSeg, mindistSeg));
		globalQuartileScalarsExp.push_back(cs.getScalars(renderTupels.at(i)->getDistanceExpert2Seg(), positivDistancesExp.at((positivDistancesExp.size() - 1) * 0.99), positivDistancesExp.at((positivDistancesExp.size() - 1) * 0.05)));
		globalQuartileScalarsSeg.push_back(cs.getScalars(renderTupels.at(i)->getDistanceSeg2Expert(), positivDistancesSeg.at((positivDistancesSeg.size() - 1) * 0.99), positivDistancesSeg.at((positivDistancesSeg.size() - 1) * 0.05)));
	}	
}



void mainWindow::showExpertColor(){
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getExpertLiver()->setScalarVisibility(!ui.ExpertShowColorBut->isChecked());
		widgets.at(i)->update();
	}
}

void mainWindow::showClusterColor(){

	if(ui.edal_bt->isChecked())
		changeAvgLiverToHisEukAverageClustering();

	if(ui.edml_Bt->isChecked())
		changeAvgLiverToHisEukMaxClustering();

	if(ui.mdml_bt->isChecked())
		changeAvgLiverToHisMaxMaxClustering();

	if(ui.mdal_bt->isChecked())
		changeAvgLiverToHisMaxAvgClustering();


}

void mainWindow::showSegColor(){
	//Old version
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->getSegmentationLiver()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
		renderTupels.at(i)->getSegmentationLiverPCA2D()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
		renderTupels.at(i)->getSegmentationLiverPCA3D()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
		//	widgets.at(i)->update();
	}
	for(int j = 0; j < widgets.size(); j++){
		widgets.at(j)->update();
	}
}
//for(int i = 0; i < actors.size(); i++){
//	renderTupels.at(i)->getSegmentationLiver()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
////	renderTupels.at(i)->getSegmentationLiverPCA2D()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
////	renderTupels.at(i)->getSegmentationLiverPCA3D()->setScalarVisibility(!ui.SegShowColorBut->isChecked());
//}
//for(int i = 0; i < widgets.size(); i++){
//	widgets.at(i)->update();
//}
//}

void mainWindow::changeBackgroundColor(){
	double color[3];
	if(ui.blackBG->isChecked()){
		color[0] = 0.0;
		color[1] = 0.0;
		color[2] = 0.0;
	}

	if(ui.whiteBG->isChecked()){
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}

	if(ui.greyBG->isChecked()){
		color[0] = 0.2;
		color[1] = 0.2;
		color[2] = 0.2;
	}

		for(int i = 0; i < widgets.size(); i++){
			widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetBackground(color);
			widgets.at(i)->update();
		}
}

void mainWindow::changeBackgroundToGrey(){
	for(int i = 0; i < widgets.size(); i++){
		widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetBackground(0.2,0.2,0.2);
		widgets.at(i)->update();
	}
}
void mainWindow::changeBackgroundToBlack(){
	for(int i = 0; i < widgets.size(); i++){
		widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetBackground(0.0,0.0,0.0);
		widgets.at(i)->update();
	}
}

void mainWindow::changeBackgroundToWhite(){
	for(int i = 0; i < widgets.size(); i++){
		widgets.at(i)->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetBackground(1.0,1.0,1.0);
		widgets.at(i)->update();
	}
}

void mainWindow::setBigBarProgress(int i)
{
	if(i == 0){
		ui.progressBarBig->setValue(0);
	}else{
		ui.progressBarBig->setValue(ui.progressBarBig->value() + ((1.0f/i)*100));
	}
}

void mainWindow::setSmallBarProgress( float i )
{

}

void mainWindow::scalePCA()
{
	for(int i = 0; i < renderTupels.size(); i++){
		renderTupels.at(i)->movePCA2D(ui.pcaScalingSlider->value() * ui.scalefactorSB->value() * PCASCALE);
		//	renderTupels.at(i)->movePCA3D(ui.pcaScalingSlider->value());
	}
	createPCAWidgets();
	createGrid(columns);
	cameraSync();
	//for(int i = 0; i < widgets.size(); i++){
	//	widgets.at(i)->update();
	//}
}

void mainWindow::renderGrid(std::set<int> choice){
	createWidgets(choice);
	//createWidgetsClustering();
	createGrid(columns);
	cameraSync();
}

void mainWindow::renderGrid(){
	createWidgets();
	//createWidgetsClustering();
	createGrid(columns);
	cameraSync();
}

void mainWindow::renderAverageGrid(){
	createAverageWidget();
	createAverageGrid();
	//createGrid(columns);
	cameraSync();
}

void mainWindow::showGrid(std::set<int> choice)
{
	ui.CamerGB->show();
	ui.line_2->show();
	ui.ExpertCB->show();
	ui.ExpertShowColorBut->show();
	ui.label->show();
	ui.ExpertSlider->show();
	ui.label_4->show();
	ui.ExpertThresholdSlider->show();
	ui.line->show();
	ui.IracCB->show();
	ui.label_2->show();
	ui.IracSlider->show();

	ui.TestingGB->hide();

	ui.pcaGB->hide();
	ui.VisualizationGB->show();
	ui.avgGB->hide();

	ui.TestingGB->hide();
	if(ui.actionPCA->isChecked()){
		ui.actionGrid->setChecked(true);
		ui.actionPCA->setChecked(false);
	}
	if(ui.actionAverage_Liver->isChecked()){
		ui.actionGrid->setChecked(true);
		ui.actionAverage_Liver->setChecked(false);
	}

	renderGrid(choice);
}


void mainWindow::showGrid()
{
	ui.ClusteringThresholdFrame->hide();
	ui.VisualizationGB->show();
	ui.CamerGB->show();
	ui.line_2->show();
	ui.ExpertCB->show();
	ui.ExpertShowColorBut->show();
	ui.label->show();
	ui.ExpertSlider->show();
	ui.label_4->show();
	ui.ExpertThresholdSlider->show();
	ui.line->show();
	ui.IracCB->show();
	ui.label_2->show();
	ui.IracSlider->show();

	ui.TestingGB->hide();

	ui.pcaGB->hide();
	ui.VisualizationGB->show();
	ui.avgGB->hide();

	ui.TestingGB->hide();
	if(ui.actionPCA->isChecked()){
		ui.actionGrid->setChecked(true);
		ui.actionPCA->setChecked(false);
	}
	if(ui.actionAverage_Liver->isChecked()){
		ui.actionGrid->setChecked(true);
		ui.actionAverage_Liver->setChecked(false);
	}

	renderGrid();
}
void mainWindow::changeToAverageGui() 
{
	ui.ClusteringThresholdFrame->show();
	viewmode = 3;
	ui.avgGB->show();
	//ui.VisualizationGB->hide();
	ui.CamerGB->hide();
	ui.pcaGB->hide();
	ui.VisualizationGB->hide();
	//ui.InteractionGB->hide();
	if(ui.actionGrid->isChecked()){
		ui.actionGrid->setChecked(false);
		ui.actionAverage_Liver->setChecked(true);
	}
	if(ui.actionPCA->isChecked()){
		ui.actionAverage_Liver->setChecked(true);
		ui.actionPCA->setChecked(false);
	}
}
void mainWindow::showAverage()
{
	changeToAverageGui();
	renderAverageGrid();
	showClusterColor();
}

void mainWindow::showPCA()
{
	ui.ClusteringThresholdFrame->hide();
	ui.VisualizationGB->hide();
	ui.TestingGB->hide();
	ui.pcaGB->show();
	ui.CamerGB->hide();
	ui.pcaGB->show();
	if(ui.actionGrid->isChecked()){
		ui.actionPCA->setChecked(true);
		ui.actionGrid->setChecked(false);
	}
	if(ui.actionAverage_Liver->isChecked()){
		ui.actionPCA->setChecked(true);
		ui.actionAverage_Liver->setChecked(false);
	}
	scalePCA();
}



void mainWindow::calculateHistogramMaxAverageClustering()
{
	linkage clusterLinkage;
	clusterLinkage = ChebyshevFeatureCentroidLinkage;
	emit showProgressBarsSignal(clusterLinkage);
	Clustering* clustering = new Clustering();
	connect(clustering, SIGNAL(updateProgressBar(int, int)), this, SLOT(updateSBProgresses(int, int)));
	histogramMaximumAvgLinkage = clustering->hierarchicalClustering(averageLiver->segAverage, averageLiver->SegHistogramData, clusterLinkage);
	//	cout <<"Average Cluster Ready! \n";
	emit enableClusterButtonsSignal(clusterLinkage);
	emit hideProgressBarsSignal(clusterLinkage);
}

void mainWindow::calculateHistogramMaxMaxClustering()
{
	linkage clusterLinkage;
	clusterLinkage = ChebyshevFeatureCompleteLinkage;
	emit showProgressBarsSignal(clusterLinkage);
	Clustering* clustering = new Clustering();
	connect(clustering, SIGNAL(updateProgressBar(int, int)), this, SLOT(updateSBProgresses(int, int)));
	histogramMaxmimumMaximumLinkage = clustering->hierarchicalClustering(averageLiver->segAverage, averageLiver->SegHistogramData, clusterLinkage);
	//	cout <<"Average Cluster Ready! \n";
	emit enableClusterButtonsSignal(clusterLinkage);
	emit hideProgressBarsSignal(clusterLinkage);
}

void mainWindow::calculateHistogramEucMaxClustering()
{
	linkage clusterLinkage;
	clusterLinkage = EuklidFeatureCompleteLinkage;
	emit showProgressBarsSignal(clusterLinkage);
	Clustering* clustering = new Clustering();
	connect(clustering, SIGNAL(updateProgressBar(int, int)), this, SLOT(updateSBProgresses(int, int)));
	histogramEuklidMaximumLinkage = clustering->hierarchicalClustering(averageLiver->segAverage, averageLiver->SegHistogramData, clusterLinkage);
	//	cout <<"Average Cluster Ready! \n";
	emit enableClusterButtonsSignal(clusterLinkage);
	emit hideProgressBarsSignal(clusterLinkage);
}

void mainWindow::calculateHistogramEucAvgClustering()
{
	linkage clusterLinkage;
	clusterLinkage = EuklidFeatureCentroidLinkage;
	emit showProgressBarsSignal(clusterLinkage);
	Clustering* clustering = new Clustering();
	connect(clustering, SIGNAL(updateProgressBar(int, int)), this, SLOT(updateSBProgresses(int, int)));
	histogramEuklidAverageLinkage = clustering->hierarchicalClustering(averageLiver->segAverage, averageLiver->SegHistogramData, clusterLinkage);
	//	cout <<"Average Cluster Ready! \n";
	emit enableClusterViewSignal();
	emit enableClusterButtonsSignal(clusterLinkage);
	emit hideProgressBarsSignal(clusterLinkage);
}

void mainWindow::enableClusterViewBt(){
	ui.actionAverage_Liver->setEnabled(true);
	ui.ClusteringThresholdFrame->setEnabled(true);
}



void mainWindow::resetLiverSelection(){
	PCAselectedClusters->clear();
	for (int i = 0; i < renderTupels.size(); i++)
	{
		PCAselectedClusters->push_back(i);
	}
	newReducedClustering();


	//	PCAselectedClusters->clear();
	//	for (int i = 0; i < renderTupels.size(); i++)
	//	{
	//		PCAselectedClusters->push_back(i);
	//	}
	//newReducedClustering();
}
void mainWindow::showChoice(){
	std::set<int> liverNums;
	for(int i = 0; i < PCAselectedClusters->size(); i++){
		liverNums.insert(PCAselectedClusters->at(i));
	}
	showGrid(liverNums);
}


void mainWindow::exportSelection(){
	for(int i = 0; i < PCAselectedClusters->size(); i++){
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInput(renderTupels.at(PCAselectedClusters->at(i))->getSegmentationLiver()->getData());
		//	transformFilter2->SetInput(cylinderSource2->GetOutput());
		vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();
		transformFilter->SetTransform( transform2 );
		transform2->Translate(renderTupels.at(PCAselectedClusters->at(i))->getSegmentationLiver()->transX, renderTupels.at(PCAselectedClusters->at(i))->getSegmentationLiver()->transY,renderTupels.at(PCAselectedClusters->at(i))->getSegmentationLiver()->transZ);
		transform2->Update();
		transformFilter->Update();
		std::string savePath = "test";
		savePath.append(".vtk");
		vtkPolyDataWriter* writer = vtkPolyDataWriter::New();


		writer->SetFileName(savePath.c_str());

		writer->SetInput(transformFilter->GetOutput());
		writer->Update();
		if(writer->Write() == 1){
			cout << "\n File export successfull:" << savePath << "\n";
		}else{
			cout << "\n File export unsuccessfull\n";
		}
	}
}
void mainWindow::newReducedClustering(){
	ui.edml_PB->setValue(0);
	ui.mdml_PB->setValue(0);
	ui.edal_PB->setValue(0);
	ui.mdal_PB->setValue(0);
	ui.mdal_bt->setEnabled(0);
	ui.mdml_bt->setEnabled(0);
	ui.edml_Bt->setEnabled(0);
	ui.edal_bt->setEnabled(0);
	ui.mdal_PB->hide();
	ui.mdml_PB->hide();
	ui.edal_PB->hide();
	ui.edml_PB->hide();
	std::set<int> liverNums;
	for(int i = 0; i < PCAselectedClusters->size(); i++){
		liverNums.insert(PCAselectedClusters->at(i));
	}
	averageLiver = new AverageLiver(renderTupels, liverNums, true);
	cancelAllThreads();
	fsynchronizer = new QFutureSynchronizer<void>();
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucAvgClustering));
	fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramEucMaxClustering));
	//fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxMaxClustering));
	//fsynchronizer->addFuture(QtConcurrent::run(this, &mainWindow::calculateHistogramMaxAverageClustering));

}
void mainWindow::updateSBProgresses(int value, int link ){
	switch(link)
	{
	case 5:{
		ui.edml_PB->setValue(value);
		break; }

	case 6:{
		ui.mdml_PB->setValue(value);
		break; }

	case 7:{
		ui.edal_PB->setValue(value);
		break; }

	case 8:{
		ui.mdal_PB->setValue(value);
		break; 
		   }
	}
}

void mainWindow::enableClusterButtons(int link){
	switch(link)
	{
	case 5:{
		ui.edml_Bt->setEnabled(true);
		break; }

	case 6:{
		ui.mdml_bt->setEnabled(true);
		break; }

	case 7:{
		ui.edal_bt->setEnabled(true);
		break; }

	case 8:{
		ui.mdal_bt->setEnabled(true);
		break; }


	}
}

void mainWindow::changeClusterColor(int cluster, QColor col){

Node* base;
if(ui.edal_bt->isChecked())
	base = histogramEuklidAverageLinkage;

if(ui.edml_Bt->isChecked())
	base = histogramEuklidMaximumLinkage;

if(ui.mdml_bt->isChecked())
	base = histogramMaxmimumMaximumLinkage;

if(ui.mdal_bt->isChecked())
	base = histogramMaximumAvgLinkage;
std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

double temp = base->hierarchyLevel - ui.clusteringThresholdSlider->value();
Clustering::getAllClusterVector(base, temp, allClusters);
int pointIndex = allClusters->at(cluster).at(0);
temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(pointIndex);
ColorSource cs;

vtkLookupTable* lookupTable = averageLiver->clusteringLiver->getColorOpacityLookupTable(); //cs.getRainbowLookupTable((int) allClusters->size());
lookupTable->SetTableValue(temp,((double) col.red())/ 255.0, ((double) col.green())/ 255.0, ((double) col.blue())/ 255.0);
lookupTable->SetTableValue(temp - allClusters->size(),((double) col.red())/ 255.0, ((double) col.green())/ 255.0, ((double) col.blue())/ 255.0, 0.4);
averageLiver->clusteringLiver->setLookupTable(lookupTable, 0, (allClusters->size() * 2) - 1);
emit sendClusteringData(base,ui.clusteringThresholdSlider->value(), averageLiver->SegAverageDistances, PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());

if(clusterHighGui != NULL)
clusterHighGui->highlightClusters(clusterChoice);
//emit send
repaintAll();
//averageLiver->clusteringLiver.
////for(int k = 0; k < averageLiver->clusteringLiver->getData()->GetNumberOfPoints(); k++)
//vtkDoubleArray* dArray = vtkDoubleArray::New();
//std::set<int>::const_iterator itr;
//for(int k = 0; k  < averageLiver->clusteringLiver->getData()->GetNumberOfPoints(); k++){
//	bool insert = true;
//
//	for (itr = clusters->begin(); itr != clusters->end(); ++itr)
//	{
//		for (int j = 0; j < allClusters->at(*itr).size(); j++)
//		{
//			if(k == allClusters->at(*itr).at(j)){
//				insert = false;
//				break;
//			}
//
//		}
//		if(!insert)
//			break;
//	}
//	if(insert){
//		double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(k);
//		if(temp >= allClusters->size()){
//			dArray->InsertTuple1(k, temp - allClusters->size());
//		}
//		else{
//			dArray->InsertTuple1(k, temp);
//		}
//	}
//}
}


void mainWindow::highightClusters( std::set<int>* clusters )
{
	highlightedClusters = clusters;
	Node* base;
	if(ui.edal_bt->isChecked())
		base = histogramEuklidAverageLinkage;

	if(ui.edml_Bt->isChecked())
		base = histogramEuklidMaximumLinkage;

	if(ui.mdml_bt->isChecked())
		base = histogramMaxmimumMaximumLinkage;

	if(ui.mdal_bt->isChecked())
		base = histogramMaximumAvgLinkage;


	
	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

	double temp = base->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(base, temp, allClusters);
	//for(int k = 0; k < averageLiver->clusteringLiver->getData()->GetNumberOfPoints(); k++)
	vtkDoubleArray* dArray = vtkDoubleArray::New();
	std::set<int>::const_iterator itr;
	for(int k = 0; k  < averageLiver->clusteringLiver->getData()->GetNumberOfPoints(); k++){
		bool insert = true;

		for (itr = clusters->begin(); itr != clusters->end(); ++itr)
		{
			for (int j = 0; j < allClusters->at(*itr).size(); j++)
			{
				if(k == allClusters->at(*itr).at(j)){
					insert = false;
					break;
				}

			}
			if(!insert)
				break;
		}
		if(insert){
			double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(k);
			if(temp >= allClusters->size()){
				dArray->InsertTuple1(k, temp - allClusters->size());
			}
			else{
				dArray->InsertTuple1(k, temp);
			}
		}
	}
	for (itr = clusters->begin(); itr != clusters->end(); ++itr)
	{
		for (int j = 0; j < allClusters->at(*itr).size(); j++)
		{
			double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(allClusters->at(*itr).at(j));

			if(temp < allClusters->size()){
				dArray->InsertTuple1(allClusters->at(*itr).at(j), temp + allClusters->size());
			}
			else{
				dArray->InsertTuple1(allClusters->at(*itr).at(j), temp);
			}

		}
	}
	averageLiver->clusteringLiver->setScalars(dArray);

	//for (int i = 0; i < clusters->size(); i++)
	//{
	//	for (int j = 0; j < allClusters->at(clusters->at(i)).size(); j++)
	//	{
	//		double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(allClusters->at(clusters->at(i)).at(j));//) + allClusters->size())
	//			//if()
	//	/*	averageLiver->clusteringLiver->getData()->GetPointData()->GetScalars()->SetTuple1(allClusters->at(i).at(j), (averageLiver->clusteringLiver->getData()->GetPointData()->GetScalars()->GetTuple1(allClusters->at(i).at(j)) + allClusters->size()));
	//		averageLiver->clusteringLiver->getScalars()*/
	//		vtkDoubleArray* dArray = averageLiver->clusteringLiver->getScalars();
	//		dArray->SetTuple1(allClusters->at(clusters->at(i)).at(j), (dArray->GetTuple1(allClusters->at(clusters->at(i)).at(j)) + allClusters->size()));
	//		//	averageLiver->clusteringLiver->getScalars()->SetTuple1(allClusters->at(i).at(j), (averageLiver->clusteringLiver->getScalars()->GetTuple1(allClusters->at(i).at(j)) + allClusters->size()));
	//		averageLiver->clusteringLiver->setScalars(dArray);
	//		//		averageLiver->clusteringLiver->getData()->GetPointData()->GetScalars()->SetValue(1,2);
	//	}
	//}
	repaintAll();
	//
	//
	//
	//
	//	QString qStr = QString::number(allClusters->size());
	//	//	QString number( allClusters->size(), 10);
	//	ui.clusterCountLabel->setText(qStr);
	//	QPalette palette = ui.clusterCountLabel->palette();
	//	//	palette.setColor(ui->clusterCountLabel->backgroundRole(), Qt::yellow);
	//	palette.setColor(ui.clusterCountLabel->foregroundRole(), Qt::darkRed);
	//	ui.clusterCountLabel->setPalette(palette);
	//	ColorSource cs;
	//	averageLiver->clusteringLiver->setLookupTable(cs.getRainbowLookupTable(allClusters->size()), 0, allClusters->size() - 1);
	//	averageLiver->clusteringLiver->getData()->GetPointData()->SetScalars(cs.getScalars(allClusters, 0));
	//	repaintAll();
}

void mainWindow::highightClusters( int pointID, bool ctrl )
{
	Node* base;
	if(ui.edal_bt->isChecked())
		base = histogramEuklidAverageLinkage;

	if(ui.edml_Bt->isChecked())
		base = histogramEuklidMaximumLinkage;

	if(ui.mdml_bt->isChecked())
		base = histogramMaxmimumMaximumLinkage;

	if(ui.mdal_bt->isChecked())
		base = histogramMaximumAvgLinkage;



	std::vector< std::vector<int> >* allClusters = new std::vector< std::vector<int> >();

	double temp = base->hierarchyLevel - ui.clusteringThresholdSlider->value();
	Clustering::getAllClusterVector(base, temp, allClusters);
	int clusterID;

	bool breakflag = false;
	for (int i = 0; (i < allClusters->size()) && !breakflag ; i++)
	{
		for (int j = 0; j < allClusters->at(i).size(); j++)
		{
			if(allClusters->at(i).at(j) == pointID){
				clusterID = i;
				breakflag = true;

			}
		}
	}
	if(!ctrl)
		clusterChoice = new std::set<int>();

	clusterChoice->insert(clusterID);
	emit sendClusterIDs(clusterChoice);
	vtkDoubleArray* dArray = vtkDoubleArray::New();
	std::set<int>::const_iterator itr;
	for(int k = 0; k  < averageLiver->clusteringLiver->getData()->GetNumberOfPoints(); k++){
		bool insert = true;
		/*	if((*itr == -1) || (*itr > allClusters->size())){
		cout << "\n Something wrong with Point ID \n";
		} else{*/



		for (itr = clusterChoice->begin(); itr != clusterChoice->end(); ++itr)
		{
			for (int j = 0; j < allClusters->at(*itr).size(); j++)
			{
				if(k == allClusters->at(*itr).at(j)){
					insert = false;
					break;
				}

			}
			if(!insert)
				break;
		}
		//}
		if(insert){
			double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(k);
			if(temp >= allClusters->size()){
				dArray->InsertTuple1(k, temp - allClusters->size());
			}
			else{
				dArray->InsertTuple1(k, temp);
			}
		}
	}
	for (itr = clusterChoice->begin(); itr != clusterChoice->end(); ++itr)
	{
		/*if((*itr == -1) || (*itr >allClusters->size())){
		cout << "\n Something wrong with Point ID \n";
		} else{*/
		for (int j = 0; j < allClusters->at(*itr).size(); j++)
		{
			double temp = averageLiver->clusteringLiver->getScalars()->GetTuple1(allClusters->at(*itr).at(j));

			if(temp < allClusters->size()){
				dArray->InsertTuple1(allClusters->at(*itr).at(j), temp + allClusters->size());
			}
			else{
				dArray->InsertTuple1(allClusters->at(*itr).at(j), temp);
			}

			//	}
		}
	}

	averageLiver->clusteringLiver->setScalars(dArray);


	repaintAll();

}



void mainWindow::changeClusterThreshold(int newThreshold){
	ui.clusteringThresholdSlider->setValue(newThreshold);
	showClusterColor();
	//clusterHighGui->close();
	//showClusterGraph();
}
void mainWindow::showClusterGraph()
{

	clusterHighGui = new clusterHighlighting(this, averageLiver, ui.clusteringThresholdSlider->value(), PCAselectedClusters);
	QObject::connect(clusterHighGui, SIGNAL(sendThreshold(int)), this, SLOT(changeClusterThreshold(int)));
	QObject::connect(clusterHighGui, SIGNAL(sendColorChangeCluster(int, QColor)), this, SLOT(changeClusterColor(int, QColor)));
	QObject::connect(this, SIGNAL(sendClusterIDs(std::set<int>*)), clusterHighGui, SLOT(highlightClusters(std::set<int>*)));
	QObject::connect(ui.clusteringThresholdSlider, SIGNAL(sliderMoved(int)), clusterHighGui, SLOT(repaintThreshline(int)));

	//QObject::connect(ui.clusteringThresholdSlider, SIGNAL(valueChanged(int)), clusterHighGui, SLOT(repaintThreshline(int)));
	Node* base;
	if(ui.edal_bt->isChecked())
		base = histogramEuklidAverageLinkage;

	if(ui.edml_Bt->isChecked())
		base = histogramEuklidMaximumLinkage;

	if(ui.mdml_bt->isChecked())
		base = histogramMaxmimumMaximumLinkage;

	if(ui.mdal_bt->isChecked())
		base = histogramMaximumAvgLinkage;


	connect(this, SIGNAL(sendClusteringData(Node*, double, vtkDoubleArray*, std::vector<int> *, vtkLookupTable*)), clusterHighGui, SLOT(setupGraph(Node*, double, vtkDoubleArray*, std::vector<int> *, vtkLookupTable*)));
	connect(clusterHighGui, SIGNAL(chosenCluster(std::set<int>*)), this, SLOT(highightClusters(std::set<int>*)));
	clusterHighGui->show();
	emit sendClusteringData(base,ui.clusteringThresholdSlider->value(), averageLiver->SegAverageDistances, PCAselectedClusters, averageLiver->clusteringLiver->getColorOpacityLookupTable());

}


void mainWindow::showHeatmap(){
	QWidget* widget = new QWidget();
	widget->setObjectName(QString::fromUtf8("Heatmap"));
	widget->setMinimumHeight(500);
	widget->setMinimumWidth(500);
	//TODO BACK IN IF NEEDED
	//QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	//sizePolicy1.setHorizontalStretch(0);
	//sizePolicy1.setVerticalStretch(0);
	//sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
	//widget->setSizePolicy(sizePolicy1);

	QWidget* frame = new QWidget();
	QGridLayout* layout = new QGridLayout();
	layout->setMargin(1);
	frame->setLayout(layout);

	widget->setLayout(layout);
	for(int x = 0; x < 10 ; x++){
		for(int y = 0; y < 10 ; y++){

			QWidget* wid = new QWidget();
			//QGraphicsView* gv = new QGraphicsView(wid);
			//QGraphicsTextItem* ti = new QGraphicsTextItem(gv);
			//ti->setPlainText("test");
			//QLabel* label = new QLabel(wid);
			//label->setText("Test");
			//label->adjustSize();
			//label->setScaledContents(true);
			QPalette Pal(palette());
			Pal.setColor(QPalette::Background, QColor (((double) x/10.0)*255.0 , ((double) y/10.0)*255.0, 0));
			wid->setAutoFillBackground(true);
			wid->setPalette(Pal);

			/*	QGraphicsView* gv = new QGraphicsView(wid);
			QGraphicsScene* gs = new QGraphicsScene();	
			gs->setSceneRect(0,0,10,10);
			gs->setBackgroundBrush(QBrush(Qt::red, Qt::SolidPattern));
			gv->setScene(gs);*/
			layout->addWidget(wid,x,y);
		}
	}
	widget->update();
	widget->show();
}

void mainWindow::showProgressBars(int link)
{
	switch(link)
	{
	case 5:{
		ui.edml_PB->show();
		break; }

	case 6:{
		ui.mdml_PB->show();
		break; }

	case 7:{
		ui.edal_PB->show();
		break; }

	case 8:{
		ui.mdal_PB->show();
		break; }


	}

}

void mainWindow::hideProgressBars(int link)
{
	switch(link)
	{
	case 5:{
		ui.edml_PB->hide();
		break; }

	case 6:{
		ui.mdml_PB->hide();
		break; }

	case 7:{
		ui.edal_PB->hide();
		break; }

	case 8:{
		ui.mdal_PB->hide();
		break; }
	}

}

void mainWindow::enablePCAButton()
{
	ui.actionPCA->setEnabled(true);
}

void mainWindow::cancelAllThreads()
{
	for(int i = 0; i < futureList->size(); i++){
		//	futureList->at(i).cancel();

	}
	//futureList->clear();

}

void mainWindow::changeClusteringThresholdScale( int i)
{
	ui.clusteringThresholdSlider->setMaximum(i);
}
