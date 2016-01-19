// Vamco:
// Author: Dennis Basgier
// 20.01.2013
#include "PCA.h"

void PCA::calculatePCA( std::vector<RenderTupel*>* tupels)
{

	int numP = tupels->back()->getSegmentationLiver()->getData()->GetNumberOfPoints();
	vtkSmartPointer<vtkPCAStatistics> pcaStatistics = vtkSmartPointer<vtkPCAStatistics>::New();
	vtkSmartPointer<vtkTable> dataTable = vtkSmartPointer<vtkTable>::New();
	std::vector<vtkSmartPointer<vtkDoubleArray> >* dA = new std::vector<vtkSmartPointer<vtkDoubleArray> >(numP);

	//Some meshes start with point ID 1 and some with 0
	for(int i = 0; i < numP; i++){
		dA->at(i) = vtkSmartPointer<vtkDoubleArray>::New();
	}

	std::ofstream ofs ("Input.txt", std::ofstream::out);

	
		for(int j = 0; j < tupels->size(); j++){
			for(int i = 0; i < numP; i++){
			double aa = tupels->at(j)->getDistanceSeg2Expert()->getDistanceArray()->GetValue(i);
			dA->at(i)->InsertNextValue(aa);
			ofs << aa << "\t";
		}
		ofs << "\n";
	}

	
	for(int i = 0; i < numP; i++){
		char buffer [33];
		_itoa_s(i,buffer,10);

		dA->at(i)->SetName(buffer);
		dA->at(i)->SetNumberOfComponents(1);

		for(int j = 0; j < tupels->size(); j++){
			double aa = tupels->at(j)->getDistanceSeg2Expert()->getDistanceArray()->GetValue(i);
			dA->at(i)->InsertNextValue(aa);
		//	ofs << aa << "\t";
		}

		//ofs << "\n";
		dataTable->AddColumn(dA->at(i));
	}
	ofs.close();
	pcaStatistics->SetInput(vtkStatisticsAlgorithm::INPUT_DATA,dataTable);
	for(int i = 0; i < numP; i++){
		char buffer [33];
		_itoa_s(i,buffer,10);
		pcaStatistics->SetColumnStatus( buffer, 1 );
	}


	pcaStatistics->RequestSelectedColumns();
	pcaStatistics->SetDeriveOption(true);
	time_t start = time(0);

	cout <<" \n STARTING CALCULATION OF EIGENVALUES! \n";
	pcaStatistics->Update();
	double seconds_since_start = difftime( time(0), start);
	cout <<" \n EIGENVALUES CALCULATED IN: " << seconds_since_start << " seconds \n" ;


	///////// Eigenvalues ////////////
	vtkSmartPointer<vtkDoubleArray> eigenvalues =
		vtkSmartPointer<vtkDoubleArray>::New();

	pcaStatistics->GetEigenvalues(eigenvalues);
	for(vtkIdType i = 0; i < 2; i++)
	{
		std::cout << "Eigenvalue " << i << " = " << eigenvalues->GetValue(i) << std::endl;
	}

	///////// Eigenvectors ////////////
	vtkSmartPointer<vtkDoubleArray> eigenvectors =
		vtkSmartPointer<vtkDoubleArray>::New();

	pcaStatistics->GetEigenvectors(eigenvectors);

	vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast( pcaStatistics->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
	vtkSmartPointer<vtkTable> dataset2Table = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 1) );
	std::vector<double>* newCoords = new std::vector<double>();

	double* evecX = new double[eigenvectors->GetNumberOfComponents()];
	eigenvectors->GetTuple(0, evecX);

	double* evecY = new double[eigenvectors->GetNumberOfComponents()];
	eigenvectors->GetTuple(1, evecY);
	std::ofstream ofs2 ("Output.txt", std::ofstream::out);

	for(int k = 0; k < tupels->size(); k++){
		double x = 0;
		double y = 0;
		double z = 0;
		for(vtkIdType j = 0; j < numP; j++)
		{
			int ewSize = eigenvectors->GetNumberOfComponents();
			int dataSize = (int) dA->size();
			double a = dA->at(j)->GetValue(k);
			double mean = dataset2Table->GetValue(j,1).ToDouble();
			double meanCorrected = a - dataset2Table->GetValue(j,1).ToDouble();


			x = x + evecX[j] * meanCorrected;
			y = y + evecY[j] * meanCorrected;


		}

		ofs2 << x << "\t" << y;
		ofs2 <<  "\n";
		tupels->at(k)->pcaX = x;
		tupels->at(k)->pcaY = y;

	}
	ofs2.close();


}