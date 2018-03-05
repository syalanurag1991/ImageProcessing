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
#include <climits>

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

#endif