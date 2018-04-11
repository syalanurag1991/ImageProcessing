#include <stdio.h>
#include <cstdio>
#include <vector>
#include <array>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>
#include <climits>
#include <cfloat>
#include <cstring>

#include "FilterApplication.h"
#include "ChannelManipulation.h"

#ifndef __IMAGEANALYSIS_H__
#define __IMAGEANALYSIS_H__

using namespace std;

/////////////////////////////////////////////	IMAGE ANALYSIS	/////////////////////////////////////////////////////////////////
//Returns the histogram of a single channel in a vector
vector <int> GetHistogramSingleChannel(vector<vector<vector<unsigned char>>> ImageVector, int channelNumber);

//Print Histogram to a file
bool PrintHistogramToFile(string fileName, vector <int> histogramVector);

// Manipulating Histogram - Cumulative Probability Distribution
vector<int> SingleChannelHistogramCDF (vector<int> singleChannelHistogram);

//Match histogram of a channel with a target histogram
vector<vector<vector<unsigned char>>> SingleChannelHistogramMatching(vector<vector<vector<unsigned char>>> SingleChannelVector,
																	 vector<int> targetHistogram, vector<int> subjectHistogram);

//Return Mean-Square Difference (Error) between two images
float CalculateMSD(vector<vector<vector<unsigned char>>>outputImageVector, vector<vector<vector<unsigned char>>>ImageVector);

//Return PSNR
float CalculatePSNR(float meanSquareDifference, int MAX);


/////////////////////////////////////////////	TEXTURE ANALYSIS	/////////////////////////////////////////////////////////////////
//Defining type for collecting information on centroids in one place
typedef struct centroid_info {
	vector<vector<float>> CollectionOfCentroids;
	vector<int> indexesOfSelectedCentroids;
} centroid_info;

//Calculate average energy of a group of regions: Energy is calculated pixel-wise, summed over and divided by total number of pixels
vector<vector<float>> CalculateAverageEnergyOfRegions (vector<vector<vector<vector<float>>>> CollectionOfFilterOutputs_allImages);

//Calculate average energy of a SINGLE region: Energy is calculated pixel-wise, summed over and divided by total number of pixels - PASS BY REFERENCE
void CalculateAverageEnergySingleRegion (vector<vector<vector<float>>> &CollectionOfFilterOutputs_singleRegion,
										 vector<float> &featurePointsSingleRegion);


//Calculate average of feature points from all images
vector<float> CalculateAverageOfAllFeaturePoints (vector<vector<float>> featurePointsForAllImages);

//Calculate standard deviation of feature points from all images
vector<float> CalculateStdDevOfAllFeaturePoints (vector<vector<float>> featurePointsForAllImages, vector<float> averageFeatureVector);

//K++ initialization, first centroid must be passed and can be chosen randomly or manually, random selction is not implemented
centroid_info KMeansPlusPlusInitlization (vector<vector<float>> featurePointsForAllImages, int numberOfClusters, int firstCentoidIndex);

//Triangular Initialization
centroid_info TriangularInitlization (vector<vector<float>> featurePointsForAllImages, int numberOfClusters);

// K-Means clustering
vector<int> KMeansClustering (vector<vector<float>> featurePointsForAllImages, vector<vector<float>> CollectionOfInitialCentroids);

//Find average intensity of a group of regions
vector<float> CalculateAverageRegionIntensity (vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_input);

//Find average intensity of a single region
float CalculateAverageRegionIntensitySingleRegion (vector<vector<vector<unsigned char>>> &singleImageVector);

//Subtract mean intensity to subjugate illumination effects - For a group of regions
vector<vector<vector<vector<int>>>> SubtractMeanPixelIntensity (vector<vector<vector<vector<unsigned char>>>> CollectionOfImageVectors_intermediate,
																vector<float> averageRegionIntensity);

//Subtract mean intensity to subjugate illumination effects - For a single region (PASS BY REFERENCE)
void SubtractMeanPixelIntensitySingleRegion (vector<vector<vector<unsigned char>>> &ImageVector_intermediate, float &averageRegionIntensity,
											 vector<vector<vector<int>>> &balancedImagePixelIntensities);

//Normalizing Features-vector using Average-feature vector and standard deviation
vector<vector<float>> NormalizeAllFeaturePoints (vector<vector<float>> featurePointsForAllImages, vector<float> averageFeatureVector,
												 vector<float> standardDeviationVector);

//Get edge-map for an image using Sobel-filter, gives a tuned output according to threshold
vector<vector<vector<unsigned char>>> EdgeDetectionSobelFilter (vector<vector<vector<unsigned char>>> ImageVector, int threshold);

//Intermidate step of edge detection using LoG, gets a ternary edge map (-1 0 1) to be used for zero crossing later
//Uses histogram of output from LoG to decide pixels belongin to background and pixel that contribute to edges
//Pass empty string if histogram is not required
vector<vector<vector<unsigned char>>> GetTernaryMap (double_differentiated_image doubledifferentiatedImageInfo, int threshold,
													 string outputFileName, bool writeHistogramToFile);

//Detect edges using Zero-crossing morphological (4-connected) filters, final step of edge-detection using LoG
vector<vector<vector<unsigned char>>> GetZeroCrossingEdgeMap (vector<vector<vector<unsigned char>>> manipulatedImageVector, int method);

//Get edge-map for an image using LoG-filter, gives a tuned output according to threshold, prints intermediate histogram if needed
vector<vector<vector<unsigned char>>> EdgeDetectionLoGFilter (vector<vector<vector<unsigned char>>> ImageVector, int threshold,
													 		  string outputFileName, bool writeHistogramToFile, int zeroCorssingmMethod);

#endif