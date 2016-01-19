// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef mainWindow_H
#define mainWindow_H

#include <Qt>
#include <QtGui>
#include <QVTKWidget.h>
#include <vector>
#include <vtkCamera.h>
#include <vtkVector.h>
#include <vtkLabeledDataMapper.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkQtLabelRenderStrategy.h>
#include <vtkActor2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor3D.h>
#include <vtkStringArray.h>
#include <vtkCallbackCommand.h>
#include "MouseInteractorStyle2.h"
#include "ui_mainWindow.h"
#include <vtkAreaPicker.h>
#include <vtkTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkProp3DCollection.h>
#include <vtkCallbackCommand.h>
#include <vtkActor.h>
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleFlight.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkPCAStatistics.h>
#include <vtkAxesActor.h>
#include <vtkMultiCorrelativeStatistics.h>
#include "QtInterface.h"
#include "HighlightInteractor.h"
#include "RenderTupel.h"
#include "ColorSource.h"
#include "PCA.h"
#include "MoveAGlyph.h"
#include "AverageLiver.h"
#include "Clustering.h"
#include "ClusterHighlighting.h"

class mainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		//viewmode: 1 is grid view, 2 PCA and 3 Clustering
		int viewmode;
		//for multithreading
		QFutureSynchronizer<void>* fsynchronizer;
		QList<QFuture<void> >* futureList;
		std::vector<int>* PCAselectedClusters;
		std::vector<QVTKWidget*> widgets;
		std::vector<std::pair<vtkSmartPointer<vtkActor>,vtkSmartPointer<vtkActor> > >actors;
	//	std::vector<std::pair<vtkSmartPointer<vtkActor2D>,vtkSmartPointer<vtkActor2D> > >actors;
		std::vector<std::pair<vtkSmartPointer<vtkPolyData>,vtkSmartPointer<vtkPolyData> > > dataInputs;
		AverageLiver* averageLiver;
		AverageLiver* averageBaseLiver;
		clusterHighlighting* clusterHighGui;
		std::set<int>* highlightedClusters;
		bool sideBarActivated;
		std::vector<RenderTupel*> renderTupels;
		double maxdistSeg;
		double maxdistExp;
		double mindistSeg;
		double mindistExp;
		std::vector<vtkSmartPointer<vtkDoubleArray> > localScalarsSeg;
		std::vector<vtkSmartPointer<vtkDoubleArray> > globalScalarsSeg;
		std::vector<vtkSmartPointer<vtkDoubleArray> > localScalarsExp;
		std::vector<vtkSmartPointer<vtkDoubleArray> > globalScalarsExp;
		std::vector<vtkSmartPointer<vtkDoubleArray> > globalQuartileScalarsExp;
		std::vector<vtkSmartPointer<vtkDoubleArray> > globalQuartileScalarsSeg;
		int columns;
		Node* fastTestCluster;
		Node* averageCluster;
		Node* maximumCluster;
		Node* minimumCluster;
		Node* histogramEuklidMaximumLinkage;
		Node* histogramEuklidAverageLinkage;
		Node* histogramMaxmimumMaximumLinkage;
		Node* histogramMaximumAvgLinkage;
		std::set<int>* clusterChoice;
		vtkSmartPointer<vtkTextActor> PCAtextActor;
		//QWidget* frame;
		//QGridLayout* layout;
		
		mainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
		~mainWindow();
		void createGrid(int max);
		void setWidgets(std::vector<QVTKWidget*> vec);
		void setRenderTupels(std::vector<RenderTupel*> tupels);
		void createWidgets();
		void createWidgets(std::set<int> choice);
		void repaintAll();
		void setupScalars();
		std::vector<QVTKWidget*> testWindows(int a);
		void prepareLivers(QStringList listMhd, QStringList listVtk, bool decimate, bool withDistances);
		void createAverageWidget();
		void calculateHistogramEucAvgClustering();
		void calculateHistogramEucMaxClustering();
		void calculateHistogramMaxMaxClustering();
		void calculateHistogramMaxAverageClustering();
		//--- log system -----------------------------


		//---------------------------------------------
	signals:
		void prepareForRenderingSignal(int columns);
		void sendClusteringData(Node* cluster, double thresh, vtkDoubleArray* distances, std::vector<int> *, vtkLookupTable* lookupTable);
		void showProgressBarsSignal(int link);
		void hideProgressBarsSignal(int link);
		void enableClusterButtonsSignal(int link);
		void enablePCAButtonSignal();
		void sendClusterIDs(std::set<int>* clusterChoice);
		void enableClusterViewSignal();

	public slots:

		//Syncs the Camera of all RenderWidgets
		void cameraSync();
		//Unsyncs cameras of all RenderWidgets
		void unSyncCamera();
		//Opens 2 File Dialogs, to select expert and segmentation Livers
		//the Number of Files must be similar between those 2 
		void openFileDialog();
		//change window Interactor to normal View Mode
		void changeViewMode();
		//Test: Change interactor to single point selection
		void changeSelectMode();
		//Test: Change interactor to Rectangular selection (for Rectangular selection press "r" in an active Window
		void changeRectangularMode();
		//Set Side Bar visible
		void showSideBar();
		//returns if Sidebar is visible
		void isSideBarVisible();
		//Test function to insert create simple Test Windows
		void insertTestWindows();
		//Show or hide Liver 1 (Expert Liver)
		void changeVisibilityActor1();
		//Show or hide Liver 2 (Segmentation Liver)
		void changeVisibilityActor2();
		void changeOpacityActor1();
		void changeOpacityActor2();
		void toggleSideBarActive();
		void activateSideBar();
		void resetCamera();
		void changeExpertScalars();
		void changeSegScalars();
		void changeAvgScalars();
		void showExpertColor();
		void showSegColor();
		void showClusterColor();
		void changeScalarsToLocal();
		void changeScalarsToGlobal();
		void changeScalarsToGlobalQuartiles();
		void changeBackgroundToGrey();
		void changeBackgroundToBlack();
		void changeBackgroundToWhite();
		void changeBackgroundColor();
		void setBigBarProgress(int i);
		void setSmallBarProgress(float i);
		void prepareForRendering(int columns);
		void createPCAWidgets();
		void scalePCA();
		void renderGrid();
		void renderGrid(std::set<int> choice);
		void showGrid();
		void showGrid(std::set<int> choice);
		void showPCA();
		void showAverage();

		void changeToAverageGui(); 


		void renderAverageGrid();
		void createWidgetsClustering();
		//calculate AverageLiver for Multithreading
		void calculateAverage();
		//calculate PCA for Multithreading
		void calculatePCA();
		void changeAvgLiverToHisMaxAvgClustering();
		void changeAvgLiverToHisMaxMaxClustering();
		void changeAvgLiverToHisEukMaxClustering();
		void changeAvgLiverToHisEukAverageClustering();
		void highightClusters(std::set<int>* clusters);
		void highightClusters( int pointID, bool ctrl );
		void showClusterGraph();
		void updateSBProgresses(int value, int link );
		void showHeatmap();
		void showProgressBars(int link);
		void hideProgressBars(int link);
		void enableClusterButtons(int link);
		void changeClusterThreshold(int newThreshold);
		void startSyntheticDataTube();
		void startSyntheticDataSphere();
		void newReducedClustering();
		void resetLiverSelection();
		void createAverageGrid();
		void enablePCAButton();
		void showChoice();
		void cancelAllThreads();
		void openFileDialogForConversion();
		void changeClusteringThresholdScale(int);
		void exportSelection();
		void enableClusterViewBt();
		void toggleGridWidget(QTreeWidgetItem* item, int column);
		void checkAll();
		void uncheckAll();
		void changeClusterColor(int cluster, QColor col);
		void changePCAlabelState(int state);
		//	void PickCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData));
private:
		Ui::MainWindow ui;
};

#endif // MSCAN_H
