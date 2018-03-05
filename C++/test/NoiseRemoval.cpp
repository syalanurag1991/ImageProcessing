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
#include <cmath>
#include <map>
#include <iomanip>

#include "FileIO.h"
#include "ChannelManipulation.h"
#include "ImageAnalysis.h"
#include "FilterApplication.h"

using namespace std;

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 5) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "NoiseRemoval.exe <N-for Filter block size> <original_filename.raw> <input_filename.raw> <output_filename.raw>\n\n";
		return 0;
	}
	
	string originalFilename = argv[2];
	string inputFileName  = argv[3];
	string outputFileName = argv[4];
	
	if(!(FileOk(originalFilename) && FileOk(inputFileName))) {
		cout << "Can't read file(s)...";
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
	
	//Reading the image be cleaned to a vector
	vector<vector<vector<unsigned char>>>originalImageVector = ImageInput(originalFilename, height, width, BytesPerPixel);

	//Reading the image be cleaned to a vector
	vector<vector<vector<unsigned char>>>ImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////

	string inputKernelSize = argv[1];
    int kernelSize = 0;
	bool kernelSizeOK = true;

	//Checking block size
	if(inputKernelSize.size()<=2){
		kernelSize = stoi(inputKernelSize);
		if(!(kernelSize>=3 && kernelSize<=11 && kernelSize%2==1)){
			kernelSizeOK = false;
		}
	} else {
		kernelSizeOK = false;
	}

	//Filter bloack size ranges from 3-11
	if(!kernelSize){
		cout << "\nINVALID INPUT!!\n";
		cout << "Block-size is an odd number ranging from 3-11\n";
		return 0;	
	}

	//MAX is defined as 255 here, the maximum value a pixel can take in a 8-but color system
	//Calculating Mean Square Difference and PSNR for Noisy image
	int MAX = 255;
	float msdNoisy = CalculateMSD(ImageVector, originalImageVector);
	float psnrNoisy = 10*log10 ((MAX*MAX)/msdNoisy);
	
	//Create a Gaussian filter kernel of given size
	//For now, sigma is taken as 1
	float sigma = 1;
	vector<vector<float>> gaussKernel = CreateGaussianFilterKernel(kernelSize, sigma);
	
	//Preparing a channel image for filter operations, like Gaussian Filter or Low-Pass filter
	//Applying filters to channel vectors
	//Assimilating channels vectors to form final image
	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_Gauss(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_Median(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_Original(BytesPerPixel, SingleChannelImageVector);
	

	//Preparing channels for noise removal
	for(int i=0; i<BytesPerPixel; i++){
		cout << "\n*** Working on Channel "<< i << " *** \n";
		CollectionOfChannels_Original.at(i) = ExtractChannel(originalImageVector, i);

		CollectionOfChannels.at(i) = ExtractChannel(ImageVector, i);
		//Calculating Mean Square Difference for Noisy image
		float msdNoisy = CalculateMSD(CollectionOfChannels.at(i), CollectionOfChannels_Original.at(i));
		
		CollectionOfChannels.at(i) = FilterPreprocessing(CollectionOfChannels.at(i), height, width, BytesPerPixel, kernelSize);
		
		//Applying filter to generate inidividual files demoing the filters
		CollectionOfChannels_Median.at(i) = SingleChannelMedianFilter(CollectionOfChannels.at(i), kernelSize);
		CollectionOfChannels_Gauss.at(i)  = SingleChannelGaussFilter(CollectionOfChannels.at(i), gaussKernel);

		//Calculating Mean Square Difference for Only-Gauss and Only-Median methods
		float msdMedian = CalculateMSD(CollectionOfChannels_Median.at(i), CollectionOfChannels_Original.at(i));
		float msdGauss = CalculateMSD(CollectionOfChannels_Gauss.at(i), CollectionOfChannels_Original.at(i));

		//Actual noise removal is this much only, the 2 lines below, Median followed by Gauss
		CollectionOfChannels.at(i) = FilterPreprocessing(CollectionOfChannels_Median.at(i), height, width, BytesPerPixel, kernelSize);
		CollectionOfChannels.at(i)  = SingleChannelGaussFilter(CollectionOfChannels.at(i), gaussKernel);

		//Calculating Mean Square Difference for Cascaded filter
		float msdNoiseRemoval = CalculateMSD(CollectionOfChannels.at(i), CollectionOfChannels_Original.at(i));

		int MAX = 255;
		float psnrNoisy = CalculatePSNR (msdNoisy, MAX);
		float psnrGauss = CalculatePSNR (msdGauss, MAX);
		float psnrMedian = CalculatePSNR (msdMedian, MAX);
		float psnrNoiseRemoval = CalculatePSNR (msdNoiseRemoval, MAX);

		cout << "\n";
		cout << "MSE in Noisy Image                    : " << setprecision(4) << msdNoisy << "\n";
		cout << "MSE with only Median filter           : " << setprecision(4) << msdMedian << "\n";
		cout << "MSE with only Gauss filter            : " << setprecision(4) << msdGauss << "\n";
		cout << "MSE after Noise-Removal (Cascaded)    : " << setprecision(4) << msdNoiseRemoval << "\n";

		cout << "\n";
		cout << "PSNR of Noisy Image                    : " << setprecision(4) << psnrNoisy << "dB\n";
		cout << "PSNR with only Median filter           : " << setprecision(4) << psnrMedian << "dB\n";
		cout << "PSNR with only Gauss filter            : " << setprecision(4) << psnrGauss << "dB\n";
		cout << "PSNR after Noise-Removal (Cascaded)    : " << setprecision(4) << psnrNoiseRemoval << "dB\n";
	}
		
	//For output
	//Separately printing Median-filtered, Gauss-filtered and both-filtered images
	vector<vector<vector<unsigned char>>> resultImageVector_Median, resultImageVector_Gauss, resultImageVector;
	resultImageVector_Median = AssimilateChannels(CollectionOfChannels_Median);
	resultImageVector_Gauss  = AssimilateChannels(CollectionOfChannels_Gauss);
	resultImageVector = AssimilateChannels(CollectionOfChannels);
	
	bool fileOuputOK_main = ImageOutput(outputFileName, resultImageVector);

	outputFileName.replace(outputFileName.begin() + outputFileName.find(".raw"), outputFileName.end(), "_Median.raw");
	bool fileOuputOK_median = ImageOutput(outputFileName, resultImageVector_Median);

	outputFileName.replace(outputFileName.begin() + outputFileName.find("_Median.raw"), outputFileName.end(), "_Gauss.raw");
	bool fileOuputOK_gauss = ImageOutput(outputFileName, resultImageVector_Gauss);

	//MAX is defined as 255 here, the maximum value a pixel can take in a 8-but color system
	float msdMedian = CalculateMSD(resultImageVector_Median, originalImageVector);
	float msdGauss = CalculateMSD(resultImageVector_Gauss, originalImageVector);
	float msdNoiseRemoval = CalculateMSD(resultImageVector, originalImageVector);

	float psnrGauss = 10*log10 ((MAX*MAX)/msdGauss);
	float psnrMedian = 10*log10((MAX*MAX)/msdMedian);
	float psnrNoiseRemoval = 10*log10 ((MAX*MAX)/msdNoiseRemoval);

	cout << "\n";
	cout << "MSE with of Noisy image               : " << setprecision(4) << msdNoisy << "\n";
	cout << "MSE with only Median filter           : " << setprecision(4) << msdMedian << "\n";
	cout << "MSE with only Gauss filter            : " << setprecision(4) << msdGauss << "\n";
	cout << "MSE after Noise-Removal (Cascaded)    : " << setprecision(4) << msdNoiseRemoval << "\n";

	cout << "\n";
	cout << "PSNR with only Noisy image             : " << setprecision(4) << psnrNoisy << "dB\n";
	cout << "PSNR with only Median filter           : " << setprecision(4) << psnrMedian << "dB\n";
	cout << "PSNR with only Gauss filter            : " << setprecision(4) << psnrGauss << "dB\n";
	cout << "PSNR after Noise-Removal (Cascaded)    : " << setprecision(4) << psnrNoiseRemoval << "dB\n";

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	//if(fileOuputOK_main && fileOuputOK_median && fileOuputOK_gauss){
	if(fileOuputOK_median){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "\nOutput file-name or the image is invalid...\n\n";
	}
	return 0;
}