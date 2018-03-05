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
#include <ctime>

using namespace std;

#include "FileIO.h"
#include "FilterApplication.h"
#include "ChannelManipulation.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 6) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "Dithering.exe <Dithering Method> <Number of Tones/Gray Levels> <Kernel Size> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available dithering methods:\n" << "1 - Fixed\n" << "2 - Random\n" << "3 - Bayer Matrix\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Available kernel sizes for dithering: 2, 4 and 8\n\n";
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
	int method = 0;
	if(*argv[1] == '1'){
		method = 1;
		cout << "\nDithering by Fixed thresholding\n";
	} else if (*argv[1] == '2'){
		method = 2;
		cout << "\nDithering by Random thresholding\n";
	} else if (*argv[1] == '3'){
		method = 3;
		cout << "\nDithering using Bayer Matrices\n";
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "Dithering.exe <Dithering Method> <Number of Tones/Gray Levels> <Kernel Size> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available dithering methods:\n" << "1 - Fixed\n" << "2 - Random\n" << "3 - Bayer Matrix\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Available kernel sizes for dithering: 2, 4 and 8\n\n";
		return 0;
	}

	int numberOfTones = 0;
	if(*argv[2] == '2'){
		numberOfTones = 2;
		cout << "\nNumber of tones/gray levels = 2\n";
	} else if (*argv[2] == '4'){
		numberOfTones = 4;
		cout << "\nNumber of tones/gray levels = 4\n";
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "Dithering.exe <Dithering Method> <Number of Tones/Gray Levels> <Kernel Size> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available dithering methods:\n" << "1 - Fixed\n" << "2 - Random\n" << "3 - Bayer Matrix\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Available kernel sizes for dithering: 2, 4 and 8\n\n";
		return 0;
	}

	int bayerMatrixNumber = 0;


	if(method == 3){

		if(*argv[3] == '2'){
			bayerMatrixNumber = 2;
			cout << "\nBayer matrix kernel size = 2\n";
		} else if (*argv[3] == '4'){
			bayerMatrixNumber = 4;
			cout << "\nBayer matrix kernel size = 4\n";
		} else if (*argv[3] == '8'){
			bayerMatrixNumber = 8;
			cout << "\nBayer matrix kernel size = 8\n";
		} else {
			cout << "\nINVALID INPUT!!\n";
			cout << "Please use the command format given below:\n";
			cout << "Dithering.exe <Dithering Method> <Number of Tones/Gray Levels> <Kernel Size> <input_filename.raw> <output_filename.raw>\n\n";
			cout << "Available dithering methods:\n" << "1 - Fixed\n" << "2 - Random\n" << "3 - Bayer Matrix\n\n";
			cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
			cout << "Available kernel sizes for dithering: 2, 4 and 8\n\n";
			return 0;
		}

	}

	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);

	for(int i=0; i<BytesPerPixel; i++){
		
		CollectionOfChannels.at(i) = ExtractChannel(ImageVector, i);
		CollectionOfChannels.at(i) = SingleChannelDither (CollectionOfChannels.at(i), method, numberOfTones, bayerMatrixNumber);

	}

	vector<vector<vector<unsigned char>>> resultImageVector = AssimilateChannels(CollectionOfChannels);
	//vector<vector<vector<unsigned char>>> manipulatedImageVector = SingleChannelDither (ImageVector, method, numberOfTones, bayerMatrixNumber);

	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////	
	bool fileOuputOK = ImageOutput(outputFileName, resultImageVector);
	
	if(fileOuputOK){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "\nOutput file-name or the image is invalid...\n\n";
	}
	
	return 0;
}