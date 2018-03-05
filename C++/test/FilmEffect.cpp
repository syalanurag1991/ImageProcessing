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
#include <climits>
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
	if(argc != 5){
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "FilmEffect.exe <target_filename.raw> <input_filename.raw> <output_filename.raw> <histogram_filename.csv>\n";
		cout << "Both target and input images must have the same width and height\n\n";
		return 0;
	}
	
	string targetFileName  = argv[1];
	string inputFileName  = argv[2];
	string outputFileName_histogram = argv[4];
	string outputFileName_image = argv[3];
	
	if( !(FileOk(inputFileName) && FileOk(inputFileName))) {
		cout << "Can't read target/input file...\n";
		return 0;
	}
	
	int fileSize1 = FileSize(targetFileName);
	cout << "\nThe size of the target file is: " << fileSize1 << "bytes";

	int fileSize2 = FileSize(inputFileName);
	cout << "\nThe size of the input file is : " << fileSize2 << "bytes\n\n";
	
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
	vector<vector<vector<unsigned char>>>targetImageVector = ImageInput(targetFileName, height, width, BytesPerPixel);
    
    /////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
   
   	//Separating R, G abd B channels into separet vectors
	//Reading the the symbols from each channel
	//Storing the counts of the individual pixels in a histogram variable

	cout << "\n";

	ImageVector = RGBtoCMY(ImageVector);
	ImageVector = HorizontalFlip(ImageVector);

	cout << "\n";

	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels_target(BytesPerPixel, SingleChannelImageVector);

    bool fileOuputOK_histogram_Channels = true;
    bool fileOuputOK_histogram_Channels_target = true;
    bool fileOuputOK_histogram_Channels_new = true;
    //for(int i=0; i<3; i++){
    for(int i=0; i<BytesPerPixel; i++){
		
		//Preparing output file-names
    	string extension, newfileName;
		newfileName = outputFileName_histogram;
		extension = "_channel_";
		if(i==0)
			extension.append("red");
		else if(i==1)
			extension.append("green");
		else
			extension.append("blue");
		extension.append(".csv");
		
		CollectionOfChannels.at(i) = ExtractChannel(ImageVector, i);
		CollectionOfChannels_target.at(i) = ExtractChannel(targetImageVector, i);
    	
    	vector<int> histogramVector = GetHistogramSingleChannel(CollectionOfChannels.at(i), 0);
		vector<int> histogramVector_target = GetHistogramSingleChannel(CollectionOfChannels_target.at(i), 0);

		//Storing the histogram to a file
		newfileName.replace(newfileName.begin() + newfileName.find(".csv"), newfileName.end(), extension);
		bool fileOuputOK_histogram = PrintHistogramToFile(newfileName, histogramVector);
		newfileName.replace(newfileName.begin() + newfileName.find(".csv"), newfileName.end(), "_target.csv");
		bool fileOuputOK_histogram_target = PrintHistogramToFile(newfileName, histogramVector_target);

		vector<int> cdfHistogramVector = SingleChannelHistogramCDF(histogramVector);
		vector<int> cdfHistogramVector_target = SingleChannelHistogramCDF (histogramVector_target);

		//Histogram matching and printing new histogram in a file
		CollectionOfChannels.at(i) = SingleChannelHistogramMatching(CollectionOfChannels.at(i), cdfHistogramVector_target, cdfHistogramVector);
		vector<int> newHistogramVector = GetHistogramSingleChannel(CollectionOfChannels.at(i), 0);
		newfileName.replace(newfileName.begin() + newfileName.find("_target.csv"), newfileName.end(), "_new.csv");
		bool fileOuputOK_histogram_new = PrintHistogramToFile(newfileName, newHistogramVector);

		fileOuputOK_histogram_Channels = fileOuputOK_histogram_Channels & fileOuputOK_histogram;
		fileOuputOK_histogram_Channels_target = fileOuputOK_histogram_Channels_target & fileOuputOK_histogram_target;
    	fileOuputOK_histogram_Channels_new = fileOuputOK_histogram_Channels_new & fileOuputOK_histogram_new;

		cout << "\n";

	}

	vector<vector<vector<unsigned char>>> resultImageVector = AssimilateChannels(CollectionOfChannels);
	bool fileOuputOK_main = ImageOutput(outputFileName_image, resultImageVector);

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////    
    
    if(fileOuputOK_histogram_Channels && fileOuputOK_histogram_Channels_target && fileOuputOK_histogram_Channels_new && fileOuputOK_main){
		cout << "Success!!\n\n";
	} else {
		cout << "Image is invalid...\n\n";
	}
    
	return 0;
}