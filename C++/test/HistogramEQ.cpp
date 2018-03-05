///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////		COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "FileIO.h"
#include "ChannelManipulation.h"
#include "ImageManipulation.h"
#include "ImageAnalysis.h"

#define SYMBOL_NUM 256

using namespace std;

int main(int argc, const char *argv[]) {
    /////////////////////////////////////////////	COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 5) || !((*argv[1] == '1') || (*argv[1] == '2')) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "HistogramEQ.exe <equalization method> <input_filename.raw> <output_filename.raw> <histogram_filename.csv>\n\n";
		cout << "Available HistogramEQ methods:\n" << "1-Transfer function (CDF)\n" << "2-Bucket Fill\n\n";
		return 0;
	}
	
	string inputFileName  = argv[2];
	string outputFileName_histogram = argv[4];
	string outputFileName_image = argv[3];
	
	if(!FileOk(inputFileName)){
		cout << "Can't read file...";
		return 0;
	}
	
	int fileSize = FileSize(inputFileName);
	cout << "\nThe size of the file is: " << fileSize << "bytes\n";
	
	cout << "Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3\n";
	int height = 0, width = 0, BytesPerPixel = 1;
	cin >> width >> height >> BytesPerPixel;
	
	if(!( (height>0) && (width>0) && ((BytesPerPixel==1)||(BytesPerPixel==3)) && (outputFileName_histogram.find("csv")<outputFileName_histogram.size()) ) ){
		cout << "\nINVALID INPUT!!\n";
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color> | Output histogram file should be a CSV\n\n";
		return 0;
	}
	
	//Reading the image to a vector
	vector<vector<vector<unsigned char>>>ImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);
    
    /////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
   
   	//Separating R, G abd B channels into separet vectors
	//Reading the the symbols from each channel
	//Storing the counts of the individual pixels in a histogram variable

	cout << "\n";

	bool method = true;
	if(*argv[1] == '2'){
		method = false;
		cout << "*** Histogram EQ using Method 2 - Bucket Fill ***\n\n";
	} else {
		cout << "*** Histogram EQ using Method 1 - Transfer function (CDF) ***\n\n";
	}

	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);

    bool fileOuputOK_histogram_Channels = true;
    for(int i=0; i<3; i++){
		
		//Preparing output file-names
    	string extension, newfileName, inputHistogramFileName;
    	inputHistogramFileName = outputFileName_histogram;
		newfileName = outputFileName_histogram;
		extension = "_channel_";
		if(i==0)
			extension.append("red");
		else if(i==1)
			extension.append("green");
		else
			extension.append("blue");

    	CollectionOfChannels.at(i) = ExtractChannel(ImageVector, i);
		
		vector<int> inputHistogramVector = GetHistogramSingleChannel(CollectionOfChannels.at(i), 0);
		inputHistogramFileName.replace(inputHistogramFileName.begin() + inputHistogramFileName.find(".csv"), inputHistogramFileName.end(), extension);
		inputHistogramFileName.append(".csv");
		bool fileOuputOK_histogram = PrintHistogramToFile(inputHistogramFileName, inputHistogramVector);
		
		inputHistogramFileName.replace(inputHistogramFileName.begin() + inputHistogramFileName.find(".csv"), inputHistogramFileName.end(), "_cumulative.csv");
		vector<int> inputHistogramVectorCDF = SingleChannelHistogramCDF (inputHistogramVector);
		fileOuputOK_histogram &= PrintHistogramToFile(inputHistogramFileName, inputHistogramVectorCDF);

		vector<int> newHistogramVector, newHistogramVectorCDF;
		string cumulativeHistogramFileName;
		cumulativeHistogramFileName = newfileName;
		cumulativeHistogramFileName.append(extension);

		if(method){
			newHistogramVector = inputHistogramVectorCDF;	
			CollectionOfChannels.at(i) = SetSingleChannelByHistogram(CollectionOfChannels.at(i), newHistogramVector);
			newHistogramVector = GetHistogramSingleChannel(CollectionOfChannels.at(i), 0);
			newHistogramVectorCDF = SingleChannelHistogramCDF (newHistogramVector);

			cumulativeHistogramFileName.append("_cumulative_cdf.csv");
			extension.append("_cdf.csv");
		} else {
			CollectionOfChannels.at(i) = SingleChannelBucketFilling (CollectionOfChannels.at(i), inputHistogramVector);
			newHistogramVector = GetHistogramSingleChannel(CollectionOfChannels.at(i), 0);
			newHistogramVectorCDF = SingleChannelHistogramCDF (newHistogramVector);
			
			cumulativeHistogramFileName.append("_cumulative_bucketfill.csv");
			extension.append("_bucketfill.csv");
		}
    	
		newfileName.replace(newfileName.begin() + newfileName.find(".csv"), newfileName.end(), extension);
		
		//Storing the histogram to a file
    	fileOuputOK_histogram &= PrintHistogramToFile(newfileName, newHistogramVector);
    	bool fileOuputOK_cumulativeHistogram = PrintHistogramToFile(cumulativeHistogramFileName, newHistogramVectorCDF);

    	fileOuputOK_histogram_Channels = fileOuputOK_histogram_Channels & fileOuputOK_histogram & fileOuputOK_cumulativeHistogram;
		cout << "\n";

	}

	vector<vector<vector<unsigned char>>> resultImageVector = AssimilateChannels(CollectionOfChannels);
	bool fileOuputOK_main = ImageOutput(outputFileName_image, resultImageVector);

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////    
    
    if(fileOuputOK_histogram_Channels && fileOuputOK_main){
		cout << "Success!!\n\n";
	} else {
		cout << "Image is invalid...\n\n";
	}
    
	return 0;
}