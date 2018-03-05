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
#include "ImageManipulation.h"
#include "MorphologicalFilterTable.h"
#include "BinaryImageOperations.h"

//Returns similarity %age, Image 1 is the subject image, Image 2 is the test image
float CompareImages (vector<vector<vector<unsigned char>>> ImageVector1_bin, vector<vector<vector<unsigned char>>> ImageVector2_bin);

int main (int argc, char *argv[]) {
	/////////////////////////////////////////////		COMMON CODE		/////////////////////////////////////////////////////////////////
	if( (argc != 4) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "CountingGame.exe <Foreground> <input_filename.raw> <output_filename.raw>\n";
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
	int threshold = 1*foregroundFactor;
	vector<vector<vector<unsigned char>>> ImageVector_bin = ConvertToBinary(ImageVector, threshold);

	//Gap filling
	vector<vector<vector<unsigned char>>> gapFillImageVector_bin = BinaryGapFillAlgorithm_Rectangle(ImageVector_bin);
	
	//Labeling the Filled binary image
	cout << "\nLabeling objects in the image ...\n";
	type_labelData labelInformation = BinaryImageLabeling (gapFillImageVector_bin);

	//Extracting individual objects in input image
	cout << "\nExtracting objects in the image ...\n";
	//vector<vector<vector<vector<unsigned char>>>> CollectionOfObjects = ExtractObjectsFromImage (ImageVector, labelInformation);
	vector<vector<vector<vector<unsigned char>>>> CollectionOfObjects_bin = ExtractObjectsFromImage (ImageVector_bin, labelInformation);
	bool fileOuputOK_imageObject = true;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	int label = labelInformation.label;
	int uniqueObjects = label;
	for(int i=0; i<label; i++){
		
		vector<vector<vector<unsigned char>>> tempImageVector_bin = CollectionOfObjects_bin.at(i);
		int objectHeight = tempImageVector_bin.size();
		int objectWidth = tempImageVector_bin.front().size();
		int objectChannels = tempImageVector_bin.front().front().size();

		//This is done to take care of forebackground being black
		if(threshold<0){
			tempImageVector_bin = BinaryImageOperation(tempImageVector_bin, NOT);
		}

		string objectFilename = outputFileName;
		string extension = "_imageObject_";
		extension.append(to_string(i+1));
		extension.append("_width_");
		extension.append(to_string(objectWidth));
		extension.append("_height_");
		extension.append(to_string(objectHeight));
		extension.append(".raw");
		
		cout << "\nObject #" << i+1;
		cout << "\nPrinting to file\nWidth: " << objectWidth << " Height: " << objectHeight << "\n";

		objectFilename.replace(objectFilename.begin() + objectFilename.find(".raw"), objectFilename.end(), extension);
		fileOuputOK_imageObject &= ImageOutput(objectFilename, tempImageVector_bin);


		//Comparison loop

		//This is done to reverse the effect of earlier NOT
		if(threshold<0){
			tempImageVector_bin = BinaryImageOperation(tempImageVector_bin, NOT);
		}

		cout << "Matches for this object:\n\n   ";
		float similarityPercentage = 0;
		bool matchFound = false;
		for(int j=i+1; j<label; j++){
			similarityPercentage = 0;
			//Simple comparison - No change in orientation
			// A B
			// C D
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Horizontal flip
			// B A
			// D C
			tempImageVector_bin = HorizontalFlip(tempImageVector_bin);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));
			
			//Rotate by 90° (H-Flip of 270° Effectively)
			// A C
			// B D
			tempImageVector_bin = RotateImage (tempImageVector_bin, 90);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Horizontal flip (270° Effectively)
			// C A
			// D B
			tempImageVector_bin = HorizontalFlip(tempImageVector_bin);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Rotate by 180° (90° Effectively)
			// B D
			// A C
			tempImageVector_bin = RotateImage (tempImageVector_bin, 180);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Horizontal flip (H-Flip of 90° Effectively)
			// D B
			// C A
			tempImageVector_bin = HorizontalFlip(tempImageVector_bin);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Rotate by 270° (H-Flip of 180° Effectively)
			// C D
			// A B
			tempImageVector_bin = RotateImage (tempImageVector_bin, 270);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			//Horizontal flip (180° Effectively)
			// D C
			// B A
			tempImageVector_bin = HorizontalFlip(tempImageVector_bin);
			similarityPercentage = max(similarityPercentage, CompareImages (tempImageVector_bin, CollectionOfObjects_bin.at(j)));

			if(similarityPercentage > 97.0){
				cout << setw(4) << setprecision(4) << "Object #" << j+1 << " "<<similarityPercentage << "%\n   ";
				matchFound = true;
			}
		}

		if ( (similarityPercentage < 97.0) && !matchFound){
			cout << "No match\n";
		}

		if(matchFound){
			uniqueObjects--;
		}

		cout << "\n";

	}

	cout << "\n";
	cout << "Total number of objects : " << label << "\n";
	cout << "Total unique objects    : " << uniqueObjects << "\n";

	cout << "\n";
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	outputFileName.replace(outputFileName.begin() + outputFileName.find(".raw"), outputFileName.end(), "_label.raw");

	gapFillImageVector_bin = BinaryImageOperation(gapFillImageVector_bin, NOT);
	bool fileOuputOK_gapFill = ImageOutput(outputFileName, gapFillImageVector_bin);
	bool fileOuputOK_labelVector = ImageOutput(outputFileName, labelInformation.labelVector);
	
	cout << "\n";
	
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	
	if(fileOuputOK_imageObject && fileOuputOK_gapFill && fileOuputOK_labelVector){
	//if(fileOuputOK_main && fileOuputOK_Channels){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	
	return 0;
}

//Returns similarity %age, Image 1 is the subject image, Image 2 is the test image
float CompareImages (vector<vector<vector<unsigned char>>> ImageVector1, vector<vector<vector<unsigned char>>> ImageVector2){

	int height1 = ImageVector1.size();
	int width1 = ImageVector1.front().size();
	int channels1 = ImageVector1.front().front().size();

	int height2 = ImageVector2.size();
	int width2 = ImageVector2.front().size();
	int channels2 = ImageVector2.front().front().size();

	if(!( (height1 == height2) && (width1 == width2) && (channels1 == channels2))){
		return 0;
	}

	int totalPixels = height1*width1;
	int samePixels = 0;

	for(int i=0; i<height1; i++){
		for(int j=0; j<width1; j++){
			for(int k=0; k<channels1; k++){
				
				if(ImageVector1[i][j][k] == ImageVector2[i][j][k]){
					samePixels++;
				}

			}
		}
	}

	float similarityPercentage = ((float)samePixels*100)/(float)totalPixels;
	//cout << "Height 1: " << height1 << " Width1: " << width1 << "\n";
	//cout << "Height 2: " << height2 << " Width2: " << width2 << "\n";
	//cout << totalPixels << " " << samePixels << " " << similarityPercentage << "\n\n";
	return similarityPercentage;
}