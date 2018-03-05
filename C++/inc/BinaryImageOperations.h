///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>

#include "MorphologicalFilterTable.h"

#ifndef __BINARYIMAGEOPERATIONS_H__
#define __BINARYIMAGEOPERATIONS_H__

using namespace std;

/////////////////////////////////////////////	BINARY IMAGE OPERATIONS	/////////////////////////////////////////////////////////////////
//Image Thresholding, image is converted to binary (pixel value = 255 or 0)
vector<vector<vector<unsigned char>>> ConvertToBinary(vector<vector<vector<unsigned char>>>ImageVector, int threshold);

// Defining Type of Operations
typedef enum type_binary_operation {
	AND = 0x1,
	OR  = 0x2,
	NOT = 0x4,
	XOR = 0x8
} type_binary_operation;

typedef struct LabelVectorInformation {
	vector<vector<vector<unsigned char>>> labelVector;
	int label;
} type_labelData;


//Binary Operations on Images
//AND, OR
vector<vector<vector<unsigned char>>> BinaryImageOperation(vector<vector<vector<unsigned char>>>ImageVector1,
					                                       vector<vector<vector<unsigned char>>>ImageVector2, type_binary_operation binOperation);
//NOT
vector<vector<vector<unsigned char>>> BinaryImageOperation(vector<vector<vector<unsigned char>>>ImageVector, type_binary_operation binOperation);

//Returns a vector in which every byte represents the 8 neighboring pixels, window is always 3x3
vector<vector<vector<unsigned char>>> NeighborsInAByte (vector<vector<vector<unsigned char>>> ImageVector);

//Marking Pixels by Morphological Filtering - Enter Stage number and Type
vector<vector<vector<unsigned char>>> PixelMarking (vector<vector<vector<unsigned char>>> ImageVector_bin,
																 		vector<vector<vector<unsigned char>>> neighborVector,
																 		int stage, type_morphing morphingOperation);

//Single Iteration Morphological Filtering - Input Binary image and Operation-type
vector<vector<vector<unsigned char>>> BinaryMorphologicalFilteringSingleIteration (vector<vector<vector<unsigned char>>> ImageVector_bin,
																 		 type_morphing morphingOperation);

//Complete Morphological Filtering - Enter Input image, image threshold, number of iterations and operation-type
vector<vector<vector<unsigned char>>> BinaryMorphologicalFilteringComplete (vector<vector<vector<unsigned char>>> ImageVector, int theshold,
																			int numberOfIterations, type_morphing morphingOperation);

//Converts all the disconnected objects into rectangles, fills cavities and corners, receives binary input
vector<vector<vector<unsigned char>>> BinaryGapFillAlgorithm_Rectangle(vector<vector<vector<unsigned char>>> ImageVector_bin);

//Returns a 3D vector contaning the channel wise label data for binary images
//Also returns the maximum value of label used, which is inherently number of objects
type_labelData BinaryImageLabeling (vector<vector<vector<unsigned char>>> ImageVector_bin);

//Returns a collection of image object (3D vector)
vector<vector<vector<vector<unsigned char>>>> ExtractObjectsFromImage (vector<vector<vector<unsigned char>>> ImageVector_bin,
																	   type_labelData labelInformation);

#endif