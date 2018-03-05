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
#include "ChannelManipulation.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 3) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "RGBtoCMY.exe <input_filename.raw> <output_filename.raw>\n\n";
		return 0;
	}
	
	string inputFileName  = argv[1];
	string outputFileName = argv[2];
	
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
	
	vector<vector<vector<unsigned char>>>CMYImageVector = RGBtoCMY(ImageVector);
	bool fileOuputOK_main = ImageOutput(outputFileName, CMYImageVector);
	bool fileOuputOK_Channels = true;
	
	for(int i=0; i<3; i++){
		vector<vector<vector<unsigned char>>>SingleChannelVector = ExtractChannel(CMYImageVector, i);
		string extension, newfileName;
		newfileName = outputFileName;
		extension = "_channel_";
		if(i==0)
			extension.append("cyan");
		else if(i==1)
			extension.append("magenta");
		else
			extension.append("yellow");
		extension.append(".raw");
		newfileName.replace(newfileName.begin() + newfileName.find(".raw"), newfileName.end(), extension);
		bool fileOuputOK = ImageOutput(newfileName, SingleChannelVector);
		fileOuputOK_Channels = fileOuputOK_Channels & fileOuputOK;
	}
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "Success!!\n";
	} else {
		cout << "Output file-name or the image is invalid...";
	}
	
	return 0;
}