///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CHANGE PHYSICAL ASPECTS LIKE DIMENSIONS, ORIENTATION ETC. OF IMAGE

#include "ImageManipulation.h"

using namespace std;

//Flip the image about Y axis
vector<vector<vector<unsigned char>>> HorizontalFlip(vector<vector<vector<unsigned char>>> ImageVector){

	//cout << "Horizontally flipping the image ...\n";
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	//cout << height << " " << width << " " << channels << "\n";

	vector<vector<vector<unsigned char>>> resultImageVector;

	for(int i=0; i<height; i++){
		vector<vector<unsigned char>> ImageColumn;
		for(int j=0; j<width; j++){
			vector<unsigned char> RGBPixel;
			for(int k=0; k<channels; k++){
				RGBPixel.push_back(ImageVector[i][width-1-j][k]);
			}
			ImageColumn.push_back(RGBPixel);
		}
		resultImageVector.push_back(ImageColumn);
	}

	return resultImageVector;
}

//Flip the image about X axis
vector<vector<vector<unsigned char>>> VerticalFlip(vector<vector<vector<unsigned char>>> ImageVector){

	//cout << "Vertically flipping the image ...\n";
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	//cout << height << " " << width << " " << channels << "\n";

	vector<vector<vector<unsigned char>>> resultImageVector;

	for(int i=0; i<height; i++){
		vector<vector<unsigned char>> ImageColumn;
		for(int j=0; j<width; j++){
			vector<unsigned char> RGBPixel;
			for(int k=0; k<channels; k++){
				RGBPixel.push_back(ImageVector[height-1-i][j][k]);
			}
			ImageColumn.push_back(RGBPixel);
		}
		resultImageVector.push_back(ImageColumn);
	}

	return resultImageVector;
}

//Scaling image using Bilinear Interpolation
vector<vector<vector<unsigned char>>> BilinearInterpolation(vector<vector<vector<unsigned char>>> ImageVector, int targetHeight, int targetWidth){
	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int numberOfChannels = ImageVector.front().front().size();

	float scaleY = (float)targetHeight/(float)height;
	float scaleX = (float)targetWidth/(float)width;

	cout << fixed;
	cout << setprecision(2);
	cout << "\nTarget scales: " << scaleX << " " << scaleY << "\n";
	cout << "Target dimensions: " << targetWidth << " " << targetHeight << "\n\n";

	vector<unsigned char> newImageChannel(numberOfChannels, 0);
	vector<vector<unsigned char>> newImageColumn(targetWidth, newImageChannel);
	vector<vector<vector<unsigned char>>>resultImageVector(targetHeight, newImageColumn);

	//int i_max = 0, j_max = 0;

	for(int p=0; p<targetHeight; p++){
		for(int q=0; q<targetWidth; q++){
			
			int i=0, j=0;
			float delta_y = 0, delta_x = 0;

			//Optimizations: No need to calculate i and j for image boundaries
			//Ex. i=0 for all pixels for 1st row
			//Ex. j=hwidth-1 for all pixels for last column
			//Boundary conditions: i+1 or j+1 should not exceed original height/width
			//to avoid segmentation fault

			if (p==0){
				i=0;
				delta_y=1;
			} else if(p==targetHeight-1){
				i=height-2;
				delta_y=0;
			} else {
				float i_exact = abs( ( ((float)(p+1))/scaleY ) - 1 );
				float i_nearest = roundf( i_exact*100 )/100;
				i = (int)(i_nearest);
				delta_y = i_exact - i_nearest;
				if(i>height-2){
					i = height-2;
				}
			}

			if (q==0){
				j=0;
				delta_x=1;
			} else if(q==targetWidth-1){
				j=width-2;
				delta_x=0;
			} else {
				float j_exact = abs( ( ((float)(q+1))/scaleX ) - 1 );
				float j_nearest = roundf( j_exact*100 )/100;
				j = (int)(j_nearest);
				delta_x = j_exact - j_nearest;
				if(j>width-2){
					j = width-2;
				}
			}

			for(int channel=0; channel<numberOfChannels; channel++){

				resultImageVector[p][q][channel] = (unsigned char) ( delta_y*(delta_x*ImageVector[i][j][channel] + (1-delta_x)*ImageVector[i][j+1][channel]) +
				(1-delta_y)*(delta_x*ImageVector[i+1][j][channel] + (1-delta_x)*ImageVector[i+1][j+1][channel]) );

			}
		}
	}

	return resultImageVector;
}

//Rotate image by 90°, 180° and 270°
vector<vector<vector<unsigned char>>> RotateImage (vector<vector<vector<unsigned char>>> ImageVector, int angle){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	vector<vector<vector<unsigned char>>> manipulatedImageVector;

	if( (angle == 90)  || (angle == 270) ){

		vector<unsigned char> newImageChannel(channels,0);
		vector<vector<unsigned char>> newImageColumn(height, newImageChannel);
		vector<vector<vector<unsigned char>>> rotatedImageVector(width, newImageColumn);

		//Rotate by 90° Clockwise or 270°
		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				for(int k=0; k<channels; k++){
					if(angle == 90){
						rotatedImageVector[j][height-1-i][k] = ImageVector[i][j][k];
					} else {
						rotatedImageVector[width-1-j][i][k] = ImageVector[i][j][k];
					}
					
				}
			}
		}

		manipulatedImageVector = rotatedImageVector;
	} else {
		vector<unsigned char> newImageChannel(channels,0);
		vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
		vector<vector<vector<unsigned char>>> rotatedImageVector(height, newImageColumn);

		//Rotate by 90° Clockwise or 270°
		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				for(int k=0; k<channels; k++){
					rotatedImageVector[height-1-i][width-1-j][k] = ImageVector[i][j][k];
				}
			}
		}

		manipulatedImageVector = rotatedImageVector;	
	}

	return manipulatedImageVector;
}

//Warp OR De-Warp an imgae to circle/ellipse using Elliptical Grid Mapping
// WARP - warpThis = TRUE | DE-WARP - warpThis = FALSE
//vector<vector<vector<unsigned char>>> ImageWarp_EllipticalGrid(vector<vector<vector<unsigned char>>> ImageVector, bool warpThis);
vector<vector<vector<unsigned char>>> ImageWarp_EllipticalGrid(vector<vector<vector<unsigned char>>> ImageVector, bool warpThis){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int channels = ImageVector.front().front().size();

	vector<unsigned char> newImageChannel(channels, 0);
	vector<vector<unsigned char>> newImageColumn(width, newImageChannel);
	vector<vector<vector<unsigned char>>> resultImageVector(height, newImageColumn);

	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){

			//Change from IJ space to XY space
			float xCartesian = (float)j + 0.5;
			float yCartesian = (float)height - 0.5 - (float)i;

			float halfHeight = (float)height/2;
			float halfWidth = (float)width/2;

			//Recenter x and y
			xCartesian = xCartesian - halfWidth;
			yCartesian = yCartesian - halfHeight;

			//Normalizing coordinates
			xCartesian = xCartesian/halfWidth;
			yCartesian = yCartesian/halfHeight;			

			//Declare new coordinates
			float newX, newY;
			bool insideArea = true;

			if(warpThis){

				float checkCircle = xCartesian*xCartesian + yCartesian*yCartesian;

				if(checkCircle > 1)
					continue;
				
				//Warp Rectangle to circle
				float xWarp = 2*sqrt(2)*xCartesian;
				float yWarp = 2*sqrt(2)*yCartesian;

				float changeSquare = xCartesian*xCartesian - yCartesian*yCartesian;
				float xDelta = 2 + changeSquare;
				float yDelta = 2 - changeSquare;

				float xMajor = xDelta + xWarp;
				float xMinor = xDelta - xWarp;
				float yMajor = yDelta + yWarp;
				float yMinor = yDelta - yWarp;

				if(xMajor < 0)
					xMajor = 0;

				if(xMinor < 0)
					xMinor = 0;

				if(yMajor < 0)
					yMajor = 0;

				if(yMinor < 0)
					yMinor = 0;

				newX = 0.5*( sqrt(xMajor) - sqrt(xMinor) );
				newY = 0.5*( sqrt(yMajor) - sqrt(yMinor) );

			} else {

				//Warp Circle to Rectangle
				float xWarp = xCartesian * xCartesian / 2;
				float yWarp = yCartesian * yCartesian / 2;
				
				float xDelta = sqrt(1.0 - xWarp);
				float yDelta = sqrt(1.0 - yWarp);

				newX = xCartesian * yDelta;
				newY = yCartesian * xDelta;

			}

			//Reversing normalization of coordinates
			newX = newX * halfWidth;
			newY = newY * halfHeight;

			//Translate back to nullify recentering
			newX = newX + halfWidth;
			newY = newY + halfHeight;

			//Change from XY space to IJ space with Billinear Interpolation
			float newI_exact = (float)height - 0.5 - newY;
			float newI_nearest = roundf( newI_exact*100 )/100;
			int delta_newI = newI_exact - newI_nearest;
			int newI = (int)(newI_nearest);

			float newJ_exact = newX - 0.5;
			float newJ_nearest = roundf( newJ_exact*100 )/100;
			int delta_newJ = newJ_exact - newJ_nearest;
			int newJ = (int)(newJ_nearest);

			if(newI>height-1)
				newI = height-1;

			if(newI<0)
				newI = 0;

			if(newJ>width-1)
				newJ = width-1;

			if(newJ<0)
				newJ = 0;

			int tempI = newI+1;
			int tempJ = newJ+1;

			if(tempI > height-1)
				tempI = height-1;

			if(tempJ > width-1)
				tempJ = width-1;

			for(int k=0; k<channels; k++){

				resultImageVector[i][j][k] = (unsigned char) ( delta_newI * (delta_newJ*ImageVector[newI][newJ][k] + (1-delta_newJ)*ImageVector[newI][tempJ][k]) +
				(1-delta_newI)*(delta_newJ*ImageVector[tempI][newJ][k] + (1-delta_newJ)*ImageVector[tempI][tempJ][k]) );
				
			}
		}
	}

	return resultImageVector;
}


//Multiply matrix A by matrix B, beware of order
vector<vector<float>> MatrixMultiplication (vector<vector<float>> matrix_A, vector<vector<float>> matrix_B) {

	int matrix_A_height = matrix_A.size();
	int matrix_A_width  = matrix_A.front().size();
	int matrix_B_height = matrix_B.size();
	int matrix_B_width  = matrix_B.front().size();

	int result_AB_Width  = matrix_B_width;
	int result_AB_Height = matrix_A_height;
	
	bool proceed = false;

	if(matrix_A_width == matrix_B_height)
		proceed = true;
	
	vector <float> matrix_B_tranpose_row (matrix_B_height, 0);
	vector <vector <float>> matrix_B_tranpose (matrix_B_width, matrix_B_tranpose_row);

	for(int i=0; i<matrix_B_width; i++){
		for(int j=0; j<matrix_B_height; j++){
			matrix_B_tranpose[i][j] = matrix_B[j][i];
		}
	}

	vector <float> resultRow (result_AB_Width, 0);
	vector <vector <float>> resultAB (result_AB_Height, resultRow);

	for(int k=0; k<matrix_A_height; k++){
		for(int i=0; i<matrix_B_width; i++){
			float sum = 0;
			for(int j=0; j<matrix_A_width; j++){
				sum += matrix_A[k][j]*matrix_B_tranpose[i][j];
			}
			resultAB[k][i] = sum;
		}
	}


	return resultAB;
}

//Rotate image at any arbitary angle, angle is in degrees
vector<vector<vector<unsigned char>>> RotateImage_AnyAngle (vector<vector<vector<unsigned char>>> ImageVector, float angle){

	int height = ImageVector.size();
	int width = ImageVector.front().size();
	int numberOfChannels = ImageVector.front().front().size();

	cout << "Target Angle is: " << angle << "°\n";
	
	angle = angle * 3.14 / 180.0;
	
	//Defining a rotation matrix	
	vector<vector<float>> RotationMatrix = {
		{cos(angle), -sin(angle), 0},
		{sin(angle), cos(angle), 0},
		{0, 0, 1}
	};

	//Creating structure to store mapping of coordinates
	vector<int> newCoordinate (2, 0);
	vector<vector<int>> newCoordinateRow (width, newCoordinate);
	vector<vector<vector<int>>> newCoordinateMAP (height, newCoordinateRow);

	//Helps in translating the image
	int minI = INT_MAX;
	int minJ = INT_MAX;
	int maxI = INT_MIN;
	int maxJ = INT_MIN;
	
	//Generating MAPPING for Rotation
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){

				//Change from IJ space to XY space
				float x = (float)j + 0.5;
				float y = (float)height - 0.5 - (float)i;

				//Declare new coordinates
				float newX = 0, newY = 0;

				//Convert to Homogeneous Coordinates
				vector<vector<float>> homogeneousXY = {
					{x},
					{y},
					{1}
				};

				//Apply Rotation
				vector<vector<float>> newHomogeneousXY = MatrixMultiplication(RotationMatrix, homogeneousXY);
				
				//Convert back to Cartesian Coodinates
				newX = newHomogeneousXY[0][0]/newHomogeneousXY[2][0];
				newY = newHomogeneousXY[1][0]/newHomogeneousXY[2][0];

				//Change from XY space to IJ space
				//int newI = (int)((float)blackBoardHeight - 0.5 - newY);
				int newI = (int)( -1*(newY + 0.5) );
				int newJ = (int)(newX - 0.5);
				
				//Maintain coordinates in map
				newCoordinateMAP[i][j][0] = newJ;
				newCoordinateMAP[i][j][1] = newI;
				
				if(minI > newI)
					minI = newI;

				if(minJ > newJ)
					minJ = newJ;

				if(maxI < newI)
					maxI = newI;

				if(maxJ < newJ)
					maxJ = newJ;

		}
	}

	//Creating a blackboard to store image
	int translateI = -1*minI;
	int translateJ = -1*minJ;

	int targetHeight = maxI + translateI + 1;
	int targetWidth = maxJ + translateJ + 1;

	float scaleY = (float)targetHeight/(float)height;
	float scaleX = (float)targetWidth/(float)width;

	vector<unsigned char> resultChannel (numberOfChannels, 0);
	vector<vector<unsigned char>> resultRow (targetWidth, resultChannel);
	vector<vector<vector<unsigned char>>> resultImageVector (targetHeight, resultRow);

	//Placing the pixels of input image on the blackboard
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){

			int newI = newCoordinateMAP [i] [j] [1] + translateI;
			int newJ = newCoordinateMAP [i] [j] [0] + translateJ;

			for(int channel=0; channel<numberOfChannels; channel++){
				resultImageVector [newI][newJ][channel] = ImageVector[i][j][channel];
			}
		}
	}

	return resultImageVector;
}