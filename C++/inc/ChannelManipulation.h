///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#ifndef __CHANNELMANIPULATION_H__
#define __CHANNELMANIPULATION_H__

using namespace std;

/////////////////////////////////////////////	CHANNEL MANIPULATION	/////////////////////////////////////////////////////////////////
//Extracting channels
vector<vector<vector<unsigned char>>> ExtractChannel(vector<vector<vector<unsigned char>>>ImageVector, int channelNumber);

//Assimilating channels to form an 3-channel image (like RGB/CMY/YUV)
vector<vector<vector<unsigned char>>> AssimilateChannels(vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels);

//Quantize a channel, receives image, returns channel
vector<vector<vector<unsigned char>>> SingleChannelQuantizer(vector<vector<vector<unsigned char>>>ImageVector, int channelNumber, int quantizeBy);

//Manipulate channel by changing its histogram, i.e. normalizing histogram to range between values 0-255
vector<vector<vector<unsigned char>>> SetSingleChannelByHistogram(vector<vector<vector<unsigned char>>> SingleChannelVector, vector <int> histogramVector);

//Bucket-filling is used to redistribute pixels to achieve a flat histogram, color-depth measured in bits
vector<vector<vector<unsigned char>>> SingleChannelBucketFilling (vector<vector<vector<unsigned char>>> SingleChannelVector, vector<int> histogram);

//RGB to CMY
vector<vector<vector<unsigned char>>> RGBtoCMY(vector<vector<vector<unsigned char>>>ImageVector);

//Grayscale - Lightness Method
vector<vector<vector<unsigned char>>> GrayLightness(vector<vector<vector<unsigned char>>>ImageVector);

//Grayscale - Average Method
vector<vector<vector<unsigned char>>> GrayAverage(vector<vector<vector<unsigned char>>>ImageVector);

//Grayscale - Luminosity Method
vector<vector<vector<unsigned char>>> GrayLuminosity(vector<vector<vector<unsigned char>>>ImageVector);

#endif