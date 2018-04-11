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
	if( (argc != 7) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <Number of clusters> <Balance> <Normalize> <Initializer> <use Feature Reduction> <input_images_folder>\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n";
		cout << "Feature Reduction: -fr to use| -wfr to not use Feature Reductionn \n\n";
		return 0;
	}
	
	int numberOfClusters = stoi(argv[1]);

	bool balance;
	string balanceInput = argv[2];
	if(balanceInput.compare("-b")==0)
		balance = true;
	else if(balanceInput.compare("-wb")==0){
		balance = false;
		cout << "No balance\n\n";
	}
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

	bool normalize;
	string normalizeInput = argv[3];
	if(normalizeInput.compare("-n")==0)
		normalize = true;
	else if(normalizeInput.compare("-wn")==0)
		normalize = false;
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

	int method = stoi(argv[4]);
	if(method != 1 && method != 2 && method != 3){
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "TextureClassifcation <number of clusters> <Balance> <Normalize> <input_images_folder>\n";
		cout << "Balancing options  : -b for balance | -wb for no balance\n";
		cout << "Normalizing options: -n to normalize data | -wn for no normalization\n";
		cout << "Initializer options: 1-K++ means | 2-Triangular | 3-Manual (Already Coded) \n";
		cout << "Feature Reduction: -fr to use| -wfr to not use Feature Reductionn \n\n";
		return 0;
	}

	bool featureReduction;
	string useFeatureReduction = argv[5];
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

	string inputFolderName  = argv[6];
	if(inputFolderName.size() == 0){
		cout << "\nINVALID INPUT!!\n";
		cout << "Directory name invalid ...\n\n";
		return 0;
	}
	
	cout << "\nMake sure that all files have the same dimensions and number of channels\n";
	cout << "Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3\n";
	int height = 0, width = 0, BytesPerPixel = 1;
	cin >> width >> height >> BytesPerPixel;
	
	if(!( (height>0) && (width>0) && ((BytesPerPixel==1)||(BytesPerPixel==3)) ) ){
		cout << "INVALID INPUT!!\n";
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color>\n";
		return 0;
	}

	//Read files in a folder
	vector <string> fileNames = OpenFolder (inputFolderName);
	int numberOfFilesFound = fileNames.size();
	if(numberOfFilesFound == 0){
		cout << "\nNo such directory OR no files found\n\n";
	} else {
		cout << "\nChecking folder... " << numberOfFilesFound << " enties/files found:\n";
		for(int i=0; i<numberOfFilesFound; i++){
			cout << "Index " << i << " allotted to " << fileNames[i] << "\n";
		}
	}

	//Reading input images to make a collection of image vectors
	cout << "\nPreparing images ... \n";
	vector <string> finalFilesConsidered;
	vector<vector<vector<unsigned char>>> ImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_input;
	for(vector<string>::iterator itFileName = fileNames.begin(); itFileName!=fileNames.end(); itFileName++){

		string inputFileName = *itFileName;
		if(inputFileName.find(".raw") >= inputFileName.size()){
			numberOfFilesFound--;
			continue;
		}

		finalFilesConsidered.push_back(inputFileName);
		inputFileName = inputFolderName + "/" + inputFileName;
		cout << "Buffering: " << inputFileName << "\n";

		vector<vector<vector<unsigned char>>> tempImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);
		CollectionOfImageVectors_input.push_back(tempImageVector);
	}


	if(finalFilesConsidered.size() != numberOfFilesFound){
		cout << "\n\nError in reading files. Number of raw-files not counted properly";
		return 0;
	}

	cout << "\nAll " << numberOfFilesFound << " files with '.raw' extension have been included for processing\n\n";

	//Input from user for initial centroid for method 3
	vector<int> indexesOfInitialCentroids (numberOfClusters, 0);
	if(method == 3){
		int maxIndexPossible = numberOfFilesFound-1;
		cout << "\n\nEnter the indices of the images you want to be initial centroids";
		cout << "\nChoose any " << numberOfClusters << " numbers in the range 0 - " << maxIndexPossible << endl;

		for(int i=0; i<numberOfClusters; ++i){
			cout << "\nEnter index for centroid #" << i << " and press enter/return: ";
			int currentI = i-1;
			
			int tempIndex;
			cin >> tempIndex;
			//cout << "\ncurrent index = " << i;
			if(tempIndex < maxIndexPossible && tempIndex >= 0){
				indexesOfInitialCentroids[i] = tempIndex;
			}
			else{
				cout << "\nChoose a value between 0 - " << maxIndexPossible << endl;
				i = currentI;
			}
			//cout << "  current index = " << i;
		}

	}


	bool fileOuputOK_main = false;
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
	
	int kernalSize = 5;
	vector<vector<vector<float>>> lawsFilter = CreateLawsFilter (kernalSize);
	int numberOfImages = CollectionOfImageVectors_input.size();
	int imageHeight = CollectionOfImageVectors_input.front().size();
	int imageWidth = CollectionOfImageVectors_input.front().front().size();
	int numberOfChannels = CollectionOfImageVectors_input.front().front().front().size();

	//Balancing data - Calculate mean intensity for all regions/images
	vector<float> averageRegionIntensity;
	if(balance){
		cout << "Balancing data ... \n\n";
		averageRegionIntensity = CalculateAverageRegionIntensity (CollectionOfImageVectors_input);
	} else {
		vector<float> zeros(numberOfImages, 0);
		averageRegionIntensity = zeros;
	} 

	//Pre-process all regions/images for filter-application
    vector<vector<vector<unsigned char>>> intermediateImageVector; // Height x Width x BytesPerPixel
    vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_intermediate (numberOfImages, intermediateImageVector);
    for (int i=0; i<numberOfFilesFound; i++){
        cout << "Image #" << i << " - ";
        CollectionOfImageVectors_intermediate.at(i) = FilterPreprocessing(CollectionOfImageVectors_input.at(i), kernalSize);
    }

    //Subtract mean intensity to subjugate illumination effects
    vector<vector<vector<vector<int>>>> CollectionOfBalancedImagePixelIntensities = SubtractMeanPixelIntensity (CollectionOfImageVectors_intermediate, averageRegionIntensity);
	
    //Apply Laws' filter on all regions/images
	vector<vector<vector<vector<float>>>> CollectionOfFilterOutputs_allImages = FeaturePointsOfMultipleRegionsUsingMultipleLawsFilters(CollectionOfBalancedImagePixelIntensities, lawsFilter);

	cout << "\nSize of Filter-Outputs vector: ";
	cout << CollectionOfFilterOutputs_allImages.size() << " x " << CollectionOfFilterOutputs_allImages.front().size() << " x ";
	cout << CollectionOfFilterOutputs_allImages.front().front().size() << " x " << CollectionOfFilterOutputs_allImages.front().front().front().size() << "\n";
		
	//Calculate average energy for each filter for each region
	vector<vector<float>> featurePointsForAllImages = CalculateAverageEnergyOfRegions (CollectionOfFilterOutputs_allImages);

	//Display information about calculated feature-points
	cout << "\nSize of feature vector: ";
	int numberOfRegions = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	cout << numberOfRegions << " x " << numberOfFeatures << endl;

	cout << "\nFeature vector:\n";
	for(int i=0; i<numberOfRegions; i++){
		for(int j=0; j<numberOfFeatures; j++){	
			cout << setw(10) << setprecision(4) << featurePointsForAllImages[i][j];
		}
		cout << "   " << finalFilesConsidered[i] << "\n";
	}
	
	//Calculate average of all feature vectors
	cout << "\n\nAverage feature vector:\n";
	vector<float> averageFeatureVector = CalculateAverageOfAllFeaturePoints (featurePointsForAllImages);
	for(float x : averageFeatureVector)
		cout << setw(10) << setprecision(4) << x;

	//Calculate standard deviation from the average feature vector
	cout << "\n\nStandard deviation vector:\n";
	vector<float> standardDeviationVector = CalculateStdDevOfAllFeaturePoints (featurePointsForAllImages, averageFeatureVector);
	for(float x : standardDeviationVector)
		cout << setw(10) << setprecision(4) << x;

	//Normalizing Features-vector using Average-feature vector and standard deviation
	if(normalize){
		cout << "\n\nNormalized feature vector:\n";
		featurePointsForAllImages =	NormalizeAllFeaturePoints (featurePointsForAllImages, averageFeatureVector, standardDeviationVector);
		for(int i=0; i<numberOfRegions; i++){
			for (int j=0; j<numberOfFeatures; j++){
				cout << setw(10) << setprecision(4) << featurePointsForAllImages[i][j];
			}
			cout << "   " << finalFilesConsidered[i] << endl;
		}
	}

	if(featureReduction){
		cout << "\nUse Feature Reduction\n";
		vector<vector<float>> reducedFeaturePointsForAllImages = ReduceFeaturesUsingPCA(featurePointsForAllImages);
		featurePointsForAllImages.clear();
		featurePointsForAllImages = reducedFeaturePointsForAllImages;
		cout << "\nSize of feature vector: ";
		numberOfRegions = featurePointsForAllImages.size();
		numberOfFeatures = featurePointsForAllImages.front().size();
		cout << numberOfRegions << " x " << numberOfFeatures << endl; 
	}
		
	//K-means initialization
	centroid_info initialCentroids;
	vector<vector<float>> CollectionOfInitialCentroids;
	vector<int> indexesOfSelectedCentroids;
	if(method == 1){
		int firstCenroidIndex = 5;
		cout << "\nMethod #" << method << " Initializaing with K-means++\n";
		initialCentroids = KMeansPlusPlusInitlization (featurePointsForAllImages, numberOfClusters, firstCenroidIndex);
		CollectionOfInitialCentroids = initialCentroids.CollectionOfCentroids;
		indexesOfSelectedCentroids = initialCentroids.indexesOfSelectedCentroids;
	} else if(method == 2){
		cout << "\nMethod #" << method << " Initializaing with Triangular Initializer\n";
		initialCentroids = TriangularInitlization (featurePointsForAllImages, numberOfClusters);
		CollectionOfInitialCentroids = initialCentroids.CollectionOfCentroids;
		indexesOfSelectedCentroids = initialCentroids.indexesOfSelectedCentroids;
	} else {
		cout << "\n\nMethod #" << method << " Initializaing manually (Supervised initialization)\n";
		cout << "Initial centroids are: " << endl;
			for(int i=0; i<numberOfClusters; i++){
				cout << finalFilesConsidered[indexesOfInitialCentroids[i]] << endl;
				CollectionOfInitialCentroids.push_back(featurePointsForAllImages.at(indexesOfInitialCentroids[i]));
			}
		
		indexesOfSelectedCentroids = indexesOfInitialCentroids;
	}


	cout << "\n\nNumber of Clusters and dimensions: " << CollectionOfInitialCentroids.size() << " x " << CollectionOfInitialCentroids.front().size() << endl;
	cout << "\nInitial centroids: " << endl;

	for(int i=0; i<numberOfClusters; i++){
		for(int j=0; j<numberOfFeatures; j++){
			cout << setw(10) << setprecision(4) << CollectionOfInitialCentroids[i][j];
		}
		cout << "   " << finalFilesConsidered[ indexesOfSelectedCentroids[i] ] << endl;
	}

	//K-means clustering
	vector <int> objectCluster = KMeansClustering (featurePointsForAllImages, CollectionOfInitialCentroids);

	if(objectCluster.size() > 0){
		fileOuputOK_main = true;
		cout << "\n\nClustering complete!!\n";
		for(int i=0; i<numberOfClusters; i++){
			for(int j=0; j<numberOfRegions; j++){
				//cout << objectCluster[j] << " ";
				if(i == objectCluster[j]){
					cout << finalFilesConsidered[j] << " -> " << objectCluster[j] << endl;
				}
			}
			cout << endl;
		}	
	} else {
		cout << "\n\nClustering failed!!\n";
	}

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main){
	//if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "\nOutput file-name or the image is invalid...\n\n";
	}
	
	return 0;
}


	