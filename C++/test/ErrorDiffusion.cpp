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
		cout << "ErrorDiffusion.exe <Diffusion Method> <Number of Tones/Gray Levels> <-f/-uf> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available error diffusion methods:\n" << "1 - Floyd–Steinberg\n" << "2 - JJN\n" << "3 - Stucki\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Use -f to use median filter, else use -nf for no filtering\n\n";
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
	int kernelSize = 0;
	if(*argv[1] == '1'){
		method = 1;
		kernelSize = 3;
		cout << "\nError Diffusion by Floyd–Steinberg\n";
	} else if (*argv[1] == '2'){
		method = 2;
		kernelSize = 5;
		cout << "\nError Diffusion by JJN\n";
	} else if (*argv[1] == '3'){
		method = 3;
		kernelSize = 5;
		cout << "\nError Diffusion using Stucki\n";
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "ErrorDiffusion.exe <Diffusion Method> <Number of Tones/Gray Levels> <-f/-uf> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available error diffusion methods:\n" << "1 - Floyd–Steinberg\n" << "2 - JJN\n" << "3 - Stucki\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Use -f to use median filter, else use -nf for no filtering\n\n";
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
		cout << "ErrorDiffusion.exe <Diffusion Method> <Number of Tones/Gray Levels> <-f/-uf> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available error diffusion methods:\n" << "1 - Floyd–Steinberg\n" << "2 - JJN\n" << "3 - Stucki\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Use -f to use median filter, else use -nf for no filtering\n\n";
		return 0;
	}

	bool useFilter;
	string filter = argv[3];
	if(filter.compare("-f") == 0){
		useFilter = true;
	} else if(filter.compare("-nf") == 0) {
		useFilter = false;
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "ErrorDiffusion.exe <Diffusion Method> <Number of Tones/Gray Levels> <input_filename.raw> <output_filename.raw>\n\n";
		cout << "Available error diffusion methods:\n" << "1 - Floyd–Steinberg\n" << "2 - JJN\n" << "3 - Stucki\n\n";
		cout << "Available tones/gray levels:\n" << "2 - Dual Tone\n" << "4 - Quad Tone\n\n";
		cout << "Use -f to use median filter, else use -nf for no filtering\n\n";
		return 0;
	}

	cout << "\n";

	vector<vector<vector<unsigned char>>> SingleChannelImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels(BytesPerPixel, SingleChannelImageVector);

	for(int i=0; i<BytesPerPixel; i++){

		CollectionOfChannels.at(i) = ExtractChannel(ImageVector, i);
		SingleChannelImageVector = FilterPreprocessing(CollectionOfChannels.at(i), height, width, BytesPerPixel, kernelSize);
		CollectionOfChannels.at(i) = SingleChannelErrorDiffusion(SingleChannelImageVector, numberOfTones, method);
		
		if(useFilter){
			int medianKernelSize = 3;
			SingleChannelImageVector = FilterPreprocessing(CollectionOfChannels.at(i), height, width, BytesPerPixel, medianKernelSize);
			CollectionOfChannels.at(i) = SingleChannelMedianFilter(SingleChannelImageVector, medianKernelSize);	
		}
	
		cout << "\n\n";	
	}

	vector<vector<vector<unsigned char>>> resultImageVector = AssimilateChannels(CollectionOfChannels);
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////	
	bool fileOuputOK = ImageOutput(outputFileName, resultImageVector);
	
	if(fileOuputOK){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "\nOutput file-name or the image is invalid...\n\n";
	}
	
	return 0;
}

