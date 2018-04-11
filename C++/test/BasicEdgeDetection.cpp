///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////		COMMON CODE	START	/////////////////////////////////////////////////////////////////
#include <cstdio>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "FileIO.h"
#include "ImageAnalysis.h"
#include "FilterApplication.h"
#include "ChannelManipulation.h"
#include "BinaryImageOperations.h"

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if(argc != 7) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available detection methods : 1-Sobel | 2-LoG" << endl;
		cout << "Print histogram             : -h to print histogram | -wh for no histogram" << endl;
		cout << "Denoise using Gauss kernel  : -d to denoise  | -wd for no denoising" << endl;
		cout << "0 <= Threshold <= 100" << endl << endl;
		return 0;
	}

	int method = stoi(argv[1]);
	if(method != 1 && method != 2){
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available detection methods : 1-Sobel | 2-LoG" << endl;
		cout << "Print histogram             : -h to print histogram | -wh for no histogram" << endl;
		cout << "Denoise using Gauss kernel  : -d to denoise  | -wd for no denoising" << endl;
		cout << "0 <= Threshold <= 100" << endl << endl;
		return 0;
	}

	int threshold = stoi(argv[2]);
	string percentage = argv[2];
	if(threshold < 0 || threshold > 100){
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available detection methods : 1-Sobel | 2-LoG" << endl;
		cout << "Print histogram             : -h to print histogram | -wh for no histogram" << endl;
		cout << "Denoise using Gauss kernel  : -d to denoise  | -wd for no denoising" << endl;
		cout << "0 <= Threshold <= 100" << endl << endl;
		return 0;
	}

	bool writeHistogramToFile;
	string histogramRequired = argv[3];
	if(histogramRequired.compare("-h") == 0){
		writeHistogramToFile = true;
	} else if(histogramRequired.compare("-wh") == 0){
		writeHistogramToFile = false;
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available detection methods : 1-Sobel | 2-LoG" << endl;
		cout << "Print histogram             : -h to print histogram | -wh for no histogram" << endl;
		cout << "Denoise using Gauss kernel  : -d to denoise  | -wd for no denoising" << endl;
		cout << "0 <= Threshold <= 100" << endl << endl;
		return 0;	
	}

	bool denoise;
	string denoiseRequired = argv[4];
	if(denoiseRequired.compare("-d") == 0){
		denoise = true;
	} else if(denoiseRequired.compare("-wd") == 0){
		denoise = false;
	} else {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "BasicEdgeDetection <detection filter> <threshold percent (integer)> <print histogram> <denoise> <input_filename.raw> <output_filename.raw>\n";
		cout << "Available detection methods : 1-Sobel | 2-LoG" << endl;
		cout << "Print histogram             : -h to print histogram | -wh for no histogram" << endl;
		cout << "Denoise using Gauss kernel  : -d to denoise  | -wd for no denoising" << endl;
		cout << "0 <= Threshold <= 100" << endl << endl;
		return 0;	
	}	

	string inputFileName  = argv[5];
	string outputFileName = argv[6];
	
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
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color>\n\n";
		return 0;
	}
	
	//Reading the image to a vector
	vector<vector<vector<unsigned char>>>ImageVector = ImageInput(inputFileName, height, width, BytesPerPixel);
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
	
	//Converting to grayscale
	vector<vector<vector<unsigned char>>> manipulatedImageVector = GrayLuminosity(ImageVector);
	string tempFilename = outputFileName;
	tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), "_gray.raw");
	bool fileOuputOK = ImageOutput(tempFilename, manipulatedImageVector);

	//Denoising
	if(denoise){
		int denoiseKernelSize = 3;
		//vector<vector<float>> gaussKernel = CreateGaussianFilterKernel(denoiseKernelSize, 1);
		vector<vector<vector<unsigned char>>> tempImageVector = FilterPreprocessing(manipulatedImageVector, denoiseKernelSize);
		vector<vector<float>> gaussKernel = {
			{1/16.0, -2/16.0, 1/16.0},
			{-2/16.0, 4/16.0, -2/16.0},
			{1/16.0, -2/16.0, 1/16.0},
		};
		manipulatedImageVector = SingleChannelGaussFilter(tempImageVector, gaussKernel);
		outputFileName.replace(outputFileName.begin() + outputFileName.find(".raw"), outputFileName.end(), "_denoised.raw");
		tempFilename = outputFileName;
		cout << outputFileName << endl;
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);	
	}
	
	if(method == 1){

		//Differentiating image to get edges
		manipulatedImageVector = DifferentiateImageSobelFilter (manipulatedImageVector);
		tempFilename = outputFileName;
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), "_differentiated.raw");
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);	
		
		//Negating image to get edge map
		manipulatedImageVector = RGBtoCMY (manipulatedImageVector);
		tempFilename = outputFileName;
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), "_edge_map_no_threshold.raw");
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

		//Get edge-map for an image using Sobel-filter
		manipulatedImageVector = EdgeDetectionSobelFilter (ImageVector, threshold);
		tempFilename = outputFileName;
		string extension = "_binary_edge_map_threshold_";
		extension.append(percentage);
		extension.append(".raw");
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), extension);
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

	} else {

		//Double-differentiating image to get edges
		double_differentiated_image doubleDifferentiatedImageInfo = DoubleDifferentiateImageLoGFilter (manipulatedImageVector);
		manipulatedImageVector = doubleDifferentiatedImageInfo.NormalizedDoubleDifferentiatedImage;
		tempFilename = outputFileName;
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), "_double_differentiated.raw");
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

		//Create ternary edge map
		manipulatedImageVector = GetTernaryMap (doubleDifferentiatedImageInfo, threshold, "", false);
		tempFilename = outputFileName;
		string extension = "_ternary_edge_map_threshold_";
		extension.append(percentage);
		extension.append(".raw");
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), extension);
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

		//Get edge-map for an image using LoG-filter (Zero-Crossing implemented inside function)
		int zeroCrossingMethod = 3;
	    manipulatedImageVector = EdgeDetectionLoGFilter (ImageVector, threshold, outputFileName, writeHistogramToFile, zeroCrossingMethod);
	    tempFilename = outputFileName;
		extension = "_binary_edge_map_threshold_";
		extension.append(percentage);
		extension.append(".raw");
		tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), extension);
		fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

	}

	//Remove speckles from output
	int background = 255;
	int numberOfIterations = 0;
	type_morphing morphingOperation = THINNING;
	//manipulatedImageVector = SpeckleRemovalFilter (manipulatedImageVector, background);
	manipulatedImageVector = BinaryMorphologicalFilteringComplete (manipulatedImageVector, -100, numberOfIterations, morphingOperation);
	tempFilename = outputFileName;
	string extension = "_binary_edge_map_threshold_clean_";
	extension.append(percentage);
	extension.append(".raw");
	tempFilename.replace(tempFilename.begin() + tempFilename.find(".raw"), tempFilename.end(), extension);
	fileOuputOK = fileOuputOK & ImageOutput(tempFilename, manipulatedImageVector);

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////	
	
	if(fileOuputOK){
		cout << "\nSuccess!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	
	return 0;
}
