// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef clusterGraph_H
#define clusterGraph_H

//#include "ui_clusterHighlightning.h"
#include <Qt>
#include <QtGui>
#include <QVTKWidget.h>
#include <vtkDoubleArray.h>
#include "ClusterScene.h"


#define FRAMEWIDTH 620
#define FRAMEHEIGHT 260
//#define VIEWWIDTH 600
//#define VIEWHEIGHT 200
#define VIEWWIDTH 906 //800
#define VIEWHEIGHT 360
#define CLUSTERGRAPHHEIGHT 200
#define OFFSET 20
#define RADIUS 5
#define HEATMAPHEIGHT 500
#define HEATMAPWIDTH 500
#define TEXTOFFSET 15
#define BLOCKSIZE 15
#define BLOCKHEIGHT 40

struct  graphStruct
{
	double x;
	double y;
	double rgb[3];
};

class ClusterGraph : public QGraphicsView
{
	Q_OBJECT

public:
	Qt::ItemSelectionMode rubberBandSelectionMode;
	ClusterGraph(QWidget *parent = 0);
	void updateRect();
	void updateHisto();
	void calculateHeatmap(QPoint pos );
	QPen axisPen;

	QGraphicsScene* gScene;
	QRectF coord;
	QRectF frontRect;
	QRectF rearRect;
	QGraphicsRectItem* frontRI;
	QGraphicsRectItem* rearRI;
	double win;
	int pos;
	int scale;
	bool pressed;
	QCursor cursor;
	int vectX;
	int ForR;
	std::vector<graphStruct>* coordinations;
	std::vector<QGraphicsEllipseItem*>* ellipses; 
	QWidget* heatmap;
	QRubberBand* rubberBand ;
	QPoint origin;
	QPoint released;
	QList<QGraphicsItem*> selectedList;
	//bool processing;
	std::vector< std::vector<int> >* allClusters;
	vtkDoubleArray* distances;
	double maxSize;


public slots:
	void paintGraph(std::vector<graphStruct>* coordsVec);
	void markSelected();
signals:
	void rectChangedHand(int leftPos, int rightPos);
	void rectChangedSlider(float leftPos, float rightPos);
	void shaderDataUpdate(float l,float r);
	void sendClusterID(int cluster);

protected:
	void mouseReleaseEvent ( QMouseEvent * event );
	void mousePressEvent ( QMouseEvent * event );
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent ( QMouseEvent * event );
	void showHeatmap(QPoint pos);
	void contextMenuEvent(QContextMenuEvent *event);
};

class listViewCustom : public QListWidget
{
	Q_OBJECT
public:
	listViewCustom(QWidget *parent = 0);

	signals:
	void sendSortedVector(std::vector<int> sortedVector);

protected:
		void dropEvent ( QDropEvent * e ); 

};

#endif