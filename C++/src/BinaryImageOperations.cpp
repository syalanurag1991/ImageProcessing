///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryImageOperations.h"

using namespace std;

/////////////////////////////////////////////	BINARY IMAGE OPERATIONS	/////////////////////////////////////////////////////////////////
//Image Thresholding, image is converted to binary (pixel value = 255 or 0)
vector<vector<vector<unsigned char>>> ConvertToBinary(vector<vector<vector<unsigned char>>>ImageVector, int threshold){

	cout << "Converting to Binary ...\n\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	int white = 255, black = 0;;

	if(threshold<0){
		white = 0;
		black = 255;
		threshold = -1*threshold;
	}
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageRow;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			vector<unsigned char> newImageColumn;
			for(vector<unsigned char>::iterator itChannel = ImageColumn.begin(); itChannel!=ImageColumn.end(); itChannel++){
				if(*itChannel>threshold){
					newImageColumn.push_back(white);
				} else {
					newImageColumn.push_back(black);
				}
			}
			newImageRow.push_back(newImageColumn);
		}
		manipulatedImageVector.push_back(newImageRow);
	}
	
	return manipulatedImageVector;
}

//Binary Operations on Images
//AND, OR, XOR
vector<vector<vector<unsigned char>>> BinaryImageOperation(vector<vector<vector<unsigned char>>>ImageVector1,
					                                       vector<vector<vector<unsigned char>>>ImageVector2, type_binary_operation binOperation){
	
	int height1 = ImageVector1.size();
	int width1 = ImageVector1.front().size();
	int channels1 = ImageVector1.front().front().size();

	int height2 = ImageVector2.size();
	int width2 = ImageVector2.front().size();
	int channels2 = ImageVector2.front().front().size();

	if(!( (height1 == height2) && (width1 == width2) && (channels1 == channels2))){
		cout << "Input images have different dimensions and/or color channels\n";
		cout << "Results will be inaccurate!!\n\n";
	}

	int height = height1;
	int width = width1;
	int channels = channels1;

	/*
	cout << "Performing Binary Operation ";

	if(binOperation == AND){
		cout << "AND ... \n";
	} else if (binOperation == OR){
		cout << "OR ... \n";
	} else if (binOperation == XOR){
		cout << "XOR ...\n";
	}
	*/

	vector<unsigned char> newImageChannel(channels,0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>>manipulatedImageVector(height, newImageColumn);
	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<channels; k++){
				
				if(binOperation == AND){
					manipulatedImageVector[i][j][k] = ImageVector1[i][j][k] & ImageVector2[i][j][k];
				} else if (binOperation == OR){
					manipulatedImageVector[i][j][k] = ImageVector1[i][j][k] | ImageVector2[i][j][k];
				} else if (binOperation == XOR){
					manipulatedImageVector[i][j][k] = ImageVector1[i][j][k] ^ ImageVector2[i][j][k];
				}

			}
		}
	}

	return manipulatedImageVector;
}

//NOT
vector<vector<vector<unsigned char>>> BinaryImageOperation(vector<vector<vector<unsigned char>>>ImageVector, type_binary_operation binOperation){
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	//cout << "Performing Binary Operation NOT ... \n";

	vector<unsigned char> newImageChannel(channels,0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>>manipulatedImageVector(height, newImageColumn);
	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<channels; k++){
				manipulatedImageVector[i][j][k] = ~ImageVector[i][j][k];
			}
		}
	}

	return manipulatedImageVector;
}

//Returns a vector in which every byte represents the 8 neighboring pixels, window is always 3x3
vector<vector<vector<unsigned char>>> NeighborsInAByte (vector<vector<vector<unsigned char>>> ImageVector){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	int newWidth = width + 2;
	int newHeight = height + 2;

	//cout << "\n" << newWidth << " " << newHeight << " " << width << " " << height << "\n";

	//cout << "Packing neighbors of all pixels in bytes ... \n";

	vector<unsigned char> newImageChannel(channels, 0);
	vector<vector<unsigned char>> newImageColumn(newWidth, newImageChannel);
	vector<vector<vector<unsigned char>>>manipulatedImageVector(newHeight, newImageColumn);

	//Copying the raw image in to middle of the block
	for(int i=0; i<height; i++)
    	for(int j=0; j<width; j++)
    		for(int k=0; k<channels; k++)
    			manipulatedImageVector[i+1][j+1][k] = ImageVector[i][j][k];
    
    for(int i=1; i<newHeight-1; i++){
		for(int j=1; j<newWidth-1; j++){
			for(int k=0; k<channels; k++){



				unsigned char temp = 0b00000000;
				if(manipulatedImageVector[i][j+1][k]){
					temp |= 0b00000001;
				}

				if(manipulatedImageVector[i-1][j+1][k]){
					temp |= 0b00000010;
				}

				if(manipulatedImageVector[i-1][j][k]){
					temp |= 0b00000100;
				}

				if(manipulatedImageVector[i-1][j-1][k]){
					temp |= 0b00001000;
				}

				if(manipulatedImageVector[i][j-1][k]){
					temp |= 0b00010000;
				}

				if(manipulatedImageVector[i+1][j-1][k]){
					temp |= 0b00100000;
				}

				if(manipulatedImageVector[i+1][j][k]){
					temp |= 0b01000000;
				}

				if(manipulatedImageVector[i+1][j+1][k]){
					temp |= 0b10000000;
				}
				
				ImageVector[i-1][j-1][k] = temp;
			}
		}
	}

	return ImageVector;
}

//Marking Pixels by Morphological Filtering - Enter Stage number and Type
vector<vector<vector<unsigned char>>> PixelMarking (vector<vector<vector<unsigned char>>> ImageVector_bin,
																 		vector<vector<vector<unsigned char>>> neighborVector,
																 		int stage, type_morphing morphingOperation){
	//Input is a binary image, 0 and 255
	//Neighbors in a byte: Function will return an unisgned char value for every pixel, denoting nearby neighbors
	//LOOP: Run On ImageVector having Packed neighbours
		//Get bond value of everypixel: Pass Neighborhood byte
		//Get number of masks for a bond
		//LOOP: Get stage 1 mask for a bond
			//compare with neighborhood pack byte
			//If yes, mark pixel with 255, break loop for a match

	//cout << "Marking Pixels for erasure ... \n";

	int height = ImageVector_bin.size();
	int width = ImageVector_bin.front().size();
	int channels = ImageVector_bin.front().front().size();

	vector<unsigned char> newImageChannel(channels, 0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>> markForErase_bin(height, newImageColumn);
	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<channels; k++){

				if(ImageVector_bin[i][j][k]){
					int bond = find_bond_mask(neighborVector[i][j][k]);

					if((bond == 0) || (bond == 12)){
						continue;
					}

					int numberOfMasks = 0;
					if(stage==1){
						numberOfMasks = get_num_elements_for_stage1_bond(bond, morphingOperation);
					} else {
						numberOfMasks = get_num_elements_for_stage2_bond(bond, morphingOperation);
					}
					
					
					for(int index=0; index<numberOfMasks; index++){
							
						unsigned char mask = 0;					
						if(stage==1){
							mask = get_stage1_mask_for_bond(bond, index, morphingOperation);
						} else {
							mask = get_stage2_mask_for_bond(bond, index, morphingOperation);
						}
						
						if(mask == neighborVector[i][j][k]){
							markForErase_bin[i][j][k] = 255;
							break;
						}

					}	
				}

			}
		}
	}

	return markForErase_bin;
}

//Single Iteration Morphological Filtering - Input Binary image and Operation-type
vector<vector<vector<unsigned char>>> BinaryMorphologicalFilteringSingleIteration (vector<vector<vector<unsigned char>>> ImageVector_bin,
																 		 		   type_morphing morphingOperation){
	
	//Stage 1 - X and Mbar
	cout << "   Stage 1\n";
	vector<vector<vector<unsigned char>>> neighborVector = NeighborsInAByte (ImageVector_bin); //X
	vector<vector<vector<unsigned char>>> markForErase_bin = PixelMarking (ImageVector_bin, neighborVector, 1, morphingOperation); //M
	vector<vector<vector<unsigned char>>> doNotErase_bin = BinaryImageOperation(markForErase_bin, NOT); //Mbar
	vector<vector<vector<unsigned char>>> stage1_OutputImageVector_bin = BinaryImageOperation(ImageVector_bin, doNotErase_bin, AND); //X AND Mbar

	//Stage 2: X and PHit
	cout << "   Stage 2\n";
	neighborVector = NeighborsInAByte (markForErase_bin);
	doNotErase_bin = PixelMarking (markForErase_bin, neighborVector, 2, morphingOperation); //PHit
	vector<vector<vector<unsigned char>>> stage2_OutputImageVector_bin = BinaryImageOperation(ImageVector_bin, doNotErase_bin, AND); //X AND PHit

	//Output = X AND (Mbar OR PHit) = (X AND Mbar) OR (X AND PHit)
	vector<vector<vector<unsigned char>>> outputImageVector_bin	= BinaryImageOperation(stage1_OutputImageVector_bin, stage2_OutputImageVector_bin, OR);

	return outputImageVector_bin;
}

//Complete Morphological Filtering - Enter Input image, image threshold, number of iterations and operation-type
vector<vector<vector<unsigned char>>> BinaryMorphologicalFilteringComplete (vector<vector<vector<unsigned char>>> ImageVector, int threshold,
																			int numberOfIterations, type_morphing morphingOperation){

	//Convert to binary
	vector<vector<vector<unsigned char>>> ImageVector_bin = ConvertToBinary(ImageVector, threshold);

	bool processComplete = false;
	int iteration = 1;

	//Declaring previous and Current variables in a way so that loop runs at least once
	vector<vector<vector<unsigned char>>> currentIteration_bin = ImageVector_bin;
	vector<vector<vector<unsigned char>>> previousIteration_bin = BinaryImageOperation(currentIteration_bin, NOT);

	while( !(equal(previousIteration_bin.begin(), previousIteration_bin.end(), currentIteration_bin.begin())) && (!processComplete) ){
		
		cout << "Morphological filtering - Iteration #" << iteration << "\n";

		previousIteration_bin = currentIteration_bin;
		//Single iteration of Morphological Filtering
		currentIteration_bin = BinaryMorphologicalFilteringSingleIteration (previousIteration_bin, morphingOperation);

		if( (iteration == numberOfIterations) && (numberOfIterations > 0) ){
			processComplete = true;
		}
		iteration++;
		
	}

	//This is done to take care of forebackground being black
	if(threshold<0){
		currentIteration_bin = BinaryImageOperation(currentIteration_bin, NOT);
	}

	return currentIteration_bin;
}

//Converts all the disconnected objects into rectangles, fills cavities and corners, receives binary input
vector<vector<vector<unsigned char>>> BinaryGapFillAlgorithm_Rectangle(vector<vector<vector<unsigned char>>> ImageVector_bin){

	int height = ImageVector_bin.size();
    int width = ImageVector_bin.front().size();
    int channels = ImageVector_bin.front().front().size();

	//Declaring previous and Current variables in a way so that loop runs at least once
	vector<vector<vector<unsigned char>>> currentIteration_bin = ImageVector_bin;
	vector<vector<vector<unsigned char>>> previousIteration_bin = BinaryImageOperation(currentIteration_bin, NOT);

	int iteration = 1;
	while( !(equal(previousIteration_bin.begin(), previousIteration_bin.end(), currentIteration_bin.begin()))) {
		
		vector<vector<vector<unsigned char>>> neighborVector = NeighborsInAByte (currentIteration_bin);

		cout << "Filling gaps to nearest rectangle - Iteration #" << iteration << "\n";
		previousIteration_bin = currentIteration_bin;

		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				for(int k=0; k<channels; k++){
					
					if(currentIteration_bin[i][j][k] == 0){
						unsigned char neighborByte = neighborVector[i][j][k];
						if(    ( (neighborByte & 0x07) == 0x07) || ( (neighborByte & 0x70) == 0x70) 
							|| ( (neighborByte & 0x15) == 0x15) || ( (neighborByte & 0x51) == 0x51)
							|| ( (neighborByte & 0x45) == 0x45) || ( (neighborByte & 0x54) == 0x54)
							|| ( (neighborByte & 0x1C) == 0x1C) || ( (neighborByte & 0xC1) == 0xC1) ){

								currentIteration_bin[i][j][k] = 255;
						}
					}

				}
			}
		}

		iteration++;
	}

	return currentIteration_bin;
}


//Returns a 3D vector contaning the channel wise label data for binary images
//Also returns the maximum value of label used, which is inherently number of objects
type_labelData BinaryImageLabeling (vector<vector<vector<unsigned char>>> ImageVector_bin){
	
	type_labelData labelInformation;

	//Zero-initialize a Label Image Matrix
	int height = ImageVector_bin.size();
	int width = ImageVector_bin.front().size();
	int channels = ImageVector_bin.front().front().size();

	vector<unsigned char> newImageChannel(channels, 0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>> labelVector(height, newImageColumn);

	//Calculate neighbor vector - pack neighbors around a pixel in a byte
	vector<vector<vector<unsigned char>>> neighborVector = NeighborsInAByte(ImageVector_bin);
	int label = 0;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<channels; k++){

				unsigned char currentPixel = ImageVector_bin[i][j][k];
				unsigned char neighborByte = neighborVector[i][j][k];

				if(currentPixel > 0){
					if(neighborByte == 0xC1){
						//Top-left corner pixel is detected, assign a new label to it
						label++;
						labelVector[i][j][k] = label;
					} else if((neighborByte & 0x10) == 0x10){
						//Pixel has a neighbor in the left, take label from pixel in the left
						labelVector[i][j][k] = labelVector[i][j-1][k];
					} else if((neighborByte & 0x04) == 0x04){
						//Pixel has a neighbor above it, take label from pixel above it
						labelVector[i][j][k] = labelVector[i-1][j][k];
					}
				}

			}
		}
	}

	labelInformation.label = label;
	labelInformation.labelVector = labelVector;

	return labelInformation;
}

//Returns a collection of image object (3D vector)
vector<vector<vector<vector<unsigned char>>>> ExtractObjectsFromImage (vector<vector<vector<unsigned char>>> ImageVector_bin,
																	   type_labelData labelInformation){

	int height = ImageVector_bin.size();
	int width = ImageVector_bin.front().size();
	int channels = ImageVector_bin.front().front().size();

	vector<vector<vector<unsigned char>>> labelVector = labelInformation.labelVector;
	int label = labelInformation.label;

	vector<vector<vector<unsigned char>>> tempImageVector;
	vector<vector<vector<vector<unsigned char>>>> CollectionOfObjects (label, tempImageVector);

	//Search for the first non-zero label,this indicates start of a rectangle
		//Push all the channel values to a column vector
		//Subsequently push column vector to a row vector
	//If a 0 is encountered, means we reached the end of a rectangle
		//If the current row size>0
			//Push the row vector to an image object pertaining to that label
			//Empty the row vector, because there may be more non-zero labels in the row
	for(int i=0; i<height; i++){
		int lastPositiveLabel = 0;
		vector<vector<unsigned char>> newRow;
		for(int j=0; j<width; j++){
			int currentLabel = 0;
			vector<unsigned char> newColumn;
			for(int k=0; k<channels; k++){
				currentLabel = labelVector[i][j][k];
				if(currentLabel>0){
					lastPositiveLabel = labelVector[i][j][k];
					newColumn.push_back(ImageVector_bin[i][j][k]);
				}
			}

			if(currentLabel>0){
				newRow.push_back(newColumn);				
			} 

			if((newRow.size()>0) && (currentLabel==0)) {
				CollectionOfObjects.at(lastPositiveLabel-1).push_back(newRow);
				lastPositiveLabel = 0;
				newRow.clear();
			}
		}
	}

	return CollectionOfObjects;
}