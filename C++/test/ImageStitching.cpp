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
	if( (argc != 5) ) {
		cout << "\nINVALID INPUT!!\n";
		cout << "Please use the command format given below:\n";
		cout << "ImageStitch.exe <left_image.raw> <middle_image.raw> <right_image.raw> <output_filename.raw>\n\n";
		return 0;
	}
	
	string inputFileName1  = argv[1];
	string inputFileName2  = argv[2];
	string inputFileName3  = argv[3];
	string outputFileName = argv[4];

	bool FileNameOk = FileOk(inputFileName1);
	FileNameOk = FileNameOk & FileOk(inputFileName2);
	FileNameOk = FileNameOk & FileOk(inputFileName3);
	
	if(!FileNameOk){
		cout << "Can't read file(s)...\n\n";
		return 0;
	}
	
	int fileSize1 = FileSize(inputFileName1);
	int fileSize2 = FileSize(inputFileName2);
	int fileSize3 = FileSize(inputFileName3);
	cout << "\nThe size of the file is:\n";
	cout << fileSize1 << "bytes   " << fileSize2 << "bytes   " << fileSize3 << "bytes   " << "\n\n";
	
	cout << "All images must of the same sizes and number of channels\n";
	cout << "Enter image size as <height> <width> <1-grayscale/3-color>, e.g. 100 50 3\n";
	int height = 0, width = 0, BytesPerPixel = 1;
	cin >> width >> height >> BytesPerPixel;
	
	if(!( (height>0) && (width>0) && ((BytesPerPixel==1)||(BytesPerPixel==3)) ) ){
		cout << "INVALID INPUT!!\n";
		cout << "Height/Width cannot be 0 | Choose <1-grayscale/3-color>\n\n";
		return 0;
	}
	
	//Reading the image to a vector
	vector<vector<vector<unsigned char>>>ImageVector1 = ImageInput(inputFileName1, height, width, BytesPerPixel);
	vector<vector<vector<unsigned char>>>ImageVector2 = ImageInput(inputFileName2, height, width, BytesPerPixel);
	vector<vector<vector<unsigned char>>>ImageVector3 = ImageInput(inputFileName3, height, width, BytesPerPixel);
	cout << "\n";
	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE START		/////////////////////////////////////////////////////////////////
	int channels = BytesPerPixel;
	int blackBoardWidth  = width*5; 
	int blackBoardHeight = height*3;
	
	cout << "BlackBoard Width: " << blackBoardWidth << " BlackBoard Height: " << blackBoardHeight << "\n\n";

	int numberOfChannels = BytesPerPixel;

	vector<unsigned char> blackBoardChannel (numberOfChannels, 0);
	vector<vector<unsigned char>> blackBoardRow (blackBoardWidth, blackBoardChannel);
	vector<vector<vector<unsigned char>>> blackBoardImageVector (blackBoardHeight, blackBoardRow);

	//Defining the homography-inverse matrix
	//Left image is projected to middle
	vector<vector<float>> inverseHomography_left_to_middle = {
	    {0.505215, 0.005251, -284.408458},
        {-0.181230, 0.709900, -262.099172},
        {-0.000544, -0.000015, 1.308559}
   	};

   	vector<vector<float>> inverseHomography_right_to_middle = {
	    {1.11107, -0.0137683, -1280.095},
		{0.183995, 1.03559, -896.924},
		{0.00046821, -0.0000673658, 0.500662436}
   	};

   	cout << "\nPlacing middle image\n";
	//Placing the middle image at the middle of blackboard
	for(int i=height; i<2*height; i++){
		for(int j=2*width; j<3*width; j++){
			for(int k=0; k<numberOfChannels; k++){
				blackBoardImageVector[i][j][k] = ImageVector2[i-height][j-2*width][k];
			}
		}
	}

	//Placing left and right images on blackboard
	cout << "\nOverlaying left and right images\n";
	for(int i=0; i<blackBoardHeight; i++){
		for(int j=0; j<blackBoardWidth; j++){

				//Change from IJ space to XY space
				float x = (float)j + 0.5;
				float y = (float)blackBoardHeight - 0.5 - (float)i;

				//Convert to Homogeneous Coordinates
				vector<vector<float>> homogeneousXY = {
					{x},
					{y},
					{1}
				};

				//Apply Inverse Homography
				vector<vector<float>> newHomogeneousXY_left = MatrixMultiplication(inverseHomography_left_to_middle, homogeneousXY);
				vector<vector<float>> newHomogeneousXY_right = MatrixMultiplication(inverseHomography_right_to_middle, homogeneousXY);
				
				//Convert back to Cartesian Coordinates
				float newX_left = newHomogeneousXY_left[0][0]/newHomogeneousXY_left[2][0];
				float newY_left = newHomogeneousXY_left[1][0]/newHomogeneousXY_left[2][0];

				float newX_right = newHomogeneousXY_right[0][0]/newHomogeneousXY_right[2][0];
				float newY_right = newHomogeneousXY_right[1][0]/newHomogeneousXY_right[2][0];

				//Change from XY space to IJ space
				float newI_left_exact = (float)height - 0.5 - newY_left;
				float newI_left_nearest = roundf( newI_left_exact*100 )/100;
				int delta_left_newI = newI_left_exact - newI_left_nearest;
				int newI_left = (int)(newI_left_nearest);

				float newJ_Left_exact = newX_left - 0.5;
				float newJ_left_nearest = roundf( newJ_Left_exact*100 )/100;
				int delta_left_newJ = newJ_Left_exact - newJ_left_nearest;
				int newJ_left = (int)(newJ_left_nearest);

				float newI_right_exact = (float)height - 0.5 - newY_right;
				float newI_right_nearest = roundf( newI_right_exact*100 )/100;
				int delta_right_newI = newI_right_exact - newI_right_nearest;
				int newI_right = (int)(newI_right_nearest);

				float newJ_right_exact = newX_right - 0.5;
				float newJ_right_nearest = roundf( newJ_right_exact*100 )/100;
				int delta_right_newJ = newJ_right_exact - newJ_right_nearest;
				int newJ_right = (int)(newJ_right_nearest);

				int tempI_left = newI_left + 1;
				int tempJ_left = newJ_left + 1;
				int tempI_right = newI_right + 1;
				int tempJ_right = newJ_right + 1;

				if(tempI_left > height-1)
					tempI_left = height-1;

				if(tempJ_left > width-1)
					tempJ_left = width-1;

				if(tempI_right > height-1)
					tempI_right = height-1;

				if(tempJ_right > width-1)
					tempJ_right = width-1;

				//Looking back into left image
				for(int k=0; k<channels; k++){

					if( (newI_left >= 0) && (newJ_left >= 0) && (newI_left < height) && (newJ_left < width) ){
						
						unsigned char leftPixel = (unsigned char) ( delta_left_newI * (delta_left_newJ*ImageVector1[newI_left][newJ_left][k] + (1-delta_left_newJ)*ImageVector1[newI_left][tempJ_left][k]) +
						(1-delta_left_newI)*(delta_left_newJ*ImageVector1[tempI_left][newJ_left][k] + (1-delta_left_newJ)*ImageVector1[tempI_left][tempJ_left][k]) );

						//blackBoardImageVector[i][j][k] = leftPixel;

						if( i>=height && i<2*height && j>=2*width && j<3*width ){
							int pixelValue = (int)leftPixel + (int)blackBoardImageVector[i][j][k];
							pixelValue = pixelValue/2;
							blackBoardImageVector[i][j][k] = (unsigned char)pixelValue;
						} else {
							blackBoardImageVector[i][j][k] = leftPixel;							
						}

					}

					if( (newI_right >= 0) && (newJ_right >= 0) && (newI_right < height) && (newJ_right < width) ){
						
						unsigned char rightPixel = (unsigned char) ( delta_right_newI * (delta_right_newJ*ImageVector3[newI_right][newJ_right][k] + (1-delta_right_newJ)*ImageVector3[newI_right][tempJ_right][k]) +
						(1-delta_right_newI)*(delta_right_newJ*ImageVector3[tempI_right][newJ_right][k] + (1-delta_right_newJ)*ImageVector3[tempI_right][tempJ_right][k]) );	

						//blackBoardImageVector[i][j][k] = rightPixel;

						if( i>=height && i<2*height && j>=2*width && j<3*width ){
							int pixelValue = (int)rightPixel + (int)blackBoardImageVector[i][j][k];
							pixelValue = pixelValue/2;
							blackBoardImageVector[i][j][k] = (unsigned char)pixelValue;
						} else {
							blackBoardImageVector[i][j][k] = rightPixel;
						}

					}
					
					
				}

		}
	}

	/////////////////////////////////////////////		PROBLEM SPECIFIC CODE END		/////////////////////////////////////////////////////////////////
	cout << "\n";
	bool fileOuputOK_main = ImageOutput(outputFileName, blackBoardImageVector);

	if(fileOuputOK_main){
		cout << "Success!!\n\n";
	} else {
		cout << "Output file-name or the image is invalid...\n\n";
	}
	return 0;
}