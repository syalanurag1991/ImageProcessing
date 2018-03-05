///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////		COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <cstdio>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "FileIO.h"
#include "ChannelManipulation.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 4) || !((*argv[1] == '1') || (*argv[1] == '2') || (*argv[1] == '3')) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "Grayscale.exe <grayscale method> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available grayscale methods:\n" << "1-Lightness\n" << "2-Average\n" << "3-Luminosity\n";
		return 0;
	}
	
	string inputFileName  = argv[2];
	string outputFileName = argv[3];
	
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
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	if(*argv[1] == '1'){
		manipulatedImageVector = GrayLightness(ImageVector);	
	} else if(*argv[1] == '2'){
		manipulatedImageVector = GrayAverage(ImageVector);	
	} else {
		manipulatedImageVector = GrayLuminosity(ImageVector);
	}
	
	bool fileOuputOK = ImageOutput(outputFileName, manipulatedImageVector);
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////	
	
	if(fileOuputOK){
		cout << "Success!!\n";
	} else {
		cout << "Output file-name or the image is invalid...";
	}
	
	return 0;
}