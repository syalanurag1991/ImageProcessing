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

#ifndef __FILTERAPPLICATION_H__
#define __FILTERAPPLICATION_H__

using namespace std;

/////////////////////////////////////////////	FILTER APPLICATION	/////////////////////////////////////////////////////////////////
//Preparing an image for filter operations, like Gaussian Filter or Low-Pass filter
vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int height,
														  int width, int BytesPerPixel, int kernelSize);
vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize);

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

#endif