// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#ifndef _QT_INTERFACE_H_
#define _QT_INTERFACE_H_

#include <QStringList.h>
#include <Qt>
#include <QFuture>
#include <QtGui>
#include <vtkPolyData.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataWriter.h>
#include <sstream>
//#include <vector>
#include <string>
#include "LiverDataLoader.h"
#include "Liver.h"
#include "ColorSource.h"
#include "RenderTupel.h"
#include <vtkCylinderSource.h>
#include <vtkOBJReader.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <time.h>
#include <math.h>

class QtInterface: public QObject {
	Q_OBJECT

	public:
		 std::vector<RenderTupel*> loadLivers(QStringList listMhd, QStringList listVtk, bool decimate,  bool withDistances);
		RenderTupel* loadSingleLiver(std::string mhd ,std::string vtk , float allLivers, int LiverNum, std::pair<bool, bool> decimateWithDistances);
		std::vector<RenderTupel*> loadSingleSyntheticDataTube();
		std::vector<RenderTupel*> loadSingleSyntheticData2();
		std::vector<RenderTupel*> loadSingleSyntheticDataSphere();
		void convertMHDs(QStringList listMhd, bool decimate,  bool withDistances);
		void convertSingleMHD(std::string mhd , float allLivers, int LiverNum, std::pair<bool, bool> decimateWithDistances);
	//	int allLivers;


	signals:
	void progressUpdate(int i);
	



};
#endif