///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ImageAnalysis.h"

using namespace std;

// Read symbols and store the number of occurences of each symbol
vector <int> GetHistogramSingleChannel(vector<vector<vector<unsigned char>>> ImageVector, int channelNumber){
	
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	
	cout << "Getting histogram for Channel " << channelNumber << " ...\n";
	
	vector<int> symbolCountVector(256, 0);

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			symbolCountVector[ImageVector[i][j][channelNumber]]++;
		}
	}
	
	return symbolCountVector;
}

// Manipulating Histogram - Cumulative Probability Distribution
// Used mainly to increase image brightness as nearly all pixels are replced by increased values
// The already bright areas will get over saturated and flooded with white
vector<int> SingleChannelHistogramCDF (vector<int> singleChannelHistogram){

    vector<int> resultHistogram(256, 0);
	resultHistogram[0] = singleChannelHistogram[0];

	for (int i=1; i<256; i++){
    	resultHistogram[i] = resultHistogram[i-1] + singleChannelHistogram[i];
    }

    return resultHistogram;
}

// Print histogram information of a channel to a file
//bool PrintHistogramToFile(string fileName, vector <unsigned long int> histogramVector){
bool PrintHistogramToFile(string fileName, vector <int> histogramVector){

	cout << "Printing histogram to file...\n";
	
    if(fileName.size()==0){
		return false;
	}
	
	//Always use binary method for reading/writing files
	//Othwerwise extra bits are included which are platform dependent
	const char* output_file = fileName.c_str();
	ofstream outputfile(output_file, ios::binary);
	
	int i=0;
	for(vector<int>::iterator it = histogramVector.begin(); it!=histogramVector.end(); it++){
				outputfile << i++;
				outputfile << ',';
				outputfile << *it;
				outputfile << '\n';
	}

	outputfile.close();
	
	return true;
}

//Match histogram of a channel with a target histogram
vector<vector<vector<unsigned char>>> SingleChannelHistogramMatching(vector<vector<vector<unsigned char>>> SingleChannelVector,
																	 vector<int> targetHistogram, vector<int> subjectHistogram){

	int height = SingleChannelVector.size();
	int width = SingleChannelVector.front().size();

	cout << "Histogram matching in process ...\n";
	vector<unsigned char> resultMap (256, 0);
	for(int i=0; i<256; i++){
		int min = INT_MAX;
		for(int j=0; j<256; j++){
			int test = abs(subjectHistogram[i]-targetHistogram[j]);
			if(test<min){
				min = test;
				resultMap[i] = (unsigned char)j;
			}
		}
	}

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			SingleChannelVector[i][j][0] = resultMap[SingleChannelVector[i][j][0]];
		}
	}

	return	SingleChannelVector;
}

//Return Mean-Square Difference (Error) between two images
float CalculateMSD(vector<vector<vector<unsigned char>>>outputImageVector, vector<vector<vector<unsigned char>>>ImageVector){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();
	int totalPixels = height*width*channels;

	long sumSqDiff = 0;

	//Calculating the PSNR for the supplied pair of images
	for(int y=0; y<height; y++){
		for(int x=0; x<width; x++){
			for(int z=0; z<channels; z++){
				int temp = outputImageVector[y][x][z] - ImageVector[y][x][z];
				sumSqDiff += temp*temp;
			}
		}
	}
	
	float meanSqError = (float)sumSqDiff/(float)totalPixels;
	return meanSqError;
}

//Return PSNR
float CalculatePSNR(float meanSquareDifference, int MAX){
	float psnr = 10*log10((MAX*MAX)/meanSquareDifference);
	return psnr;
}