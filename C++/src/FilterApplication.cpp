///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FilterApplication.h"

using namespace std;

//Preparing a channel image for filter operations, like Gaussian Filter or Low-Pass filter
vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int height,
														  int width, int BytesPerPixel, int kernelSize){
	//Creating an image buffer with increased dimensions
    //Edges of the original image are replicated to fill up the blank space
    //This is done so that the edges and corners can be smoothed
	
	cout << "Preparing channel for filter operation ...\n";

	int outputWidth  = width+kernelSize-1;
	int outputHeight = height+kernelSize-1;
	
	cout << "Input Width : " << width << " Input Height : " << height << "\n";
	cout << "Output Width: " << outputWidth << " Output Height: " << outputHeight << "\n\n";
	
	vector<unsigned char> channelVector(BytesPerPixel, 0);
	vector<vector<unsigned char>> rowVector(outputWidth, channelVector);
	vector<vector<vector<unsigned char>>> manipulatedImageVector(outputHeight, rowVector);
	
	//Copying the raw image in to middle of the block
	for(int i=0; i<height; i++)
    	for(int j=0; j<width; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[ i+(kernelSize/2) ][ j+(kernelSize/2) ][k] = ImageVector[i][j][k];
    
    //Copying the top edge to the upper blank space
	for(int i=0; i<kernelSize/2; i++)
    	for(int j=0; j<width; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][ j+(kernelSize/2) ][k] = ImageVector[0][j][k];
    
    //Copying the bottom edge to the lower blank space
    for(int i=height+(kernelSize)/2; i < outputHeight; i++)
    	for(int j=0; j<width; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][ j+(kernelSize/2) ][k] = ImageVector[height-1][j][k];
    
    //Copying the left edge to blank space on left
    for(int i=0; i<height; i++)
    	for(int j=0; j < (kernelSize/2)+1; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i+(kernelSize/2)][j][k] = ImageVector[i][0][k];
    
    //Copying the right edge to blank space on right
    for(int i=0; i<height; i++)
    	for(int j=width+(kernelSize/2); j < outputWidth; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i+(kernelSize/2)][j][k] = ImageVector[i][width-1][k];

    //Copying the top-left corner
    for(int i=0; i<kernelSize/2; i++)
    	for(int j=0; j<kernelSize/2; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][j][k]=ImageVector[i][j][k];

    //Copying the top-right corner
    for(int i=0; i<kernelSize/2; i++)
    	for(int j=width+(kernelSize/2); j<outputWidth; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][j][k]=ImageVector[i][j-kernelSize+1][k];
    
    //Copying the bottom-left corner
    for(int i=height+(kernelSize)/2; i < outputHeight; i++)
    	for(int j=0; j<kernelSize/2; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][j][k]=ImageVector[i-kernelSize+1][j][k];

    //Copying the bottom-right corner
    for(int i=height+(kernelSize)/2; i < outputHeight; i++)
    	for(int j=width+(kernelSize/2); j < outputWidth; j++)
    		for(int k=0; k<BytesPerPixel; k++)
    			manipulatedImageVector[i][j][k]=ImageVector[i-kernelSize+1][j-kernelSize+1][k];
    			
    return manipulatedImageVector;
}

vector<vector<vector<unsigned char>>> FilterPreprocessing(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize){
    int height = ImageVector.size();
    int width = ImageVector.front().size();
    int BytesPerPixel = ImageVector.front().front().size();

    vector<vector<vector<unsigned char>>> manipulatedImageVector = FilterPreprocessing(ImageVector, height, width, BytesPerPixel, kernelSize);

    return manipulatedImageVector;
}

//Create a Gaussian filter kernel of given size
vector<vector<float>> CreateGaussianFilterKernel(int kernelSize, float sigma){
    // Standard Deviation (sigma) is set to 1.0
	vector<float> kernelColumn(kernelSize, 0);
	vector<vector<float>> gaussKernel (kernelSize, kernelColumn);

    float numeratorForExp;
    float denominatorForExp = 2.0 * sigma * sigma;
 
    // Sum is for normalization
    float sum = 0.0;
 
    // Generating NxN kernel
    for (int y = -(kernelSize/2); y < 1+(kernelSize/2); y++){
    	for(int x = -(kernelSize/2); x < 1+(kernelSize/2); x++){
        	numeratorForExp = x*x + y*y;
            gaussKernel[y + (kernelSize/2)][x + (kernelSize/2)] = (exp(-(numeratorForExp)/denominatorForExp) )/(M_PI * denominatorForExp);
            sum += gaussKernel[y + (kernelSize/2)][x + (kernelSize/2)];
        }
    }
 
    // normalize the Kernel
    for(int y = 0; y < kernelSize; y++){
        for(int x = 0; x < kernelSize; x++){
            gaussKernel[y][x] /= sum;
        }
    }

    return gaussKernel; 
}

//Apply Gauss filter to a channel
vector<vector<vector<unsigned char>>> SingleChannelGaussFilter(vector<vector<vector<unsigned char>>> SingleChannelVector, vector<vector<float>> gaussKernel){
	int height = SingleChannelVector.size();
	int width = SingleChannelVector.front().size();
	int kernelSize = gaussKernel.size();

	int outputHeight = height-kernelSize+1;
	int outputWidth  = width-kernelSize+1;

	vector<unsigned char> newImageChannel(1, 0);
	vector<vector<unsigned char>> newImageColumn(outputWidth, newImageChannel);
	vector<vector<vector<unsigned char>>>resultImageVector(outputHeight, newImageColumn);

	vector<float> kernelColumn(kernelSize, 0);
	vector<vector<float>> tempKernelBuffer (kernelSize, kernelColumn);

	for(int q=0; q<outputHeight; q++){
    	for(int p=0; p<outputWidth; p++){
    			
    		float runningSum = 0;
    		for(int j=0; j<kernelSize; j++){
			    for(int i=0; i<kernelSize; i++){
			    	tempKernelBuffer[j][i] = (float)SingleChannelVector[ j+q ][ i+p ][0] * gaussKernel[j][i];
			    	runningSum += tempKernelBuffer[j][i];
			    }
			}
			resultImageVector[q][p][0] = (unsigned char)runningSum;

    	}
    }

    cout << "Gauss filter applied to channel ...\n";

    return resultImageVector;
}

//Apply Median filter to a channel
vector<vector<vector<unsigned char>>> SingleChannelMedianFilter(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize){
	
	int height = ImageVector.size();
	int width = ImageVector.front().size();

	int outputHeight = height-kernelSize+1;
	int outputWidth  = width-kernelSize+1;

	vector<unsigned char> newImageChannel(1, 0);
	vector<vector<unsigned char>> newImageColumn(outputWidth, newImageChannel);
	vector<vector<vector<unsigned char>>>resultImageVector(outputHeight, newImageColumn);

	for(int p=0; p<outputHeight; p++){
    	for(int q=0; q<outputWidth; q++){

    		vector<unsigned char> tempKernelBuffer;
    		for(int i=0; i<kernelSize; i++){
			    for(int j=0; j<kernelSize; j++){
			    	tempKernelBuffer.push_back(ImageVector[p+i][q+j][0]);
			    }
			}

			//Sorting the kernel buffer, middle element is presnt at (kernelSize^2)/2
			sort(tempKernelBuffer.begin(), tempKernelBuffer.end());
			resultImageVector[p][q][0] = tempKernelBuffer[(kernelSize*kernelSize)/2];

    	}
    }

    cout << "Median filter applied to channel ...\n";

    return resultImageVector;
}

//Replace the pixel value with it's most frequest neighbour (NxN)
vector<vector<vector<unsigned char>>> SingleChannelFrequentNeighborFilter(vector<vector<vector<unsigned char>>>ImageVector, int kernelSize){
	
	int height = ImageVector.size();
	int width = ImageVector.front().size();

	int outputHeight = height-kernelSize+1;
	int outputWidth  = width-kernelSize+1;

	vector<unsigned char> newImageChannel(1, 0);
	vector<vector<unsigned char>> newImageColumn(outputWidth, newImageChannel);
	vector<vector<vector<unsigned char>>>resultImageVector(outputHeight, newImageColumn);

	for(int p=0; p<outputHeight; p++){
    	for(int q=0; q<outputWidth; q++){

    		int pixelValueCounts[256]={0};
    		for(int i=0; i<kernelSize; i++){
			    for(int j=0; j<kernelSize; j++){
			    	pixelValueCounts[ImageVector[p+i][q+j][0]]++;
			    }
			}

			//Find most frequent pixel in NxN block
			int maxCount = 0;
			unsigned char mostFreqElement = 0;
			for(int i=0; i<256; i++){
		        if(pixelValueCounts[i] > maxCount){
		            maxCount = pixelValueCounts[i];
					mostFreqElement = (unsigned char)i;
				}
			}

			resultImageVector[p][q][0] = mostFreqElement;

    	}
    }

    cout << "Frequent neighbour filter for channel ...\n";

    return resultImageVector;
}

//Dither using Fixed, Random or Bayer thresholding methods
//bayerKernelSize doesn't matter for Fixed or Random methods
vector<vector<vector<unsigned char>>> SingleChannelDither (vector<vector<vector<unsigned char>>> SingleChannelVector, 
                                                           int method, int numberOfTones, int bayerMatrixNumber){

    int height = SingleChannelVector.size();
    int width = SingleChannelVector.front().size();

    vector<unsigned char> newImageChannel(1, 0);
    vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
    vector<vector<vector<unsigned char>>> manipulatedImageVector(height, newImageColumn);

    unsigned char threshold = 0;

    
    if(method == 1){

        if(numberOfTones == 2){     
        
            threshold = 127;
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {
                    
                    if(SingleChannelVector[i][j][0] > threshold){
                            manipulatedImageVector[i][j][0] = 0;
                    } else{
                        manipulatedImageVector[i][j][0] = 255;
                    }

                }
            }

        } else if(numberOfTones == 4){

            for(int i=0; i<width; i++){
                for(int j=0; j<height; j++){
                    
                    unsigned char temp = SingleChannelVector[i][j][0];
                    if(temp > 212)
                        manipulatedImageVector[i][j][0] = 255;
                    else if (temp > 127)
                        manipulatedImageVector[i][j][0] = 170;
                    else if (temp > 42)
                        manipulatedImageVector[i][j][0] = 85;
                    else 
                        manipulatedImageVector[i][j][0] = 0;

                }   
            }

        }

    } else if (method == 2) {

        //cout << "\nI am still here: " << method << " " << numberOfTones << " " << bayerMatrixNumber << "\n";
        srand((int) time(0));
        if(numberOfTones == 2){
            
            for(int i=0; i<height; i++) {
                for(int j=0; j<width; j++) {

                    float random = rand() % 100;
                    threshold = (unsigned char) ( (random *255)/100 ) ;

                    if(SingleChannelVector[i][j][0] > threshold){
                            manipulatedImageVector[i][j][0] = 255;
                    } else{
                        manipulatedImageVector[i][j][0] = 0;
                    }

                }
            }

        } else if (numberOfTones == 4){

            for(int i=0; i<width; i++){
                for(int j=0; j<height; j++){
                    float random = rand() % 100;
                    threshold = (unsigned char) ( (random*255)/100 );
                    
                    unsigned char lowPixel = 0, highPixel = 255;

                    if(threshold > 212) {
                        highPixel = 255;
                        lowPixel = 170;
                    } else if (threshold > 127){
                        highPixel = 170;
                        lowPixel = 85;
                    } else if (threshold > 42) {
                        highPixel = 85;
                        lowPixel = 0;
                    }
                        
                    if ( SingleChannelVector[i][j][0] > threshold )
                        manipulatedImageVector[i][j][0] = highPixel;
                    else 
                        manipulatedImageVector[i][j][0] = lowPixel;

                }   
            }

        }

    } else if (method == 3){

        manipulatedImageVector = SingleChannelBayerFilter(SingleChannelVector, numberOfTones, bayerMatrixNumber);
    
    } else {
        cout << "\nNo such method\n" << "Method = 1, 2 or 3\n";
    }

    return manipulatedImageVector;
}   

//Image is dithered using Bayer Matrices of kernel sizes 2, 4 and 8
vector<vector<vector<unsigned char>>> SingleChannelBayerFilter(vector<vector<vector<unsigned char>>> SingleChannelVector,
                                                                int numberOfTones, int bayerMatrixNumber){

    //Bayer Threshold Tables
    unsigned char bayer_threshold_matrix_2 [2][2] = {
        {96, 159},
        {223, 32}
    };

    unsigned char bayer_threshold_matrix_4 [4][4] = {
        {88, 151, 104, 167},
        {215, 24, 231, 40},
        {120, 183, 72, 135},
        {247, 56, 199, 8}
    };  

    unsigned char bayer_threshold_matrix_8 [8][8] = {
        {86, 149, 102, 165, 90, 153, 106, 169},
        {213, 22, 229, 38, 217, 26, 233, 42},
        {118, 181, 70, 133, 122, 185, 74, 137},
        {245, 54, 197, 6, 249, 58, 201, 10},
        {94, 157, 110, 173, 82, 145, 98, 161},
        {221, 30, 237, 46, 209, 18, 225, 34},
        {126, 189, 78, 141, 114, 177, 66, 129},
        {253, 62, 205, 14, 241, 50, 193, 2}
    };

    int height = SingleChannelVector.size();
    int width = SingleChannelVector.front().size();
    
    vector<unsigned char> newImageChannel(1, 0);
    vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
    vector<vector<vector<unsigned char>>> manipulatedImageVector(height, newImageColumn);

    //Assigning a threshold kernel
    vector <vector <unsigned char>> thresholdKernel;
    if(bayerMatrixNumber == 2){

        for(int i=0; i<bayerMatrixNumber; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<bayerMatrixNumber; j++){
                kernelRow.push_back(bayer_threshold_matrix_2[i][j]);
            }
            
            thresholdKernel.push_back(kernelRow);
        }
        
    } else if(bayerMatrixNumber == 4){

        for(int i=0; i<bayerMatrixNumber; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<bayerMatrixNumber; j++){
                kernelRow.push_back(bayer_threshold_matrix_4[i][j]);
            }
            
            thresholdKernel.push_back(kernelRow);
        }
        
    } else if(bayerMatrixNumber == 8){
        
        for(int i=0; i<bayerMatrixNumber; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<bayerMatrixNumber; j++){
                kernelRow.push_back(bayer_threshold_matrix_8[i][j]);
            }
            
            thresholdKernel.push_back(kernelRow);
        }

    } else {
        cout << "\n\nInvalid Option - Bayer matrices = 2, 4 or 8\n\n";
        return manipulatedImageVector;
    }

    //Toning
    if(numberOfTones == 2){
        //Thresholding - Halftoning
        for(int i=0; i<width; i++){
            for(int j=0; j<height; j++){
                
                unsigned char threshold = thresholdKernel [ i % bayerMatrixNumber ][ j % bayerMatrixNumber ];
                if(SingleChannelVector[i][j][0] > threshold){
                    manipulatedImageVector[i][j][0] = 255;
                }
                
            }
        }

    } else if (numberOfTones == 4){
        //Thresholding - Quadtoning
        
        for(int i=0; i<width; i++){
            for(int j=0; j<height; j++){
                
                unsigned char threshold = thresholdKernel [ i % bayerMatrixNumber ][ j % bayerMatrixNumber ];
                unsigned char lowPixel = 0, highPixel = 255;

                if(threshold > 212) {
                    highPixel = 255;
                    lowPixel = 170;
                } else if (threshold > 127){
                    highPixel = 170;
                    lowPixel = 85;
                } else if (threshold > 42) {
                    highPixel = 85;
                    lowPixel = 0;
                }
    
                if ( SingleChannelVector[i][j][0] > threshold )
                    manipulatedImageVector[i][j][0] = highPixel;
                else 
                    manipulatedImageVector[i][j][0] = lowPixel;

            }   
        }

    }


    return manipulatedImageVector;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Image is dithered using error-diffusion - Floyd-Steinberg, JJN or Stucki
vector<vector<vector<unsigned char>>> SingleChannelErrorDiffusion(vector<vector<vector<unsigned char>>> SingleChannelVector,
                                                                  int numberOfTones, int diffusionMethod) {

    float floyd_steinberg_constant = 16;
    float jjn_constant = 48;
    float stucki_constant = 42;

    //Error Diffusion Matrices
    unsigned char floyd_steinberg [3][3] = {
        {0, 0, 0},
        {0, 0, 7},
        {3, 5, 1}
    };

    unsigned char jjn [5][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 7, 5},
        {3, 5, 7, 5, 3},
        {1, 3, 5, 3, 1},
    };  

    unsigned char stucki [5][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 8, 4},
        {2, 4, 8, 4, 2},
        {1, 2, 4, 2, 1},
    };

    //Assigning a threshold kernel
    int kernelSize = 1;
    float divisionConstant = 1;

    vector <vector <unsigned char>> diffusionMatrix;
    vector <vector <unsigned char>> diffusionMatrix_reverse;
    if(diffusionMethod == 1){
        kernelSize = 3;
        divisionConstant = floyd_steinberg_constant;
        
        for(int i=0; i<3; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<3; j++){
                kernelRow.push_back(floyd_steinberg[i][j]);
            }
            
            diffusionMatrix.push_back(kernelRow);
        }

        for(int i=0; i<3; i++){
            vector <unsigned char> kernelRow;
            for(int j=2; j>=0; j--){
                kernelRow.push_back(floyd_steinberg[i][j]);
            }
            
            diffusionMatrix_reverse.push_back(kernelRow);
        }
        
    } else if(diffusionMethod == 2){
        kernelSize = 5;
        divisionConstant = jjn_constant;
        for(int i=0; i<5; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<5; j++){
                kernelRow.push_back(jjn[i][j]);
            }
            
            diffusionMatrix.push_back(kernelRow);
        }
        
    } else if(diffusionMethod == 3){
        kernelSize = 5;
        divisionConstant = stucki_constant;
        for(int i=0; i<5; i++){
            vector <unsigned char> kernelRow;
            for(int j=0; j<5; j++){
                kernelRow.push_back(stucki[i][j]);
            }
            
            diffusionMatrix.push_back(kernelRow);
        }

    } else {
        cout << "\n\nInvalid Option - Diffusion matrices = 1, 2 or 3\n\n";
    }

    cout << "Diffusion Matrix in use:\n\n";
    for(vector<vector<unsigned char>>::iterator itRow = diffusionMatrix.begin(); itRow!=diffusionMatrix.end(); itRow++){
        vector<unsigned char> column = *itRow;
        for(vector<unsigned char>::iterator itColumn = column.begin(); itColumn!= column.end(); itColumn++){
            cout << setw(3) << (int)*itColumn;
        }
        cout << "\n";        
    }

    if(diffusionMethod == 1){
        cout << "\nReverse Diffusion Matrix:\n\n";
        for(vector<vector<unsigned char>>::iterator itRow = diffusionMatrix_reverse.begin(); itRow!=diffusionMatrix_reverse.end(); itRow++){
            vector<unsigned char> column = *itRow;
            for(vector<unsigned char>::iterator itColumn = column.begin(); itColumn!= column.end(); itColumn++){
                cout << setw(3) << (int)*itColumn;
            }
            cout << "\n";        
        }        
    }

    cout << "\n";

    //Creating Structures
    int height = SingleChannelVector.size();
    int width = SingleChannelVector.front().size();

    int outputHeight = height-kernelSize+1;
    int outputWidth  = width-kernelSize+1;

    cout << "Input Width : " << width << " Input Height : " << height << "\n";
    cout << "Output Width: " << outputWidth << " Output Height: " << outputHeight << "\n\n";

    vector<unsigned char> newImageChannel(1, 0);
    vector<vector<unsigned char>> newImageColumn(outputWidth, newImageChannel);
    vector<vector<vector<unsigned char>>> manipulatedImageVector(outputHeight, newImageColumn);

    //Toning
    float pixelMax = -1000, pixelMin = 1000;

    for(int p=0; p<outputHeight; p++){

        for(int q=0; q<outputWidth; q++){
            
            //cout << "q+(kernelSize/2): " << q+(kernelSize/2) << " p+(kernelSize/2): " << p+(kernelSize/2) << "\n";

            unsigned char oldPixel;
            //cout << "Done\n";
            unsigned char newPixel = 0;
            
            if((diffusionMethod == 1) && (p%2 == 0)){
                oldPixel = SingleChannelVector[ p + (kernelSize/2) ][ width - 1 - q - (kernelSize/2) ][0];
            } else {
                oldPixel = SingleChannelVector[ p + (kernelSize/2) ][ q + (kernelSize/2) ][0];
            }

            
            if(numberOfTones == 2){
                
                if(oldPixel > 127)
                    newPixel = 255;

            } else if (numberOfTones == 4) {
                
                if(oldPixel > 212) {
                    newPixel = 255;
                } else if (oldPixel > 127){
                    newPixel = 170;
                } else if (oldPixel > 42) {
                    newPixel = 85;
                }

            } else {
                cout << "\n\nInvalid Option - Number of Tones = 2 or 4\n\n";
                return manipulatedImageVector;
            }

            
            if((diffusionMethod == 1) && (p%2 == 0)){
                manipulatedImageVector[p][outputWidth - 1 - q][0] = newPixel;
            } else {
                manipulatedImageVector[p][q][0] = newPixel;
            }


            //cout << "p: " << p << " " << " q: " << q << "\n";

            //Error diffusion in process
            //float error = abs(newPixel - oldPixel);
            float error = (float)((int)oldPixel - (int)newPixel);
            for(int i=0; i<kernelSize; i++){
                for(int j=0; j<kernelSize; j++){
                    
                    float diffuseBy;
                    if((diffusionMethod == 1) && (p%2 == 0)){
                        diffuseBy = (float)diffusionMatrix_reverse[i][j];
                    } else {
                        diffuseBy = (float)diffusionMatrix[i][j];
                    }

                    if(diffuseBy != 0){

                        float deltaCheck = (error * diffuseBy) / divisionConstant;
                        int delta = (int) roundf((error * diffuseBy) / divisionConstant);
                        
                        int checkI, checkJ;
                        //checkI = (p + 1) + i - (kernelSize/2);
                        //checkJ = (q + 1) + j - (kernelSize/2);
                        checkI = p + i;
                        checkJ = q + j;

                        if((diffusionMethod == 1) && (p%2 == 0)){
                            //checkJ = (width - 1) - (q + 1) + j - (kernelSize/2);
                            checkJ = width - kernelSize - q + j;
                            //cout << "odd  - SingleChannelVector: " << checkI << " " << checkJ << "\n";
                        } 

                        int temp = (int)SingleChannelVector[checkI][checkJ][0] + delta;

                        if(temp < 0)
                            temp = 0;
                        
                        if(temp > 255)
                            temp = 255;

                        SingleChannelVector[checkI][checkJ][0] =  (unsigned char)temp;

                        if(temp > pixelMax)
                            pixelMax = deltaCheck;

                        if(temp < pixelMin)
                            pixelMin = deltaCheck;

                    }
                }
            }

            //cout << "\n";
        }
    }

    cout << setw(5) << "pixelMin: " << pixelMin << " pixelMax: " << pixelMax << "\n";
    cout << "Error diffusion applied to channel ...\n\n";
    return manipulatedImageVector;
}