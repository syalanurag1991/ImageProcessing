///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ChannelManipulation.h"

using namespace std;

//Extracting channels
vector<vector<vector<unsigned char>>> ExtractChannel(vector<vector<vector<unsigned char>>>ImageVector, int channelNumber){
	
	cout << "Extracting channel " << channelNumber << " ...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageRow;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			vector<unsigned char> newImageColumn;
			
			unsigned char temp;
			temp = ImageColumn.at(channelNumber);
			newImageColumn.push_back(temp);
			
			newImageRow.push_back(newImageColumn);
		}
		manipulatedImageVector.push_back(newImageRow);
	}
	
	return manipulatedImageVector;
}

//Assimilating channels to form an 3-channel image (like RGB/CMY/YUV)
vector<vector<vector<unsigned char>>> AssimilateChannels(vector<vector<vector<vector<unsigned char>>>> CollectionOfChannels){
	
	int height = CollectionOfChannels.front().size();
	int width = CollectionOfChannels.front().front().size();
	int channels = CollectionOfChannels.size();

	cout << "\nAssimilating channels ... \n";

	vector<unsigned char> newImageChannel(channels,0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>>manipulatedImageVector(height, newImageColumn);
	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<channels; k++){
				manipulatedImageVector[i][j][k] = CollectionOfChannels[k][i][j][0];
			}
		}
	}

	return manipulatedImageVector;
}

//Quantize a channel, receives image, returns channel
vector<vector<vector<unsigned char>>> SingleChannelQuantizer(vector<vector<vector<unsigned char>>>ImageVector, int channelNumber, int quantizeBy){
	
	cout << "Quantizing channel " << channelNumber << " ...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageRow;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			vector<unsigned char> newImageColumn;
			int channelValue = (int)ImageColumn.at(channelNumber);
			//If desired putput is 24-bit, i.e. 8-bits/channel, then no processing takes place, because the input image is 24-bit
			//The function works as channel extracter is that case
			if(quantizeBy<256){
				channelValue = (channelValue*quantizeBy)/256;
				channelValue = ((channelValue*256)/quantizeBy) + (256/(2*quantizeBy)) - 1;	
			}
			newImageColumn.push_back((unsigned char)channelValue);
			newImageRow.push_back(newImageColumn);
		}
		manipulatedImageVector.push_back(newImageRow);
	}
	
	return manipulatedImageVector;
}

//Assign histogram to a channel (after manipulation)
vector<vector<vector<unsigned char>>> SetSingleChannelByHistogram(vector<vector<vector<unsigned char>>> SingleChannelVector, vector <int> histogramVector){
	
	int height = SingleChannelVector.size();
	int width = SingleChannelVector.front().size();
	int totalPixels = height*width;

	vector<unsigned char> newImageChannel(1, 0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>>resultSingleChannelVector(height, newImageColumn);

	//Creating normalized histogram
	vector<unsigned char> newPixelValues(256, 0);
	for(int i=0; i<256; i++){
		float temp = (histogramVector[i]*255)/totalPixels;
		if(temp>255){
			temp = 255;
		}
		int tempInt = (int)temp;
		newPixelValues[i] = (unsigned char)tempInt;
	}

	cout << "\n";

	//Assigning new pixels to image
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			resultSingleChannelVector[i][j][0] = newPixelValues[SingleChannelVector[i][j][0]];
		}
	}

	return resultSingleChannelVector;
}

//Bucket-filling is used to redistribute pixels to achieve a flat histogram, color-depth measured in bits
vector<vector<vector<unsigned char>>> SingleChannelBucketFilling (vector<vector<vector<unsigned char>>> SingleChannelVector, vector<int> histogram){

	int height = SingleChannelVector.size();
	int width = SingleChannelVector.front().size();
	int totalPixels = height*width;

	int numberOfGrayLevels = 256;
	float pixelsPerLevel_exact = (float)totalPixels/(float)numberOfGrayLevels;
	int pixelsPerLevel = (int)pixelsPerLevel_exact;
	float delta = pixelsPerLevel_exact - (float)pixelsPerLevel;
	int remainingPixels = (int)(delta*(float)numberOfGrayLevels);

	vector<int> histogramCOPY = histogram;

	//Redistribution takes place BUT (delta*numberOfGrayLevels)
	//are left out and are grouped with 255
	int	redistributionTable[numberOfGrayLevels][numberOfGrayLevels] = {0};
	for(int p=0; p<numberOfGrayLevels; p++){
		//Redistribution of remaining pixels by increasing 
		//counts of pixels by 1 where possible
		int newPixelsPerLevel = pixelsPerLevel;
		if(remainingPixels>0){
			newPixelsPerLevel = pixelsPerLevel+1;
			remainingPixels--;
		}

		if(histogram[p]>newPixelsPerLevel){		
			for(int q=0; q<numberOfGrayLevels; q++){
				if(histogram[q]<newPixelsPerLevel){
					int delta1 = histogram[p] - newPixelsPerLevel;
					int delta2 = newPixelsPerLevel - histogram[q];
					int toBeAdded = min(delta1, delta2);
					histogram[p] -= toBeAdded;
					histogram[q] += toBeAdded;
					redistributionTable[p][q] = toBeAdded;
				}
			}
		}

	}

	cout << "Bucket-fill redistribution in process ... \n";
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int p=0; p<256; p++){
				int temp = redistributionTable[ SingleChannelVector[i][j][0] ][p];
				if(temp>0){
					redistributionTable[SingleChannelVector[i][j][0]][p]--;
					SingleChannelVector[i][j][0] = (unsigned char)p;
					p=256;
				}
			}
		}
	}

	return SingleChannelVector;
}

//RGB to CMY
vector<vector<vector<unsigned char>>> RGBtoCMY(vector<vector<vector<unsigned char>>>ImageVector){
	
	cout << "Converting to from RGB to CMY ...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageRow;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			vector<unsigned char> newImageColumn;
			for(vector<unsigned char>::iterator itChannel = ImageColumn.begin(); itChannel!=ImageColumn.end(); itChannel++){
				newImageColumn.push_back((char)(255 - (int)*itChannel));
			}
			newImageRow.push_back(newImageColumn);
		}
		manipulatedImageVector.push_back(newImageRow);
	}
	return manipulatedImageVector;
}

//Grayscale - Lightness Method
vector<vector<vector<unsigned char>>> GrayLightness(vector<vector<vector<unsigned char>>>ImageVector){
	
	cout << "Creating a grayscale image by Lightness Method...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageColumn;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			int min = (int)*min_element( ImageColumn.begin(), ImageColumn.end());
			int max = (int)*max_element( ImageColumn.begin(), ImageColumn.end());
			int sum = (max + min)/2;
			vector<unsigned char> grayChannel;
			grayChannel.push_back((char)(sum));
			newImageColumn.push_back(grayChannel);
		}
		manipulatedImageVector.push_back(newImageColumn);
	}
		
	return manipulatedImageVector;
}

//Grayscale - Average Method
vector<vector<vector<unsigned char>>> GrayAverage(vector<vector<vector<unsigned char>>>ImageVector){
	
	cout << "Creating a grayscale image by Averaging Method...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageColumn;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			int sum = 0;
			for(vector<unsigned char>::iterator itChannel = ImageColumn.begin(); itChannel!=ImageColumn.end(); itChannel++){
				sum += *itChannel;
			}
			vector<unsigned char> grayChannel;
			grayChannel.push_back((char)(sum/3));
			newImageColumn.push_back(grayChannel);
		}
		manipulatedImageVector.push_back(newImageColumn);
	}

	return manipulatedImageVector;
}

//Grayscale - Luminosity Method
vector<vector<vector<unsigned char>>> GrayLuminosity(vector<vector<vector<unsigned char>>>ImageVector){
	
	cout << "Creating a grayscale image by Luminosity Method...\n";
	
	vector<vector<vector<unsigned char>>>manipulatedImageVector;
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); itRow++){
		vector<vector<unsigned char>> ImageRow = *itRow;
		vector<vector<unsigned char>> newImageColumn;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); itColumn++){
			vector<unsigned char> ImageColumn = *itColumn;
			
			float sum = 0;
			for(vector<unsigned char>::iterator itChannel = ImageColumn.begin(); itChannel!=ImageColumn.end(); itChannel++){
				int temp = (int)*itChannel;
				if(itChannel == ImageColumn.begin()){
					sum += 0.21*(float)temp;
				} else if (itChannel == ImageColumn.end()){
					sum += 0.07*(float)temp;
				} else {
					sum += 0.72*(float)temp;
				}
			}
			int temp = (int)sum;
			vector<unsigned char> grayChannel;
			grayChannel.push_back((unsigned char)(sum/3));
			newImageColumn.push_back(grayChannel);
		}
		manipulatedImageVector.push_back(newImageColumn);
	}
	
	return manipulatedImageVector;
}