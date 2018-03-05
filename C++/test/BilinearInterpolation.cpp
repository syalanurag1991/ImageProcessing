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
#include <cmath>
#include <iomanip>

#include "FileIO.h"
#include "ImageManipulation.h"

using namespace std;

int main (int argc, char *argv[]){
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 6) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BilinearInterpolation.exe <-s/-d> <target-Width/H-Scale> <target-Height/V-Scale> <input_filename.raw> <output_filename.raw>\n";
		return 0;
	}
	
	string inputFileName  = argv[4];
	string outputFileName = argv[5];
	
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
	
	int targetHeight = 0;
	int targetWidth = 0;
	
	string scalingMethod = argv[1];
	if(scalingMethod.compare("-s")==0){
		float scaleX = stof(argv[2]);
		float scaleY = stof(argv[3]);
		targetWidth = (int)((float)width*scaleX);
		targetHeight = (int)((float)height*scaleY);
	} else if (scalingMethod.compare("-d")==0){
		targetWidth = stoi(argv[2]);
		targetHeight = stoi(argv[3]);
	} else {
		cout << "INVALID INPUT!!\n";
		cout << "Sample usage: BilinearInterpolation.exe -d 800 600 Castle.raw Castle_out.raw\n\n";
		return 0;
	}

	cout << "Target Size is: " << targetWidth << " " << targetHeight << "\n";

	if(targetWidth==0 || targetHeight==0){
		cout << "INVALID INPUT!!\n";
		cout << "Target Height/ Target Width cannot be 0\n\n";
		return 0;	
	}

	cout << "\n"; 

	vector<vector<vector<unsigned char>>> resultImageVector = BilinearInterpolation(ImageVector, targetHeight, targetWidth);
	bool fileOuputOK_main = ImageOutput(outputFileName, resultImageVector);

	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	return 0;
}