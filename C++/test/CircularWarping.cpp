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
	if( (argc != 4) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "CircularWarping.exe <-w/-d> <input_filename.raw> <output_filename.raw>\n";
		cout << "w-warp | d-dewarp\n";
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
	
	int targetHeight = 0;
	int targetWidth = 0;
	
	bool warpThis = true;
	string warpThisORnot = argv[1];
	if(warpThisORnot.compare("-w")==0){
		warpThis = true;
	} else if (warpThisORnot.compare("-d")==0){
		warpThis = false;
	} else {
		cout << "INVALID INPUT!!\n";
		cout << "Sample usage: CircularWarping.exe -w Castle.raw Castle_out.raw\n\n";
		return 0;
	}

	if(warpThis){
		cout << "\nTarget operation: Warp to Circle/Ellipse\n";
	} else {
		cout << "\nTarget operation: De-Warp to Rectangle\n";
	}
	
	cout << "\n";

	vector<vector<vector<unsigned char>>> resultImageVector = ImageWarp_EllipticalGrid(ImageVector, warpThis);
	bool fileOuputOK_main = ImageOutput(outputFileName, resultImageVector);

	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	return 0;
}