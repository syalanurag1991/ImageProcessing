///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////	COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "FileIO.h"
#include "MorphologicalFilterTable.h"
#include "BinaryImageOperations.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 6) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "MorphologicalFiltering.exe <Morphing operation> <Foreground> <threshold> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available Operations: 1-Shrinking 2-Thinning 3-Skeletonizing\n";
		cout << "Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground\n\n";
		return 0;
	}
	
	string inputFileName  = argv[4];
	string outputFileName = argv[5];
	
	if(!FileOk(inputFileName)){
		cout << "Can't read file...\n\n";
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
	
	cout << "\n";
	type_morphing morphingOperation = (type_morphing)stoi(argv[1]);
	if(morphingOperation == SHRINKING){
		cout << "Current operation: Shrinking\n\n";
	} else if(morphingOperation == THINNING){
		cout << "Current operation: Thinning\n\n";
	} else if(morphingOperation == SKELETONIZING){
		cout << "Current operation: Skeletonizing\n\n";
	} else {
		cout << "Option not available\n";
		cout << "Available Operations: 1-Shrinking 2-Thinning 3-Skeletonizing\n\n";
		return 0;
	}

	int foregroundFactor = 1;
	int foreground = stoi(argv[2]);
	if(foreground == 0){
		foregroundFactor = -1;
		cout << "Foreground: Black\n\n";
	} else if(foreground == 1){
		cout << "Foreground: White\n\n";
	} else {
		cout << "Option not available\n";
		cout << "Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground\n\n";
		return 0;
	}

	int threshold = stoi(argv[3]);
	cout << "Current threshold setting: " << threshold << "\n\n";
	if(threshold > 255 || threshold < 0){
		cout << "Option not available\n";
		cout << "0 <= Threshold <= 255\n\n";
	}
	
	threshold = threshold*foregroundFactor;
	int numberOfIterations = 0;
	vector<vector<vector<unsigned char>>> outputImageVector_bin = BinaryMorphologicalFilteringComplete(ImageVector, threshold, numberOfIterations,
																									   morphingOperation);
	string extension = "_";
	extension.append(argv[3]);
	extension.append(".raw");
	outputFileName.replace(outputFileName.begin() + outputFileName.find(".raw"), outputFileName.end(), extension);
	bool fileOuputOK_main = ImageOutput(outputFileName, outputImageVector_bin);
	
	cout << "\n";
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main){
	//if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	
	return 0;
}


