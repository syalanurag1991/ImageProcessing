///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////	COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

using namespace std;

#include "FileIO.h"
#include "ChannelManipulation.h"
#include "FilterApplication.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 5) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "OilPaintingEffect.exe <n-bit color> <N-for Nearest-Neighbour block size> <input_filename.raw> <output_filename.raw>\n\n";
		return 0;
	}
	
	string inputFileName  = argv[3];
	string outputFileName = argv[4];
	
	if(!FileOk(inputFileName)){
		cout << "Can't read file...";
		return 0;
	}
	
	int fileSize = FileSize(inputFileName);
	cout << "\nThe size of the file is: " << fileSize << "bytes\n";
	
	cout << "Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3\n";
	int height = 0, width = 0, BytesPerPixel = 1;
	cin >> width >> height >> BytesPerPixel;
	
	if(!( (height>0) && (width>0) && ((BytesPerPixel==1)||(BytesPerPixel==3)) ) ){
		cout << "INVALID INPUT!!\n";
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color>\n";
		return 0;
	}
	
	//Reading the image to a vector
	vector<vector<vector<unsigned char>>>ImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
	
	string nColorBits = argv[1];
	int colorBits = 0;
	bool bitsOK = true;
    
	string nearestNeighborKernelSize = argv[2];
    int kernelSize = 0;
	bool kernelSizeOK = true;
    

	//Checking valid color bits
	if(nColorBits.size()<=2){
		colorBits = stoi(nColorBits);
		if(!(colorBits>=3 && colorBits<=24 && colorBits%3==0)){
			bitsOK = false;	
		}
	} else {
		bitsOK = false;
	}
	
	//Checking block size
	if(nearestNeighborKernelSize.size()<=2){
		kernelSize = stoi(nearestNeighborKernelSize);
		if(!(kernelSize>=3 && kernelSize<=11 && kernelSize%2==1)){
			kernelSizeOK = false;
		}
	} else {
		kernelSizeOK = false;
	}
	
	//For grayscale images, number of color bits to be entered is 3*<number of color bits>
	//i.e. if we have an 8-bit grayscale image, enter 3*8 = 24
	//Hence number of always range 3-24
	if(!(bitsOK && kernelSize)){
		cout << "\nINVALID INPUT!!\n";
		cout << "Number of color bits is a multiple of 3 ranging from 3-24\n";
		cout << "Block-size is an odd number ranging from 3-11\n";
		return 0;	
	}
	
	//Quantizing the channels according
	//Preparing a channel image for filter operations, like Gaussian Filter or Low-Pass filter
	//Applying filters to channel vectors
	//Assimilating channels vectors to form final image

	int quantizeBy = 1 << (colorBits/3);
	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_quantize(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_freqNeighbor(BytesPerPixel, SingleChannelImageVector);

	for(int i=0; i<BytesPerPixel; i++){
		
		cout << "\n*** Working on Channel " << i << " *** \n";
		
		//Only quantization of channel
		CollectionOfChannels_quantize.at(i) = SingleChannelQuantizer(ImageVector, i, quantizeBy);

		//Only frequent neighbor filter on channel
		CollectionOfChannels_freqNeighbor.at(i) = ExtractChannel(ImageVector, i);
		CollectionOfChannels_freqNeighbor.at(i) = FilterPreprocessing(CollectionOfChannels_freqNeighbor.at(i), height, width, BytesPerPixel, kernelSize);
		CollectionOfChannels_freqNeighbor.at(i) = SingleChannelFrequentNeighborFilter(CollectionOfChannels_freqNeighbor.at(i), kernelSize);

		//Applying Oil Painting effect to channel
		CollectionOfChannels.at(i) = CollectionOfChannels_quantize.at(i);
		CollectionOfChannels.at(i) = FilterPreprocessing(CollectionOfChannels.at(i), height, width, BytesPerPixel, kernelSize);
		CollectionOfChannels.at(i) = SingleChannelFrequentNeighborFilter(CollectionOfChannels.at(i), kernelSize);
		cout << "\n"; 
		
	}

	cout << "\n"; 

	//Only quantized
	vector<vector<vector<unsigned char>>> resultImageVector_quantize = AssimilateChannels(CollectionOfChannels_quantize);
	string extension, newfileName_quantized;
	newfileName_quantized = outputFileName;
	extension = "_quantized.raw";
	newfileName_quantized.replace(newfileName_quantized.begin() + newfileName_quantized.find(".raw"), newfileName_quantized.end(), extension);
	bool fileOuputOK_quantize = ImageOutput(newfileName_quantized, resultImageVector_quantize);

	
	//Only frequent neighbor
	vector<vector<vector<unsigned char>>> resultImageVector_freqNeighbor = AssimilateChannels(CollectionOfChannels_freqNeighbor);
	string newfileName_freqNeighbor;
	newfileName_freqNeighbor = outputFileName;
	extension = "_freqNeighbor.raw";
	newfileName_freqNeighbor.replace(newfileName_freqNeighbor.begin() + newfileName_freqNeighbor.find(".raw"), newfileName_freqNeighbor.end(), extension);
	bool fileOuputOK_freqNeighbor = ImageOutput(newfileName_freqNeighbor, resultImageVector_freqNeighbor);

	//Oil painting
	vector<vector<vector<unsigned char>>> resultImageVector_main = AssimilateChannels(CollectionOfChannels);
	bool fileOuputOK_main = ImageOutput(outputFileName, resultImageVector_main);

	cout << "\n";

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main && fileOuputOK_quantize && fileOuputOK_freqNeighbor){
		cout << "Success!!\n";
	} else {
		cout << "Output file-name or the image is invalid...\n";
	}
	return 0;
}

