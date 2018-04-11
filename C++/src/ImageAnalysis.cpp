///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ImageAnalysis.h"

using namespace std;

// Read symbols and store the number of occurences of each symbol
vector <int> GetHistogramSingleChannel(vector<vector<vector<unsigned char>>> ImageVector, int channelNumber){
	
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	
	cout << "Getting histogram for Channel " << channelNumber << " ...\n";
	
	vector<int> symbolCountVector(256, 0);

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			symbolCountVector[ImageVector[i][j][channelNumber]]++;
		}
	}
	
	return symbolCountVector;
}

// Manipulating Histogram - Cumulative Probability Distribution
// Used mainly to increase image brightness as nearly all pixels are replced by increased values
// The already bright areas will get over saturated and flooded with white
vector<int> SingleChannelHistogramCDF (vector<int> singleChannelHistogram){

    vector<int> resultHistogram(256, 0);
	resultHistogram[0] = singleChannelHistogram[0];

	for (int i=1; i<256; i++){
    	resultHistogram[i] = resultHistogram[i-1] + singleChannelHistogram[i];
    }

    return resultHistogram;
}

// Print histogram information of a channel to a file
//bool PrintHistogramToFile(string fileName, vector <unsigned long int> histogramVector){
bool PrintHistogramToFile(string fileName, vector <int> histogramVector){

	cout << "Printing histogram to file...\n";
	
    if(fileName.size()==0){
		return false;
	}
	
	//Always use binary method for reading/writing files
	//Othwerwise extra bits are included which are platform dependent
	const char* output_file = fileName.c_str();
	ofstream outputfile(output_file, ios::binary);
	
	int i=0;
	for(vector<int>::iterator it = histogramVector.begin(); it!=histogramVector.end(); it++){
				outputfile << i++;
				outputfile << ',';
				outputfile << *it;
				outputfile << '\n';
	}

	outputfile.close();
	
	return true;
}

//Match histogram of a channel with a target histogram
vector<vector<vector<unsigned char>>> SingleChannelHistogramMatching(vector<vector<vector<unsigned char>>> SingleChannelVector,
																	 vector<int> targetHistogram, vector<int> subjectHistogram){

	int height = SingleChannelVector.size();
	int width = SingleChannelVector.front().size();

	cout << "Histogram matching in process ...\n";
	vector<unsigned char> resultMap (256, 0);
	for(int i=0; i<256; i++){
		int min = INT_MAX;
		for(int j=0; j<256; j++){
			int test = abs(subjectHistogram[i]-targetHistogram[j]);
			if(test<min){
				min = test;
				resultMap[i] = (unsigned char)j;
			}
		}
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			SingleChannelVector[i][j][0] = resultMap[SingleChannelVector[i][j][0]];
		}
	}

	return	SingleChannelVector;
}

//Return Mean-Square Difference (Error) between two images
float CalculateMSD(vector<vector<vector<unsigned char>>>outputImageVector, vector<vector<vector<unsigned char>>>ImageVector){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();
	int totalPixels = height*width*channels;

	long sumSqDiff = 0;

	//Calculating the PSNR for the supplied pair of images
	for(int y=0; y<height; y++){
		for(int x=0; x<width; x++){
			for(int z=0; z<channels; z++){
				int temp = outputImageVector[y][x][z] - ImageVector[y][x][z];
				sumSqDiff += temp*temp;
			}
		}
	}
	
	float meanSqError = (float)sumSqDiff/(float)totalPixels;
	return meanSqError;
}

//Return PSNR
float CalculatePSNR(float meanSquareDifference, int MAX){
	float psnr = 10*log10((MAX*MAX)/meanSquareDifference);
	return psnr;
}


/////////////////////////////////////////////	TEXTURE ANALYSIS	/////////////////////////////////////////////////////////////////
//Calculate average energy of a group of regions: Energy is calculated pixel-wise, summed over and divided by total number of pixels
vector<vector<float>> CalculateAverageEnergyOfRegions (vector<vector<vector<vector<float>>>> CollectionOfFilterOutputs_allRegions){

	vector<vector<float>> featurePointsForAllRegions;

	int numberOfRegions = CollectionOfFilterOutputs_allRegions.size();
	int numberOfFeatures = CollectionOfFilterOutputs_allRegions.front().size();
	int RegionHeight = CollectionOfFilterOutputs_allRegions.front().front().size();
	int RegionWidth = CollectionOfFilterOutputs_allRegions.front().front().front().size();
	int pixelsPerRegion = RegionWidth * RegionHeight;

	for(int i=0; i<numberOfRegions; i++){
		
		vector<float> featurePointsSingleRegion;
		for(int j=0; j<numberOfFeatures; j++){	
			
			float runningSum = 0;
			for(int p=0; p<RegionHeight; p++){
				for (int q=0; q<RegionWidth; q++){
					float filterOutputForCurrentPixel = CollectionOfFilterOutputs_allRegions[i][j][p][q];
					runningSum += filterOutputForCurrentPixel * filterOutputForCurrentPixel;
				}
			}

			runningSum /= pixelsPerRegion;
			featurePointsSingleRegion.push_back(runningSum);
		}

		featurePointsForAllRegions.push_back(featurePointsSingleRegion);
	}

	return featurePointsForAllRegions;
}

//Calculate average energy of a SINGLE region: Energy is calculated pixel-wise, summed over and divided by total number of pixels - PASS BY REFERENCE
void CalculateAverageEnergySingleRegion (vector<vector<vector<float>>> &CollectionOfFilterOutputs_singleRegion,
										 vector<float> &featurePointsSingleRegion){

	int numberOfFeatures = CollectionOfFilterOutputs_singleRegion.size();
	int RegionHeight = CollectionOfFilterOutputs_singleRegion.front().size();
	int RegionWidth = CollectionOfFilterOutputs_singleRegion.front().front().size();
	int pixelsInRegion = RegionWidth * RegionHeight;

	for(int i=0; i<numberOfFeatures; i++){	
		
		float runningSum = 0;
		for(int p=0; p<RegionHeight; p++){
			for (int q=0; q<RegionWidth; q++){
				float filterOutputForCurrentPixel = CollectionOfFilterOutputs_singleRegion[i][p][q];
				runningSum += filterOutputForCurrentPixel * filterOutputForCurrentPixel;
			}
		}

		runningSum /= pixelsInRegion;
		featurePointsSingleRegion[i] = runningSum;
	}
}

//Calculate average of feature points from all images
vector<float> CalculateAverageOfAllFeaturePoints (vector<vector<float>> featurePointsForAllImages){

	int numberOfObjects = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();

	vector<float> averageFeatureVector(numberOfFeatures, 0);
	
	for(int i=0; i<numberOfObjects; i++)
		for (int j=0; j<numberOfFeatures; j++)
			averageFeatureVector[j] += featurePointsForAllImages[i][j]/(float)numberOfObjects;

	return averageFeatureVector;
}

//Calculate standard deviation of feature points from all images
vector<float> CalculateStdDevOfAllFeaturePoints (vector<vector<float>> featurePointsForAllImages, vector<float> averageFeatureVector){

	int numberOfObjects = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	vector<float> standardDeviationVector(numberOfFeatures, 0);

	for(int i=0; i<numberOfObjects; i++){
		for (int j=0; j<numberOfFeatures; j++){
			float diff = averageFeatureVector[j] - featurePointsForAllImages[i][j];
			standardDeviationVector[j] += (diff*diff)/((float)numberOfObjects);
		}
	}

	for (int j=0; j<numberOfFeatures; j++)
		standardDeviationVector[j] = sqrt(standardDeviationVector[j]);

	return	standardDeviationVector;
}

//K++ initialization, first centroid must be passed and can be chosen randomly or manually, random selction is not implemented
centroid_info KMeansPlusPlusInitlization (vector<vector<float>> featurePointsForAllImages, int numberOfClusters, int firstCentoidIndex){

	bool printAll = false;

	//Step 1: Calculate Euclidean distance of each feature-point from existing centroids - Dsq(x)
	//Step 2: Find least distance among from closest Centroid for every feature-point
	//Step 3: Calculate sum of distances between feature-points and their closest centroids - Sum(Dsq(x))
	//Step 4: Calculate probablity of selection = Dsq(x)/Sum(Dsq(x))

	int numberOfObjects = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	vector<vector<float>> CollectionOfCentroids;
	vector<int> indexesOfSelectedCentroids;

	//Let first centroid be the first feature-vector (data point) itself or select a random one (user input is ok)
	CollectionOfCentroids.push_back(featurePointsForAllImages.at(firstCentoidIndex));
	indexesOfSelectedCentroids.push_back(firstCentoidIndex);

	//float distanceFromClosestCentroid[numberOfObjects];
	vector<float> distanceFromClosestCentroid (numberOfObjects, FLT_MAX);

	for(int i=0; i<numberOfClusters-1; i++){

		//Display info about already chosen centroids
		int numberOfCentroids = CollectionOfCentroids.size();

		cout << "\nSelecting Centroid #" << i+1 << endl;
		cout << "Number of existing centroids = " << numberOfCentroids << endl;
		cout << "Existing centroids: ";
		for(int x : indexesOfSelectedCentroids)
			cout << x << " ";
		cout << endl;

		//Executing steps 1, 2 and 3
		if(i!=0)
			fill (distanceFromClosestCentroid.begin(), distanceFromClosestCentroid.end(), FLT_MAX);

		float sumOfDistancesOfFeaturePointAndClosestCentroids = 0;
		for(int j=0; j<numberOfObjects; j++){

			//Skip feature-point already selected as a centroid
			if( find(indexesOfSelectedCentroids.begin(), indexesOfSelectedCentroids.end(), j) != indexesOfSelectedCentroids.end())
				continue;

			//Step 1: Calculate Euclidean distance of each feature-point from existing centroids - Dsq(x)

			bool printDetails = false;
			if(j%5000 == 0){
				printDetails = true & printAll;
			}

			if(printDetails)
				cout << "For object #" << j << "\n";
			
			for(int p=0; p<numberOfCentroids; p++){
			
				float currentCentroidDistance = 0;
				for(int k=0; k<numberOfFeatures; k++){
					float tempDistance = featurePointsForAllImages[j][k] - CollectionOfCentroids[p][k];
					currentCentroidDistance += tempDistance * tempDistance;
				}

				//Step 2: Find least distance among from closest Centroid for every feature-point
				if(distanceFromClosestCentroid[j] > currentCentroidDistance)
					distanceFromClosestCentroid[j] = currentCentroidDistance;

				if(printDetails){
					cout << "Centroid #" << p << " " << currentCentroidDistance << " " << distanceFromClosestCentroid[j] << endl;
				}	

			}

			//Step 3: Calculate sum of distances between feature-points and their closest centroids - Sum(Dsq(x))
			sumOfDistancesOfFeaturePointAndClosestCentroids += distanceFromClosestCentroid[j];

			if(printDetails){
				cout << " " << sumOfDistancesOfFeaturePointAndClosestCentroids << endl;	
			}
			
		}

		//Step 4: Calculate probablity of selection = Dsq(x)/Sum(Dsq(x))
		if(printAll)
			cout << "Finding probablity of selection:\n";
		float maxProbability = FLT_MIN;
		int indexOfFeaturePoint = -1;
		for(int j=0; j<numberOfObjects; j++){

			if( find(indexesOfSelectedCentroids.begin(), indexesOfSelectedCentroids.end(), j) != indexesOfSelectedCentroids.end())
				continue;

			float probabilityOfSelection = distanceFromClosestCentroid[j]/sumOfDistancesOfFeaturePointAndClosestCentroids;

			if(j%5000 == 0 && printAll){
				cout << j << " " << probabilityOfSelection << "  " << endl;	
			}
			
			
			if(maxProbability < probabilityOfSelection){
				indexOfFeaturePoint = j;
				maxProbability = probabilityOfSelection;
			}
		}

		cout << "Feature-point selected as centroid: " << indexOfFeaturePoint << " Max probability = " << maxProbability << endl;
		indexesOfSelectedCentroids.push_back(indexOfFeaturePoint);
		CollectionOfCentroids.push_back(featurePointsForAllImages.at(indexOfFeaturePoint));
	}

	centroid_info initialCentroids;
	initialCentroids.CollectionOfCentroids = CollectionOfCentroids;
	initialCentroids.indexesOfSelectedCentroids = indexesOfSelectedCentroids;

	return initialCentroids;
}

//Triangular Initialization
centroid_info TriangularInitlization (vector<vector<float>> featurePointsForAllImages, int numberOfClusters){

	//Let first centroid be the first feature-vector (data point) itself or select a random one (user input is ok)
	//Calculate Euclidean distance of each feature-point from already existing centroids
	//Store the distances in a vector
	//Calculate sum of distances of a feature-point from all centroids
		//A data point is considered to be initializing centroid when it is farthest away from all the centroids decided till now
		//Find feature-point having maximum distance from the already selected centroids
	bool printAll = false;

	int numberOfObjects = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	vector<vector<float>> CollectionOfCentroids;
	vector<int> indexesOfSelectedCentroids;

	//Let first centroid be the first feature-vector (data point) itself or select a random one (user input is ok)
	CollectionOfCentroids.push_back(featurePointsForAllImages.at(5));
	indexesOfSelectedCentroids.push_back(5);

	float sumOfDistancesOfFeaturePointFromCentroids[numberOfObjects];

	for(int i=0; i<numberOfClusters-1; i++){

		//Data-structure for storing sum of Distances from Centroids
		int numberOfCentroids = CollectionOfCentroids.size();

		cout << "\nSelecting Centroid #" << i+1 << endl;
		cout << "Number of existing centroids = " << numberOfCentroids << endl;
		cout << "Existing centroids: ";
		for(int x : indexesOfSelectedCentroids)
			cout << x << " ";

		cout << endl;

		//vector<float> sumOfDistancesOfFeaturePointFromCentroids (numberOfObjects, 0);
		memset(sumOfDistancesOfFeaturePointFromCentroids, 0, sizeof(sumOfDistancesOfFeaturePointFromCentroids));

		//Calculate Euclidean distance of each feature-point from already existing centroids
		for(int j=0; j<numberOfObjects; j++){

			//Skip feature-point already selected as a centroid
			if( find(indexesOfSelectedCentroids.begin(), indexesOfSelectedCentroids.end(), j) != indexesOfSelectedCentroids.end())
				continue;

			float sumOfDistancesFromCentroids = 0;	
			
			bool printDetails = false;
			if(j%5000 == 0){
				cout << "For object #" << j << "\n";
				printDetails = true & printAll;
			}
			

			for(int p=0; p<numberOfCentroids; p++){
			
				float currentCentroidDistance = 0;
				for(int k=0; k<numberOfFeatures; k++){
					float tempDistance = featurePointsForAllImages[j][k] - CollectionOfCentroids[p][k];
					currentCentroidDistance += tempDistance * tempDistance;
				}

				//Calculate sum of distances of a feature-point from all centroids
				sumOfDistancesFromCentroids += currentCentroidDistance;
				sumOfDistancesOfFeaturePointFromCentroids[j] = sumOfDistancesFromCentroids;
				
				if(printDetails){
					cout << "Centroid #" << p << " " << currentCentroidDistance << " " << sumOfDistancesOfFeaturePointFromCentroids[j] << endl;
				}
			}
		}

		//Find feature-point having maximum distance from the already selected 
		float maxSumOfDistancesFromCentroids = FLT_MIN;
		int indexOfFeaturePoint = -1;
		for(int j=0; j<numberOfObjects; j++){

			if( find(indexesOfSelectedCentroids.begin(), indexesOfSelectedCentroids.end(), j) != indexesOfSelectedCentroids.end())
				continue;

			if(maxSumOfDistancesFromCentroids < sumOfDistancesOfFeaturePointFromCentroids[j]){
				indexOfFeaturePoint = j;
				maxSumOfDistancesFromCentroids = sumOfDistancesOfFeaturePointFromCentroids[j];
			}
		}

		indexesOfSelectedCentroids.push_back(indexOfFeaturePoint);
		maxSumOfDistancesFromCentroids = sumOfDistancesOfFeaturePointFromCentroids[indexOfFeaturePoint];

		cout << "Feature-point selected as centroid: " << indexOfFeaturePoint << " Max distance from existing centroids = " << maxSumOfDistancesFromCentroids << endl;
		CollectionOfCentroids.push_back(featurePointsForAllImages.at(indexOfFeaturePoint));		
	}

	centroid_info initialCentroids;
	initialCentroids.CollectionOfCentroids = CollectionOfCentroids;
	initialCentroids.indexesOfSelectedCentroids = indexesOfSelectedCentroids;

	return initialCentroids;
}

//K-Means clustering
vector<int> KMeansClustering (vector<vector<float>> featurePointsForAllImages, vector<vector<float>> CollectionOfInitialCentroids){

	bool printAll = false;

	cout << "\n\nClustering in process ... \n";

	int numberOfObjects = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	int numberOfClusters = CollectionOfInitialCentroids.size();

	vector <int> objectCluster(numberOfObjects, -1);
	vector<vector<float>> CollectionOfCentroids = CollectionOfInitialCentroids;
	
	int iteration=0;
	
	while(iteration < 500){

		//Intialize temp cluster with different value to make the loop run at least once
		vector <int> tempObjectCluster(numberOfObjects, -2);
		
		// Step 1 - Do for all feature vectors
		// a) Find Euclidean Distance between of a Feature-vector from each current centroids
		// b) Assign the index of the centroid (cluster number) closest to the feature-vector
		// c) Check if current assignment is same as previous, stop if same
		for(int i=0; i<numberOfObjects; i++){

			bool printDetails = false;
			if(i%5000 == 0)
				printDetails = true & printAll;
			
			float leastDistance = FLT_MAX;
			for(int j=0; j<numberOfClusters; j++){
				
				// a) Find Euclidean Distance between of a Feature-vector from each current centroids
				float currentDistance = 0;
				for(int k=0; k<numberOfFeatures; k++){
					float tempDistance = featurePointsForAllImages[i][k] - CollectionOfCentroids[j][k];
					currentDistance += tempDistance * tempDistance;
				}

				currentDistance = sqrt(currentDistance);

				// b) Assign the index of the centroid (cluster number) closest to the feature-vector
				if(currentDistance < leastDistance){
					leastDistance = currentDistance;
					tempObjectCluster[i] = j;
				}

				
				if(printDetails){
					cout << setw(3) << tempObjectCluster[i] << " " << setw(6) << currentDistance << " ";
				}

			}

			if(printDetails){
				cout << setw(3) << tempObjectCluster[i] << " " << setw(6) << leastDistance << endl;
			}
		}

		// c) Check if current assignment is same as previous, stop if same
		bool clusteringComplete = true;
		for(int i=0; i<numberOfObjects; i++){
			if(objectCluster[i] != tempObjectCluster[i]){
				clusteringComplete = false;
				objectCluster[i] = tempObjectCluster[i];
			}
		}

		if (clusteringComplete)
			break;

		iteration++;
		cout << "\nIteration #" << iteration;

		
		// Step 2 - Calculate new centroids
		// a) Add all the feature-points belonging to same cluster
		// b) Update number of objects in a cluster
		// c) Calculate average (centroid) from sum obtained in Step a), push new centroid to the centroid vector
		vector<float> newCentroid(numberOfFeatures, 0);
		vector<vector<float>> newCollectionOfCentroids (numberOfClusters, newCentroid);
		vector<int> numberOfObjectsInClusters (numberOfClusters, 0);

		for(int i=0; i<numberOfObjects; i++){

			// a) Add all the feature-points belonging to same cluster
			int indexOfClusterForCurrentObject = tempObjectCluster[i];
			for(int j=0; j <numberOfFeatures; j++)
				newCollectionOfCentroids[indexOfClusterForCurrentObject][j] += featurePointsForAllImages[i][j];

			// b) Update number of objects in a cluster
			numberOfObjectsInClusters[indexOfClusterForCurrentObject]++;
		}

		// c) Calculate average (centroid) from sum obtained in Step a), push new centroid to the centroid vector
		for(int i=0; i<numberOfClusters; i++){
			for(int j=0; j <numberOfFeatures; j++){
				CollectionOfCentroids[i][j] = newCollectionOfCentroids[i][j]/(float)numberOfObjectsInClusters[i];
			}
		}

		cout << " - New centroids: \n";
		for(int i=0; i<numberOfClusters; i++){
			for(int j=0; j<numberOfFeatures; j++){
				cout << setw(10) << setprecision(4) << CollectionOfCentroids[i][j];
			}
			cout << "\n";
		}

	}

	return objectCluster;
}

//Find average intensity of a group of regions
vector<float> CalculateAverageRegionIntensity (vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_input){
	
	int numberOfImages = CollectionOfImageVectors_input.size();
	int imageHeight = CollectionOfImageVectors_input.front().size();
	int imageWidth = CollectionOfImageVectors_input.front().front().size();
	int numberOfChannels = CollectionOfImageVectors_input.front().front().front().size();
	int numberOfPixels = imageWidth * imageHeight;

	vector<float> averageRegionIntensity (numberOfImages, 0);
	for(int p=0; p<numberOfImages; p++){
		
		float sum = 0;
		for(int i=0; i<imageHeight; i++)
			for(int j=0; j<imageWidth; j++)
				for(int k=0; k<numberOfChannels; k++)
					sum += (float)CollectionOfImageVectors_input[p][i][j][k];
				
		averageRegionIntensity[p] = sum/(float)numberOfPixels;
	}

	return averageRegionIntensity;
}

//Find average intensity of a single region
float CalculateAverageRegionIntensitySingleRegion (vector<vector<vector<unsigned char>>> &singleImageVector){
	
	int imageHeight = singleImageVector.size();
	int imageWidth = singleImageVector.front().size();
	int numberOfChannels = singleImageVector.front().front().size();
	int numberOfPixels = imageWidth * imageHeight;

	float averageRegionIntensity = 0;
	float sum = 0;
	for(int i=0; i<imageHeight; i++)
		for(int j=0; j<imageWidth; j++)
			for(int k=0; k<numberOfChannels; k++)
				sum += (float)singleImageVector[i][j][k];
			
	averageRegionIntensity = sum/(float)numberOfPixels;

	return averageRegionIntensity;
}

//Subtract mean intensity to subjugate illumination effects - For a group of regions
vector<vector<vector<vector<int>>>> SubtractMeanPixelIntensity (vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_intermediate,
																vector<float> averageRegionIntensity){

	int imageHeight = CollectionOfImageVectors_intermediate.front().size();
	int imageWidth = CollectionOfImageVectors_intermediate.front().front().size();
	int numberOfImages = CollectionOfImageVectors_intermediate.size();
	int numberOfChannels = CollectionOfImageVectors_intermediate.front().front().front().size();

	vector<int> intermediatePixelChannel (numberOfChannels, 0);
	vector<vector<int>> intermediatePixelColumn (imageWidth, intermediatePixelChannel);
    vector<vector<vector<int>>> intermediateImagePixelsHolder (imageHeight, intermediatePixelColumn);
    vector<vector<vector<vector<int>>>> CollectionOfBalancedImagePixelIntensities (numberOfImages, intermediateImagePixelsHolder);

	for(int p=0; p<numberOfImages; p++)
		for(int i=0; i<imageHeight; i++)
			for(int j=0; j<imageWidth; j++)
				for(int k=0; k<numberOfChannels; k++)
					CollectionOfBalancedImagePixelIntensities[p][i][j][k] = (int)CollectionOfImageVectors_intermediate[p][i][j][k] - averageRegionIntensity[p];

	return CollectionOfBalancedImagePixelIntensities;
}

//Subtract mean intensity to subjugate illumination effects - For a single region (PASS BY REFERENCE)
void SubtractMeanPixelIntensitySingleRegion (vector<vector<vector<unsigned char>>> &ImageVector_intermediate, float &averageRegionIntensity,
											   vector<vector<vector<int>>> &balancedImagePixelIntensities){

	int imageHeight = ImageVector_intermediate.size();
	int imageWidth = ImageVector_intermediate.front().size();
	int numberOfChannels = ImageVector_intermediate.front().front().size();

	//cout << "Width: " << imageWidth << " Height: " << imageHeight << " Channels: " << numberOfChannels << endl;

	// vector<int> intermediatePixelChannel (numberOfChannels, 0);
	// vector<vector<int>> intermediatePixelColumn (imageWidth, intermediatePixelChannel);
	// vector<vector<vector<int>>> balancedImagePixelIntensities (imageHeight, intermediatePixelColumn);

	for(int i=0; i<imageHeight; i++)
		for(int j=0; j<imageWidth; j++)
			for(int k=0; k<numberOfChannels; k++){
				balancedImagePixelIntensities[i][j][k] = (int)ImageVector_intermediate[i][j][k] - averageRegionIntensity;
			}

	//return balancedImagePixelIntensities;
}

//Normalizing Features-vector using Average-feature vector and standard deviation
vector<vector<float>> NormalizeAllFeaturePoints (vector<vector<float>> featurePointsForAllImages, vector<float> averageFeatureVector,
												 vector<float> standardDeviationVector) {

	//normalized value = (original value - mean) / std deviation
	int numberOfRegions = featurePointsForAllImages.size();
	int numberOfFeatures = featurePointsForAllImages.front().size();
	for(int i=0; i<numberOfRegions; i++)
		for (int j=0; j<numberOfFeatures; j++)
			if(standardDeviationVector[j] != 0)
				featurePointsForAllImages[i][j] = (featurePointsForAllImages[i][j] - averageFeatureVector[j])/standardDeviationVector[j];

	return featurePointsForAllImages;
}

//Get edge-map for an image using Sobel-filter, gives a tuned output according to threshold
vector<vector<vector<unsigned char>>> EdgeDetectionSobelFilter (vector<vector<vector<unsigned char>>> ImageVector, int threshold){

	//Converting to grayscale
	vector<vector<vector<unsigned char>>> manipulatedImageVector = GrayLuminosity(ImageVector);
	
	//Differentiating image to get edges
	manipulatedImageVector = DifferentiateImageSobelFilter (manipulatedImageVector);

	int height = manipulatedImageVector.size();
	int width = manipulatedImageVector.front().size();

	unsigned char maxPixeValue = 0;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			unsigned char pixel = manipulatedImageVector[i][j][0]; 
			if (pixel > maxPixeValue)
				maxPixeValue = pixel;
		}
	}

	unsigned char edgeThreshold = (unsigned char)((float)maxPixeValue*(float)threshold/100.0);

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			unsigned char pixel = manipulatedImageVector[i][j][0]; 
			if (pixel > edgeThreshold)
				manipulatedImageVector[i][j][0] = 255;
			else
				manipulatedImageVector[i][j][0] = 0;
		}
	}
	
	//Negating image to get edge map
	manipulatedImageVector = RGBtoCMY (manipulatedImageVector);
	
	return manipulatedImageVector;
}

//Intermidate step of edge detection using LoG, gets a ternary edge map (-1 0 1) to be used for zero crossing later
//Uses histogram of output from LoG to decide pixels belongin to background and pixel that contribute to edges
//Pass empty string if histogram is not required
vector<vector<vector<unsigned char>>> GetTernaryMap (double_differentiated_image doubleDifferentiatedImageInfo, int threshold,
													 string outputFileName, bool writeHistogramToFile){

	vector<vector<vector<unsigned char>>> normalizedDoubleDifferentiatedImage;
	normalizedDoubleDifferentiatedImage = doubleDifferentiatedImageInfo.NormalizedDoubleDifferentiatedImage;

	//Finding knee points according to threshold
	vector<int> normalizedLoGHistogramVector = GetHistogramSingleChannel(normalizedDoubleDifferentiatedImage, 0);

	if(writeHistogramToFile){
		string tempFilename = outputFileName;
		string extension = "_ternary_edge_map_threshold_";
		extension.append(to_string(threshold));
		extension.append(".raw");
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), extension);
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), "_histogram_of_normalized_output.csv");
		PrintHistogramToFile(tempFilename, normalizedLoGHistogramVector);	
	}
	
	//Find that gray-level which has maximum number of pixels assigned to it, also find number of pixel assigned to it
	//Most probably, it should be 0 or something very near to it
	vector<int>::iterator pointerToMaxElementInHistogram = max_element(normalizedLoGHistogramVector.begin(), normalizedLoGHistogramVector.end());
	int indexOfMaxElementInHistogram = distance (normalizedLoGHistogramVector.begin(), pointerToMaxElementInHistogram);
	int maxNumberOfPixelsAtAGrayLevel = normalizedLoGHistogramVector[indexOfMaxElementInHistogram];

	int cumulativeNumberOfPixels = 0;
	int height = normalizedDoubleDifferentiatedImage.size();
	int width = normalizedDoubleDifferentiatedImage.front().size();
	int totalNumberOfPixels = height*width;
	int minCutoffIndex = 0;
	float thresholdLevel = (float)threshold/100.0;
	while ( ((float)cumulativeNumberOfPixels/(float)totalNumberOfPixels) < thresholdLevel ){
		cumulativeNumberOfPixels += normalizedLoGHistogramVector[minCutoffIndex];
		minCutoffIndex++;
	}

	//Subtract 1 from minCutoffIndex, 1 count is present extra 
	//because of stopping condition of while loop
	minCutoffIndex--;
	int kneeDistance = indexOfMaxElementInHistogram - minCutoffIndex;

	//Since index of histogram vector ranges in 0-255,
	//And since distritution peaks at 0
	//gray-level at knee distance = knee distance
	float denormalizedGrayLevelAtKneePointPositive = (float)kneeDistance;
	float denormalizedGrayLevelAtKneePointNegative = -denormalizedGrayLevelAtKneePointPositive;
	
	cout << "\nFinding knee point in histogram ... \n";

	cout << setw(10) << setprecision(4) << "\nIndex of gray level having maximum pixels: " << indexOfMaxElementInHistogram << endl;
    cout << setw(10) << setprecision(4) << "Maximum number of pixels at a gray level : " << maxNumberOfPixelsAtAGrayLevel << endl;
    cout << setw(10) << setprecision(4) << "Knee distance from maximum in histogram  : " << kneeDistance << endl;
    cout << setw(10) << setprecision(4) << "Positive gray level at knee point : " << denormalizedGrayLevelAtKneePointPositive << endl;
    cout << setw(10) << setprecision(4) << "Negative gray level at knee point : " << denormalizedGrayLevelAtKneePointNegative << endl << endl;

	//Producing a ternary map
	vector<vector<vector<float>>> doubleDifferentiatedImage = doubleDifferentiatedImageInfo.DoubleDifferentiatedImage;

	vector<unsigned char> resultPixelValue (1, 0);
    vector<vector<unsigned char>> resultPixelValueRow (width, resultPixelValue);
    vector<vector<vector<unsigned char>>> resultImage (height, resultPixelValueRow);

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			float currentPixelValue = doubleDifferentiatedImage[i][j][0];
			if(currentPixelValue < denormalizedGrayLevelAtKneePointNegative)
				resultImage[i][j][0] = 64;
			else if(currentPixelValue >= denormalizedGrayLevelAtKneePointNegative && currentPixelValue <= denormalizedGrayLevelAtKneePointPositive)
				resultImage[i][j][0] = 128;
			else
				resultImage[i][j][0] = 192;
		}
	}

	return resultImage;
}

//Detect edges using Zero-crossing morphological (4-connected) filters, final step of edge-detection using LoG
vector<vector<vector<unsigned char>>> GetZeroCrossingEdgeMap (vector<vector<vector<unsigned char>>> manipulatedImageVector, int method){

	int kernelSize = 3;
	int height = manipulatedImageVector.size();
	int width = manipulatedImageVector.front().size();
	int outputHeight = height-kernelSize+1;
    int outputWidth  = width-kernelSize+1;

    manipulatedImageVector = FilterPreprocessing(manipulatedImageVector, kernelSize);

	//Background of edge-map is gray. This is done to see if code is running fine
	//A grayush image would indicate that not all pixel's were processed
	vector<unsigned char> resultPixel (1, 100);
    vector<vector<unsigned char>> resultPixelRow (width, resultPixel);
    vector<vector<vector<unsigned char>>> resultEdgeMap (height, resultPixelRow);

    //This method that was mentioned in the book
    //Digital Image processing by William K.Pratt
    //Page 520, Section 15.3.2 LoG Edge Detection
	if(method == 1){

		for(int q=0; q<outputHeight; q++){
	        for(int p=0; p<outputWidth; p++){
	            
	        	bool pixel_is_edge = false;
	        	bool pixel_is_background = false;
	        	
	        	int currentPixelSign = -2;
	        	int firstNeighborSign = -2;
	        	int countOfPositiveNeighbors = 0;

	        	unsigned char currentPixelValue = manipulatedImageVector[q][p][0];
	        	if(currentPixelValue == 128)
	        		currentPixelSign = 0;
	        	else if (currentPixelValue > 128)
	        		currentPixelSign = 1;
	        	else
	        		currentPixelSign = -1;

	            for(int j=0; j<kernelSize; j++){
	                for(int i=0; i<kernelSize; i++){

	                	//Window has 9 elements, counting them
	                	//linearly to check for 2, 4, 6 and 8
	                	//i.e. top, down, left and right
	                	int linearWindowIndex = (j*kernelSize) + i;

	                	if(linearWindowIndex%2 == 1){


	                		unsigned char currentNeighborValue = manipulatedImageVector[ j+q ][ i+p ][0];
	                		int currentNeighborSign = -2;

	                		if(currentNeighborValue == 128)
				        		currentNeighborSign = 0;
				        	else if (currentNeighborValue > 128)
				        		currentNeighborSign = 1;
				        	else
				        		currentNeighborSign = -1;

	                		//Can't make any decision based on just first neighbor's sign value
	                		//Need at least one ore neighbour's sign
	                		//Update first neighbor's sign value
	                		if(linearWindowIndex == 1){
	                			firstNeighborSign = currentNeighborSign;
	                			continue;
	                		}

	                		//If any neighbor is 0
	                			//pixel is not an edge
	                		if(currentNeighborSign == 0){
								pixel_is_background = true;
		                		break;
							} else if(currentNeighborSign == 1){
	                			countOfPositiveNeighbors++;
	                		}

	                		//IF current pixel is Non-Negative (0 or positive)
			        			//If there is at least one sign change
			        				//Pixel is an edge pixel

	                		//IF current pixel is Negative
			        			//Check count of Positive neghbors
	                			//If count is more than one
			        				//Pixel is a background pixel

	                		if( (currentPixelSign >= 0) && (firstNeighborSign != currentNeighborSign) ){
	                			pixel_is_edge = true;
								break;	
	                		}

	                		firstNeighborSign *= currentNeighborSign;
	                	}   		
	                }

	                if(pixel_is_edge || pixel_is_background)
	                	break;
	            }

	            if(pixel_is_edge)
	            	resultEdgeMap[q][p][0] = 0;

	            if ( (currentPixelSign == -1) && (countOfPositiveNeighbors > 0) && (countOfPositiveNeighbors < 4) ) {
	    			resultEdgeMap[q][p][0] = 0;
	    		}

	    		if ( (countOfPositiveNeighbors == 0) || (countOfPositiveNeighbors == 4) || pixel_is_background ) {
	    			resultEdgeMap[q][p][0] = 255;
	    		}

	        }
	    }
	} 

	//Background check method
	//In method, we check if there is any positive or negative member
	//in the vicinity of a 0-valued pixel
	//If a pixel is found having non-zero neighbours, it is termed an edge
	//But if all a pixel has all neighbours of the same sign, it is not an edge
	else if(method == 2){

    	for(int q=0; q<outputHeight; q++){
	        for(int p=0; p<outputWidth; p++){
	            
	        	int countOfPositiveNeighbors = 0;
	        	int countOfNegativeNeighbors = 0;

	        	for(int j=0; j<kernelSize; j++){
	                for(int i=0; i<kernelSize; i++){

	                	//Window has 9 elements, counting them
	                	//linearly to check for 2, 4, 6 and 8
	                	//i.e. top, down, left and right
	                	int linearWindowIndex = (j*kernelSize) + i;

	                	if(linearWindowIndex%2 == 1){

	                		unsigned char currentNeighborValue = manipulatedImageVector[ j+q ][ i+p ][0];
	                		if(currentNeighborValue == 64)
	                			countOfNegativeNeighbors++;
				        	else if (currentNeighborValue == 192)
				        		countOfPositiveNeighbors++;
	                		
	                	}   		
	                }
	            }

	      		if ((countOfPositiveNeighbors > 0 && countOfPositiveNeighbors < 4) || (countOfNegativeNeighbors > 0 && countOfNegativeNeighbors < 4))
	            	resultEdgeMap[q][p][0] = 0;
	            else
	            	resultEdgeMap[q][p][0] = 255;


	        }
	    }
    }

    //Simple zero-crossing detector
    //If left != right -> pixel is edge
    //If top != bottom -> pixel is edge
    else {

    	for(int i=1; i<outputHeight; i++){
	        for(int j=1; j<outputWidth; j++){
	            
	        	bool pixel_is_edge = false;

	        	//Window has 9 elements, counting them
	        	//linearly to check for 1, 3, 5 and 7
	        	//i.e. top, down, left and right
	        	unsigned char currentPixel = manipulatedImageVector[i][j][0];

	        	if(currentPixel == 128){
	        		unsigned char currentLeftNeighbor = manipulatedImageVector[ i ][ j-1 ][0];
		        	unsigned char currentRightNeighbor = manipulatedImageVector[ i ][ j+1 ][0];
		        	unsigned char currentTopNeighbor = manipulatedImageVector[ i-1 ][ j ][0];
		        	unsigned char currentBottomNeighbor = manipulatedImageVector[ i+1 ][ j ][0];

		        	if( currentLeftNeighbor != currentRightNeighbor )
		        		pixel_is_edge = true;
		        	
		        	if( currentTopNeighbor != currentBottomNeighbor )
		        		pixel_is_edge = true;	
	        	}

				if (pixel_is_edge)
	            	resultEdgeMap[i][j][0] = 0;
	            else
	            	resultEdgeMap[i][j][0] = 255;

	        }
	    }
    }

	return resultEdgeMap;
}

//Get edge-map for an image using LoG-filter, gives a tuned output according to threshold, prints intermediate histogram if needed
vector<vector<vector<unsigned char>>> EdgeDetectionLoGFilter (vector<vector<vector<unsigned char>>> ImageVector, int threshold,
													 		  string outputFileName, bool writeHistogramToFile, int zeroCorssingmMethod){

	//Converting to grayscale
	ImageVector = GrayLuminosity(ImageVector);
	
	//Differentiating image to get edges
	double_differentiated_image	doubleDifferentiatedImageInfo = DoubleDifferentiateImageLoGFilter (ImageVector);
	
	//Create ternary edge map
	ImageVector = GetTernaryMap (doubleDifferentiatedImageInfo, threshold, outputFileName, writeHistogramToFile);

	//Get binary map by applying Zero-crossing
	ImageVector = GetZeroCrossingEdgeMap(ImageVector, zeroCorssingmMethod);

	return ImageVector;
}