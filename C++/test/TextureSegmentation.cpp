///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////	COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cfloat>

using namespace std;

#include "FileIO.h"
#include "FilterApplication.h"
#include "ImageAnalysis.h"
#include "FeatureReduction.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 10) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureSegmentation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <window size> <use Feature Reduction> <input_filename.raw> <output_filename.raw>\n";
		cout << "Laws' filter size  : 3 | 5\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n";
		cout << "Feature Reduction: -fr to use| -wfr to not use Feature Reductionn \n\n";
		return 0;
	}
	
	int numberOfClusters = stoi(argv[1]);

	int kernelSize = stoi(argv[2]);
	if(kernelSize == 3 || kernelSize == 5){
		cout << "\nSelected Law's filter size = " << kernelSize << endl;
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <input_filename.raw> <output_filename.raw>\n";
		cout << "Laws' filter size  : 3 | 5\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n\n";
	}

	bool balance;
	string balanceInput = argv[3];
	if(balanceInput.compare("-b")==0)
		balance = true;
	else if(balanceInput.compare("-wb")==0){
		balance = false;
		cout << "No balance\n\n";
	}
	else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <input_filename.raw> <output_filename.raw>\n";
		cout << "Laws' filter size  : 3 | 5\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n\n";
	}

	bool normalize;
	string normalizeInput = argv[4];
	if(normalizeInput.compare("-n")==0)
		normalize = true;
	else if(normalizeInput.compare("-wn")==0)
		normalize = false;
	else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <input_filename.raw> <output_filename.raw>\n";
		cout << "Laws' filter size  : 3 | 5\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n\n";
	}

	int method = stoi(argv[5]);
	if(method != 1 && method != 2 && method != 3){
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <Number of clusters> <Laws' filter size> <Balance> <Normalize> <Initializer> <input_filename.raw> <output_filename.raw>\n";
		cout << "Laws' filter size  : 3 | 5\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n\n";
	}

	int windowSize = stoi(argv[6]);

	bool featureReduction;
	string useFeatureReduction = argv[7];
	if(useFeatureReduction.compare("-fr")==0)
		featureReduction = true;
	else if(useFeatureReduction.compare("-wfr")==0)
		featureReduction = false;
	else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <number of clusters> <Balance> <Normalize> <input_images_folder>\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n";
		cout << "Feature Reduction: -fr to use| -wfr to not use Feature Reductionn \n\n";
		return 0;
	}

	string inputFileName  = argv[8];
	string outputFileName = argv[9];

	if(!FileOk(inputFileName)){
		cout << "\nCan't read file...\n";
		return 0;
	}

	int fileSize = FileSize(inputFileName);
	cout << "\nThe size of the file is: " << fileSize << "bytes\n";
	
	cout << "Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3\n";
	int height = 0, width = 0, BytesPerPixel = 1;
	cin >> width >> height >> BytesPerPixel;
	
	if(!( (height>0) && (width>0) && ((BytesPerPixel==1)||(BytesPerPixel==3)) ) ){
		cout << "INVALID INPUT!!\n";
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color>\n\n";
		return 0;
	}

	//Input from user for initial centroid for method 3
	vector<int> indexesOfInitialCentroids (numberOfClusters, 0);
	if(method == 3){
		int maxIndexPossibleY = height-1;
		int maxIndexPossibleX = width-1;
		cout << "\n\nEnter the indices of the pixels you want to be initial centroids";
		cout << "\nChoose any " << numberOfClusters << " points in the image" << endl;
		cout << "Range of X: 0 - " << maxIndexPossibleX << endl;
		cout << "Range of Y: 0 - " << maxIndexPossibleY << endl;

		for(int i=0; i<numberOfClusters; ++i){
			cout << "\nEnter pixel coordinates (x,y) for centroid #" << i << " and press enter/return: ";
			int inX, inY;
			cin >> inX >> inY;

			int currentI = i-1;
			
			if(inX > maxIndexPossibleX || inX < 0){
				cout << "\nChoose a value for x between 0 - " << maxIndexPossibleX << endl;
				i = currentI;
				continue;
			}

			if(inY > maxIndexPossibleY || inY < 0){
				cout << "\nChoose a value between 0 - " << maxIndexPossibleY << endl;
				i = currentI;
				continue;
			}

			indexesOfInitialCentroids[i] = inY*width + inX;
			cout << "Choosen index = " << indexesOfInitialCentroids[i] << endl;
		}

	}
	
	//Reading the image to a vector
	vector<vector<vector<unsigned char>>>ImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
	
	vector<vector<vector<float>>> lawsFilter = CreateLawsFilter (kernelSize);
	int numberOfFilters = lawsFilter.size();

	int intermediateWindowSize  = windowSize+kernelSize-1;
	int numberOfRegions = 0;
	int numberOfRegionsPredicted = width*height;

	ImageVector = FilterPreprocessing(ImageVector, windowSize);

	cout << "\nNew Width: " << ImageVector.front().size() << " New Height: " << ImageVector.size() << "\n\n";
	
	//Creating window buffer for storing local region
	vector<unsigned char> windowChannel (BytesPerPixel, 0);
	vector<vector<unsigned char>> windowColumn(windowSize, windowChannel);
	vector<vector<vector<unsigned char>>> window (windowSize, windowColumn);

	//Creating window buffer for filter operation
	//cout << "\nIntermediate Window Size: " << intermediateWindowSize << "\n\n";
	vector<vector<unsigned char>> intermediateWindowColumn (intermediateWindowSize, windowChannel);
	vector<vector<vector<unsigned char>>> intermediateWindow (intermediateWindowSize, intermediateWindowColumn);

	vector<int> balancedPixel (BytesPerPixel, 0);
	vector<vector<int>> balancedPixelRow (intermediateWindowSize, balancedPixel);
	vector<vector<vector<int>>> balancedImagePixelIntensities (intermediateWindowSize, balancedPixelRow);

	vector<float> FilterOutputs_singleRegion_singleRow (windowSize, 0);
	vector<vector<float>> FilterOutputs_singleRegion (windowSize, FilterOutputs_singleRegion_singleRow);
	vector<vector<vector<float>>> CollectionOfFilterOutputs_singleRegion (numberOfFilters, FilterOutputs_singleRegion);

	vector<float> featurePointsForSingleImage (numberOfFilters, 0);
	vector<vector<float>> featurePointsForAllRegions (numberOfRegionsPredicted, featurePointsForSingleImage);

	for(int p=0; p<height; p++){
    	for(int q=0; q<width; q++){

    		for(int i=0; i<windowSize; i++){
    			for(int j=0; j<windowSize; j++){
			    	for(int k=0; k<BytesPerPixel; k++){
			    		window[i][j][k] = ImageVector[ p+i ][ q+j ][k];

			    		// if(numberOfRegions%5000 == 0)
			    		// 	cout << "i: " << i << " j: " << j << " p: " << p << " q: " << q << " p+i: " << p+i << " q+j: " << q+j << endl;
			    	}
			    }
    		}
			    
			    	
			 
			//Balancing data - Calculate mean intensity of a region/window/image
			float averageRegionIntensity;
			if(balance){
				averageRegionIntensity = CalculateAverageRegionIntensitySingleRegion (window);
			} else {
				averageRegionIntensity = 0;
			} 

			//Pre-process region/image for filter-application
		    FilterPreprocessing(window, kernelSize, intermediateWindow);

		    //Subtract mean intensity to subjugate illumination effects
		    SubtractMeanPixelIntensitySingleRegion (intermediateWindow, averageRegionIntensity, balancedImagePixelIntensities);
			
		    //Apply Laws' filter on region/image
		    for(int j=0; j<numberOfFilters; j++)
		    	FastFeaturePointsOfSingleRegionUsingSingleLawsFilter(balancedImagePixelIntensities, lawsFilter.at(j), CollectionOfFilterOutputs_singleRegion.at(j));

		    //Calculate average energy for each filter for each region
			CalculateAverageEnergySingleRegion (CollectionOfFilterOutputs_singleRegion, featurePointsForSingleImage);
			
			featurePointsForAllRegions.at(numberOfRegions) = featurePointsForSingleImage;
			numberOfRegions++;

			if(numberOfRegions%5000 == 0){
				cout << "Working on pixel #" << numberOfRegions << endl;
				// cout << "Size of Filter-Outputs vector: ";
				// cout << CollectionOfFilterOutputs_singleRegion.size() << " x " << CollectionOfFilterOutputs_singleRegion.front().size() << " x ";
				// cout << CollectionOfFilterOutputs_singleRegion.front().front().size() << endl;
				// cout << "Size of Energy vector: " << featurePointsForSingleImage.size() << endl;
		    }
		}
	}

	cout << endl;
	for(int i=0; i<numberOfFilters; i++){
		cout << featurePointsForAllRegions[19000][i] << " ";
		cout << featurePointsForAllRegions[19150][i] << " ";
		cout << featurePointsForAllRegions[19275][i] << " ";
		cout << endl;
	}

	//#if 0

	
	//Display information about calculated feature-points
	cout << "Number of regions: " << numberOfRegions << endl << "Number of regions predicted: " << endl;
	cout << "\nSize of feature vector: ";
	if(numberOfRegions != numberOfRegionsPredicted){
		cout << "\n\n Error !!!!\n\n";
		return 0;
	}

	int numberOfFeatures = featurePointsForAllRegions.front().size();
	cout << numberOfRegions << " x " << numberOfFeatures << endl;

	cout << "\nFeature vector:\n";
	for(int i=0; i<numberOfRegions; i++){
		if(i%5000 == 0){
			for(int j=0; j<numberOfFeatures; j++){
				cout << setw(10) << setprecision(4) << featurePointsForAllRegions[i][j];
			}
			cout << endl;
		}
	}
	
	//Calculate average of all feature vectors
	cout << "\n\nAverage feature vector:\n";
	vector<float> averageFeatureVector = CalculateAverageOfAllFeaturePoints (featurePointsForAllRegions);
	for(float x : averageFeatureVector)
		cout << setw(10) << setprecision(4) << x;

	//Calculate standard deviation from the average feature vector
	cout << "\n\nStandard deviation vector:\n";
	vector<float> standardDeviationVector = CalculateStdDevOfAllFeaturePoints (featurePointsForAllRegions, averageFeatureVector);
	for(float x : standardDeviationVector)
		cout << setw(10) << setprecision(4) << x;

	//Normalizing Features-vector using Average-feature vector and standard deviation
	if(normalize){
		cout << "\n\nNormalized feature vector:\n";
		featurePointsForAllRegions =	NormalizeAllFeaturePoints (featurePointsForAllRegions, averageFeatureVector, standardDeviationVector);
		for(int i=0; i<numberOfRegions; i++){
			if(i%5000 == 0){
				for (int j=0; j<numberOfFeatures; j++){
					cout << setw(10) << setprecision(4) << featurePointsForAllRegions[i][j];
				}
				cout << endl;
			}
		}
	}
	
	if(featureReduction){
		cout << "\nUse Feature Reduction\n";
		vector<vector<float>> reducedFeaturePointsForAllImages = ReduceFeaturesUsingPCA(featurePointsForAllRegions);
		featurePointsForAllRegions.clear();
		featurePointsForAllRegions = reducedFeaturePointsForAllImages;
		cout << "\nSize of feature vector: ";
		numberOfRegions = featurePointsForAllRegions.size();
		numberOfFeatures = featurePointsForAllRegions.front().size();
		cout << numberOfRegions << " x " << numberOfFeatures << endl; 
	}

	//K-means initialization
	centroid_info initialCentroids;
	vector<vector<float>> CollectionOfInitialCentroids;
	vector<int> indexesOfSelectedCentroids;
	if(method == 1){
		int firstCenroidIndex = 5;
		cout << "\nMethod #" << method << " Initializaing with K-means++\n";
		initialCentroids = KMeansPlusPlusInitlization (featurePointsForAllRegions, numberOfClusters, firstCenroidIndex);
		CollectionOfInitialCentroids = initialCentroids.CollectionOfCentroids;
		indexesOfSelectedCentroids = initialCentroids.indexesOfSelectedCentroids;
	} else if(method == 2){
		cout << "\nMethod #" << method << " Initializaing with Triangular Initializer\n";
		initialCentroids = TriangularInitlization (featurePointsForAllRegions, numberOfClusters);
		CollectionOfInitialCentroids = initialCentroids.CollectionOfCentroids;
		indexesOfSelectedCentroids = initialCentroids.indexesOfSelectedCentroids;
	} else {
		cout << "\n\nMethod #" << method << " Initializaing manually (Supervised initialization)\n";
		cout << "Initial centroids are: " << endl;
			for(int i=0; i<numberOfClusters; i++){
				int inX, inY;
				inY = indexesOfInitialCentroids[i]/width;
				inX = indexesOfInitialCentroids[i] - inY*width;
				cout << inX << ", " << inY << endl;
				CollectionOfInitialCentroids.push_back(featurePointsForAllRegions.at(indexesOfInitialCentroids[i]));
			}
		
		indexesOfSelectedCentroids = indexesOfInitialCentroids;
	}


	cout << "\n\nNumber of Clusters and dimensions: " << CollectionOfInitialCentroids.size() << " x " << CollectionOfInitialCentroids.front().size() << endl;
	cout << "\nInitial centroids: " << endl;

	for(int i=0; i<numberOfClusters; i++){
		cout << "   " << indexesOfSelectedCentroids[i] << endl;
	}

	//K-means clustering
	vector <int> objectCluster = KMeansClustering (featurePointsForAllRegions, CollectionOfInitialCentroids);

	cout << "\n\nClustering complete!!\nObject cluster size: " << objectCluster.size();

	vector<unsigned char> resultChannel (BytesPerPixel, 0);
	vector<vector<unsigned char>> resultColumn (width, resultChannel);
	vector<vector<vector<unsigned char>>> resultImageVector (height, resultColumn);


	int labelConstant = 256/numberOfClusters;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<BytesPerPixel; k++){
				resultImageVector[i][j][k] = 50*objectCluster[(i*width + j)*BytesPerPixel + k];
				int index = (i*width + j)*BytesPerPixel + k;
			}	
		}
	}

	bool fileOuputOK_main = ImageOutput(outputFileName, resultImageVector);

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	//#endif
	//bool fileOuputOK_main = ImageOutput(outputFileName, ImageVector);

	//bool fileOuputOK_main = true;
	if(fileOuputOK_main){
	//if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "\nOutput file-name or the image is invalid...\n\n";
	}
	
	return 0;
}


	