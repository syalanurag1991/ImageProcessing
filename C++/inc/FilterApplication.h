#include <stdio.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>
#include <cfloat>

#include "BinaryImageOperations.h"

#ifndef __FILTERAPPLICATION_H__
#define __FILTERAPPLICATION_H__

using namespace std;

/////////////////////////////////////////////	FILTER APPLICATION	/////////////////////////////////////////////////////////////////
//Preparing an image for filter operations, like Gaussian Filter or Low-Pass filter
vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int height,
														  int width, int BytesPerPixel, int kernelSize);
vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize);

//Preparing a channel image for filter operations, like Gaussian Filter or Low-Pass filter - PASS BY REFERENCE
void FilterPreprocessing(vector<vector<vector<unsigned char>>> &ImageVector, int &kernelSize, 
                         vector<vector<vector<unsigned char>>> &manipulatedImageVector);

//Create a Gaussian filter kernel of given size
vector<vector<float>> CreateGaussianFilterKernel(int kernelSize, float sigma);

//Apply Gauss filter to a channel
vector<vector<vector<unsigned char>>> SingleChannelGaussFilter(vector<vector<vector<unsigned char>>> SingleChannelVector, vector<vector<float>> gaussKernel);

//Apply Median filter to a channel
vector<vector<vector<unsigned char>>> SingleChannelMedianFilter(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize);

//Replace the pixel value with it's most frequest neighbour (NxN)
vector<vector<vector<unsigned char>>> SingleChannelFrequentNeighborFilter(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize);

//Dither using Fixed, Random or Bayer thresholding methods
//bayerKernelSize doesn't matter for Fixed or Random methods
vector<vector<vector<unsigned char>>> SingleChannelDither (vector<vector<vector<unsigned char>>> SingleChannelVector, 
														   int method, int numberOfTones, int bayerMatrixNumber);

//Image is dithered using Bayer Matrices of kernel sizes 2, 4 and 8
vector<vector<vector<unsigned char>>> SingleChannelBayerFilter(vector<vector<vector<unsigned char>>> SingleChannelVector,
														 int numberOfTones, int bayerMatrixNumber);

//Image is dithered using error-diffusion - Floyd-Steinberg, JJN or Stucki
vector<vector<vector<unsigned char>>> SingleChannelErrorDiffusion(vector<vector<vector<unsigned char>>> SingleChannelVector,
                                                                  int numberOfTones, int diffusionMethod);

//Create Laws' Filter kernels
vector<vector<vector<float>>> CreateLawsFilter (int kernalDimension);

//Apply Laws' Filter
vector<vector<float>> FeaturePointsOfSingleRegionUsingSingleLawsFilter(vector<vector<vector<int>>> &SingleChannelVector, vector<vector<float>> &lawsKernel);

//Extract features from images using Laws' filters, this function expects pre-processed images
vector<vector<vector<vector<float>>>> FeaturePointsOfMultipleRegionsUsingMultipleLawsFilters (vector<vector<vector<vector<int>>>> CollectionOfBalancedImagePixelIntensities,
                                                                                              vector<vector<vector<float>>> lawsFilter);

//Apply Laws' Filter - PASS BY REFERENCE
void FastFeaturePointsOfSingleRegionUsingSingleLawsFilter(vector<vector<vector<int>>> &SingleChannelVector, vector<vector<float>> &lawsKernel,
                                                      vector<vector<float>> &resultFeatureVector);

//Differentiate in X or Y direction using Sobel-filter
vector<vector<vector<float>>> DifferentiateImageSobelFilterXY (vector<vector<vector<unsigned char>>> SingleChannelVector, unsigned char direction);

//Differentiate an image in both directions - Sobel-filter - Normalized output
vector<vector<vector<unsigned char>>> DifferentiateImageSobelFilter (vector<vector<vector<unsigned char>>> SingleChannelVector);

//Double-differentiate an image in both directions - LoG-filter - Normalized output to het values in unsigned char

//Defining type for collecting information on Double-differentiate image in one place
typedef struct double_differentiated_image {
	vector<vector<vector<unsigned char>>> NormalizedDoubleDifferentiatedImage;
	vector<vector<vector<float>>> DoubleDifferentiatedImage;
	float maxResultPixelValue;
    float minResultPixelValue;
    float avgResultPixelValue;
    float rangeResultPixelValue;
} double_differentiated_image;

double_differentiated_image DoubleDifferentiateImageLoGFilter (vector<vector<vector<unsigned char>>> SingleChannelVector);

//Remove speckles from output - 0 for black, 255 for white background
vector<vector<vector<unsigned char>>> SpeckleRemovalFilter (vector<vector<vector<unsigned char>>> manipulatedImageVector, unsigned char background);

#endif