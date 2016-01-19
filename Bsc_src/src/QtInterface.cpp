// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "QtInterface.h"
#include <iosfwd>
//#include <crtdbg.h>
//#include <assert.h>

#define TOPLIVERCOUNT 5
#define BOTLIVERCOUNT 3
#define INSIDECOUNT 3
#define OUTSIDECOUNT 2
#define RADIUS 5
#define TRANSLATION 7

RenderTupel* QtInterface::loadSingleLiver(std::string mhd ,std::string vtk, float allLivers, int LiverNum, std::pair<bool, bool> TotalOrGettoDistances)
{
	std::string mhdSuffix = mhd.substr(mhd.size() - 3, mhd.size());
	std::string vtkSuffix = vtk.substr(vtk.size() - 3, vtk.size());
	vtkPolyData * mhdDataTmp;
	if(mhdSuffix == "vtk"){
	mhdDataTmp = LiverDataLoader::loadVtk(mhd);
	}else if(mhdSuffix == "off"){
	mhdDataTmp = LiverDataLoader::loadOFF(mhd);
	}else if(mhdSuffix == "mhd"){
		vtkSmartPointer<vtkMetaImageReader>  metaReader = vtkSmartPointer<vtkMetaImageReader>::New();
		metaReader->SetFileName(mhd.c_str());
		metaReader->Update();
		mhdDataTmp = LiverDataLoader::loadMhdMarchingCubes(metaReader->GetOutput(),false);
	}
	else{
		cout <<"NO ACCEPTABLE FORMAT!";
	}
	//Translate to Center
	double * transformation = mhdDataTmp->GetCenter();
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInput(mhdDataTmp);
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transformFilter->SetTransform( transform );

	transform->Translate(-transformation[0], -transformation[1], -transformation[2]);
	transform->Update();
	transformFilter->Update();

	Liver * cubesLiver = new Liver(transformFilter->GetOutput());
	//cubesLiver->getActor()->GetProperty()->SetColor(0.5,0,0);
	//cubesLiver->getActor()->GetProperty()->SetColor(0.9,0.9,0.9);
	//cubesLiver->getActor()->GetProperty()->SetColor(0.45,0.1,0.1);
	//cubesLiver->getActor()->GetProperty()->SetInterpolationToGouraud();
	//cubesLiver->getActor()->GetProperty()->SetColor(0.5,0,0);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

	vtkPolyData * vtkDataTmp;
	//vtkPolyData * vtkDataTmp = sphereSource2->GetOutput();
	if(vtkSuffix == "vtk"){
		vtkDataTmp = LiverDataLoader::loadVtk(vtk);
	}else if(vtkSuffix == "off"){
		vtkDataTmp = LiverDataLoader::loadOFF(vtk);
	}
	else{
		cout <<"NO ACCEPTABLE FORMAT!";
	}
	transformFilter2->SetInput(vtkDataTmp);
	transformation = vtkDataTmp->GetCenter();
//	transformFilter2->SetInput(cylinderSource2->GetOutput());
	vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();
	transformFilter2->SetTransform( transform2 );
	transform2->Translate(-transformation[0], -transformation[1], -transformation[2]);
	transform2->Update();
	transformFilter2->Update();

	Liver * vtkLiver = new Liver(transformFilter2->GetOutput());
	vtkLiver->transX = transformation[0];
	vtkLiver->transY = transformation[1];
	vtkLiver->transZ = transformation[2];
	//vtkLiver->getActor()->GetProperty()->SetColor(0.0,0.5,0);
	//vtkLiver->getActor()->GetProperty()->SetColor(0.45,0.1,0.1);
	//vtkLiver->getActor()->GetProperty()->SetInterpolationToGouraud();

	RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), LiverNum);
	disExpert2Seg->calculate(TotalOrGettoDistances.first,TotalOrGettoDistances.second);
	RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), LiverNum);
	disSeg2Expert->calculate(TotalOrGettoDistances.first, TotalOrGettoDistances.second);

	ColorSource cs1;
	cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
	cubesLiver->setScalarVisibility(true);
	cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
	ColorSource cs2;
	vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
	vtkLiver->setScalarVisibility(true);
	vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


	RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
	emit progressUpdate(allLivers);

	

	return tupel;
	
}

void QtInterface::convertSingleMHD(std::string mhd , float allLivers, int LiverNum, std::pair<bool, bool> decimateWithDistances)
{

	vtkSmartPointer<vtkMetaImageReader>  metaReader = vtkSmartPointer<vtkMetaImageReader>::New();
		//	metaReader->Update();
			metaReader->SetFileName(mhd.c_str());
		//	metaReader->BreakOnError();
	   	 	metaReader->Update();
	vtkPolyData * mhdDataTmp = LiverDataLoader::loadMhdMarchingCubes(metaReader->GetOutput(),decimateWithDistances.first);
	std::string savePath = mhd.substr(0,mhd.size() - 3);
	savePath.append("vtk");
	vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
	
	
	writer->SetFileName(savePath.c_str());
	
	writer->SetInput(mhdDataTmp);
	writer->Update();
	if(writer->Write() == 1){
		cout << "\n File export successfull:" << savePath << "\n";
	}else{
		cout << "\n File export unsuccessfull\n";
	}
	
	emit progressUpdate(allLivers);


}

std::vector<RenderTupel*> QtInterface::loadSingleSyntheticDataTube()
{

std::vector<RenderTupel*> renderVec;
for(int i = 1; i <= 2; i++){
vtkOBJReader * reader = vtkOBJReader::New();
reader->SetFileName("E:/Projects/newBuild/Plane2/base.obj");
//reader->SetFileName("E:/Projects/newBuild/Plane5/base.obj");
reader->Update();

//vtkPolyData * copy = vtkPolyData::New();

vtkPolyData * vtkDataTmp = reader->GetOutput();
vtkPolyData * copy1 = vtkPolyData::New();
copy1->DeepCopy(vtkDataTmp);

std::ostringstream convert;   // stream used for the conversion

convert << i; 
std::string pathTo;
pathTo.append("E:/Projects/newBuild/Plane2/");
//pathTo.append("E:/Projects/newBuild/Plane5/");
pathTo.append(convert.str());
pathTo.append(".obj");
vtkOBJReader * reader2 = vtkOBJReader::New();
reader2->SetFileName(pathTo.c_str());
reader2->Update();
vtkPolyData * vtkDataTmp2 = reader2->GetOutput();
vtkPolyData * copy2 = vtkPolyData::New();
copy2->DeepCopy(vtkDataTmp2);

Liver * vtkLiver = new Liver(copy2);

if(i == 1){
vtkLiver->getActor()->GetProperty()->SetColor(0.0,1,0.0);
}else{
	vtkLiver->getActor()->GetProperty()->SetColor(1,0,0.0);
}

Liver * cubesLiver = new Liver(copy1);
cubesLiver->getActor()->GetProperty()->SetColor(1,0,0.0);
//cubesLiver->getActor()->GetProperty()->SetColor(0,0,1);
cout << "\t Pointnumber: " << vtkLiver->getData()->GetNumberOfPoints() << "\n";
RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), i);
disExpert2Seg->calculate(true, false);
RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), i);
disSeg2Expert->calculate(true,false);

ColorSource cs1;
cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
cubesLiver->setScalarVisibility(true);
cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
ColorSource cs2;
vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
vtkLiver->setScalarVisibility(true);
vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
renderVec.push_back(tupel);
}
return renderVec;
}

std::vector<RenderTupel*> QtInterface::loadSingleSyntheticDataSphere()
{

	std::vector<RenderTupel*> renderVec;
	for(int i = 1; i <= 20; i++){
		vtkOBJReader * reader = vtkOBJReader::New();
		reader->SetFileName("E:/Projects/newBuild/Sphere2/base.obj");
		reader->Update();

		//vtkPolyData * copy = vtkPolyData::New();

		vtkPolyData * vtkDataTmp = reader->GetOutput();
		vtkPolyData * copy1 = vtkPolyData::New();
		copy1->DeepCopy(vtkDataTmp);
		
		std::ostringstream convert;   // stream used for the conversion

		convert << i; 
		std::string pathTo;
		pathTo.append("E:/Projects/newBuild/Sphere2/");
		pathTo.append(convert.str());
		pathTo.append(".obj");
		vtkOBJReader * reader2 = vtkOBJReader::New();
		reader2->SetFileName(pathTo.c_str());
		reader2->Update();
		vtkPolyData * vtkDataTmp2 = reader2->GetOutput();
		vtkPolyData * copy2 = vtkPolyData::New();
		copy2->DeepCopy(vtkDataTmp2);

		Liver * vtkLiver = new Liver(copy2);
		
		cout << "\t Pointnumber: " << vtkLiver->getData()->GetNumberOfPoints() << "\n";
		Liver * cubesLiver = new Liver(copy1);
		vtkLiver->getActor()->GetProperty()->SetColor(0.5,0,0);
		cubesLiver->getActor()->GetProperty()->SetColor(0.0,0.5,0);
		RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), i);
		disExpert2Seg->calculate(true,false);
		RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), i);
		disSeg2Expert->calculate(true,false);
  
		ColorSource cs1;
		cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
		cubesLiver->setScalarVisibility(true);
		cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
		ColorSource cs2;
		vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
		vtkLiver->setScalarVisibility(true);
		vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


		RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
		renderVec.push_back(tupel);
	}
	return renderVec;
}

std::vector<RenderTupel*> QtInterface::loadSingleSyntheticData2()
{
	std::vector<RenderTupel*> renderVec;
	int counter = 0;
	// Create a circle
	for(int i = 0; i < TOPLIVERCOUNT; i++){
		counter++;
		double random = rand() % 100; 
		double trans = ((double) TRANSLATION / 100.0) * random; 
		vtkSmartPointer<vtkSphereSource> sphereSource = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(5.0);
		sphereSource->Update();
		cout <<"         " <<sphereSource->GetOutput()->GetNumberOfPoints() <<"         ";


		vtkPolyData * mhdDataTmp = sphereSource->GetOutput();


		Liver * cubesLiver = new Liver(mhdDataTmp);



		vtkSmartPointer<vtkSphereSource> sphereSource2 = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource2->SetCenter(TRANSLATION, TRANSLATION, 0.0);
		sphereSource2->SetRadius(5.0);
		sphereSource2->Update();
		vtkPolyData * vtkDataTmp = sphereSource2->GetOutput();



		Liver * vtkLiver = new Liver(vtkDataTmp);

		RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), counter);
		disExpert2Seg->calculate(true,false);
		RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), counter);
		disSeg2Expert->calculate(true,false);

		ColorSource cs1;
		cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
		cubesLiver->setScalarVisibility(true);
		cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
		ColorSource cs2;
		vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
		vtkLiver->setScalarVisibility(true);
		vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


		RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
		renderVec.push_back(tupel);
	}

	for(int i = 0; i < BOTLIVERCOUNT; i++){
		counter++;
		double random = rand() % 100; 
		double trans = ((double) TRANSLATION / 100.0) * random; 
		vtkSmartPointer<vtkSphereSource> sphereSource = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(5.0);
		sphereSource->Update();


		vtkPolyData * mhdDataTmp = sphereSource->GetOutput();


		Liver * cubesLiver = new Liver(mhdDataTmp);



		vtkSmartPointer<vtkSphereSource> sphereSource2 = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource2->SetCenter(-TRANSLATION, -TRANSLATION, 0.0);
		sphereSource2->SetRadius(5.0);
		sphereSource2->Update();
		vtkPolyData * vtkDataTmp = sphereSource2->GetOutput();



		Liver * vtkLiver = new Liver(vtkDataTmp);

		RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), counter);
		disExpert2Seg->calculate(true,false);
		RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), counter);
		disSeg2Expert->calculate(true,false);

		ColorSource cs1;
		cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
		cubesLiver->setScalarVisibility(true);
		cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
		ColorSource cs2;
		vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
		vtkLiver->setScalarVisibility(true);
		vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


		RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
		renderVec.push_back(tupel);
	}

	for(int i = 0; i < INSIDECOUNT; i++){
		double random = rand() % 100; 
		counter++;
		double trans = ((double) TRANSLATION / 100.0) * random; 
		vtkSmartPointer<vtkSphereSource> sphereSource = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(5.0);
		sphereSource->Update();


		vtkPolyData * mhdDataTmp = sphereSource->GetOutput();


		Liver * cubesLiver = new Liver(mhdDataTmp);



		vtkSmartPointer<vtkSphereSource> sphereSource2 = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource2->SetCenter(0.0, 0.0, 0.0);
		sphereSource2->SetRadius(trans);
		sphereSource2->Update();
		vtkPolyData * vtkDataTmp = sphereSource2->GetOutput();



		Liver * vtkLiver = new Liver(vtkDataTmp);

		RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), counter);
		disExpert2Seg->calculate(true,false);
		RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), counter);
		disSeg2Expert->calculate(true,false);

		ColorSource cs1;
		cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
		cubesLiver->setScalarVisibility(true);
		cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
		ColorSource cs2;
		vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
		vtkLiver->setScalarVisibility(true);
		vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


		RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
		renderVec.push_back(tupel);
	}

	for(int i = 0; i < OUTSIDECOUNT; i++){
		counter++;
		double random = rand() % 100; 
		double trans = ((double) TRANSLATION / 100.0) * random; 
		vtkSmartPointer<vtkSphereSource> sphereSource = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(5.0);
		sphereSource->Update();

		vtkPolyData * mhdDataTmp = sphereSource->GetOutput();


		Liver * cubesLiver = new Liver(mhdDataTmp);



		vtkSmartPointer<vtkSphereSource> sphereSource2 = 
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource2->SetCenter(trans, 0.0, 0.0);
		sphereSource2->SetRadius(5.0);
		sphereSource2->Update();
		vtkPolyData * vtkDataTmp = sphereSource2->GetOutput();



		Liver * vtkLiver = new Liver(vtkDataTmp);

		RelativeDistance * disExpert2Seg = new RelativeDistance(cubesLiver->getData(), vtkLiver->getData(), counter);
		disExpert2Seg->calculate(true, false);
		RelativeDistance * disSeg2Expert = new RelativeDistance(vtkLiver->getData(), cubesLiver->getData(), counter);
		disSeg2Expert->calculate(true,false);

		ColorSource cs1;
		cubesLiver->setLookupTable(cs1.getLookupTable(), cs1.getScalarFrom(), cs1.getScalarTo());
		cubesLiver->setScalarVisibility(true);
		cubesLiver->setScalars(cs1.getScalars(disExpert2Seg));
		ColorSource cs2;
		vtkLiver->setLookupTable(cs2.getLookupTable(), cs2.getScalarFrom(), cs2.getScalarTo());
		vtkLiver->setScalarVisibility(true);
		vtkLiver->setScalars(cs2.getScalars(disSeg2Expert));


		RenderTupel * tupel = new RenderTupel(cubesLiver, vtkLiver, disExpert2Seg, disSeg2Expert);
		renderVec.push_back(tupel);
	}




	return renderVec;
}
std::vector<RenderTupel*> QtInterface::loadLivers(QStringList listMhd, QStringList listVtk, bool decimate, bool withDistances){
	std::vector<RenderTupel*> vec;

	QFutureSynchronizer<RenderTupel*> * synchronizer = new QFutureSynchronizer<RenderTupel*>();
//	synchronizer->cancelOnWait();
	for (int i=0; i < listMhd.size(); i++) {
		std::string mhd = listMhd.at(i).toAscii().data();
		std::string vtk = listVtk.at(i).toAscii().data();

		synchronizer->addFuture(QtConcurrent::run<RenderTupel*>(this, &QtInterface::loadSingleLiver, mhd, vtk, listMhd.size(), i+1, std::pair<bool, bool>(decimate,withDistances)));

			synchronizer->waitForFinished();
	}		

	for (int i=0; i < listMhd.size(); i++) {
		vec.push_back(synchronizer->futures().at(i).result());
		//	vec.push_back(loadSingleLiver(mhd, vtk, decimate, i+1 , listMhd.size()));
	}
	return vec;	
}

void QtInterface::convertMHDs( QStringList listMhd, bool decimate, bool withDistances )
{
	QFutureSynchronizer<RenderTupel*> * synchronizer = new QFutureSynchronizer<RenderTupel*>();
	//	synchronizer->cancelOnWait();
	for (int i=0; i < listMhd.size(); i++) {
		std::string mhd = listMhd.at(i).toAscii().data();
		convertSingleMHD(mhd,listMhd.size(),i+1,std::pair<bool, bool>(decimate,withDistances) );
	//	synchronizer->addFuture(QtConcurrent::run<void>(this, &QtInterface::convertSingleMHD, mhd, listMhd.size(), i+1, std::pair<bool, bool>(decimate,withDistances)));

	//	synchronizer->waitForFinished();
	}		

}
