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
	if( (argc != 4) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "ObjectsInImage.exe <Foreground> <input_filename.raw> <output_filename.raw>\n";
		cout << "Foreground: 0-Black 1-White, Background (set automatically) = Opposite of Foreground\n\n";
		return 0;
	}
	
	string inputFileName  = argv[2];
	string outputFileName = argv[3];
	
	if(!FileOk(inputFileName)){
		cout << "\nCan't read file...\n";
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

	int foregroundFactor = 1;
	int foreground = stoi(argv[1]);
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

	//Convert to binary
	int threshold = 128*foregroundFactor;
	vector<vector<vector<unsigned char>>> ImageVector_bin = ConvertToBinary(ImageVector, threshold);

	//Declaring previous and Current variables in a way so that loop runs at least once
	vector<vector<vector<unsigned char>>> currentIteration_bin = ImageVector_bin;
	vector<vector<vector<unsigned char>>> previousIteration_bin = BinaryImageOperation(currentIteration_bin, NOT);

	//Initialized with first member as 0 for finding difference between successive counts
	//Every iteration will count single pixels from previous iterations as well
	//To avoid another loop to take a sum of objectCount, keep updating countTillCurrentIteration
	//Total number of objects  = countTillCurrentIteration
	int iteration = 1;
	vector<int> objectCount;
	vector<int> objectSizeCount;
	int countTillCurrentIteration = 0, countTillPreviousIteration = 0;

	while( !(equal(previousIteration_bin.begin(), previousIteration_bin.end(), currentIteration_bin.begin()))) {
		
		cout << "\n\nCounting object - Iteration #" << iteration << "\n";

		//Count single pixels in image
		int objectCountSingleIteration = 0;
		vector<vector<vector<unsigned char>>> neighborVector = NeighborsInAByte (currentIteration_bin);

		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				for(int k=0; k<BytesPerPixel; k++){
					if((neighborVector[i][j][k] == 0) && (currentIteration_bin[i][j][k] > 0)){
						objectCountSingleIteration++;
					}
				}
			}
		}

		cout << "\nStats:\n";
		//cout << "Count till previous iteration: " << countTillPreviousIteration << "\n";
		//cout << "Count till current iteration: " << countTillCurrentIteration << "\n";
		 
		countTillPreviousIteration = countTillCurrentIteration;
		countTillCurrentIteration = objectCountSingleIteration;

		int differenceWithPrevious = countTillCurrentIteration - countTillPreviousIteration;
		objectCount.push_back(differenceWithPrevious);

		int starSize = 0;
		if(differenceWithPrevious > 0){
			starSize = (float)iteration*1.5833;
			objectSizeCount.push_back(starSize);	
		}

		cout << "Stars counted in this iteration: " << differenceWithPrevious << "\n";
		cout << "Star size found for iteration  : " << starSize << "\n";

		previousIteration_bin = currentIteration_bin;
		//Single iteration of Morphological Filtering
		currentIteration_bin = BinaryMorphologicalFilteringSingleIteration (previousIteration_bin, SHRINKING);
		iteration++;
	}

	//This is done to take care of forebackground being black
	if(threshold<0){
		currentIteration_bin = BinaryImageOperation(currentIteration_bin, NOT);
	}

	cout << "\n";
	cout << "Total number of stars    : " << countTillCurrentIteration << "\n";
	int numberOfDifferentSizes = 0;
	for(vector<int>::iterator it = objectCount.begin()+1; it!=objectCount.end(); it++){
		if(*it>0){
			numberOfDifferentSizes++;
		}
	}

	cout << "Number of different sizes: " << numberOfDifferentSizes << "\n";
	cout << "Distribution of sizes    : \n";
	for(vector<int>::iterator it = objectSizeCount.begin(); it!=objectSizeCount.end(); it++){
		if(*it>0){
			cout << setw(4) << *it;
		}
	}
	cout << "\n";
	for(vector<int>::iterator it = objectCount.begin(); it!=objectCount.end(); it++){
		if(*it>0){
			cout << setw(4) << *it;
		}
	}

	vector<vector<vector<unsigned char>>> outputImageVector_bin = currentIteration_bin;
	bool fileOuputOK_main = ImageOutput(outputFileName, outputImageVector_bin);
	
	cout << "\n\n";
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_main){
	//if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	
	return 0;
}
