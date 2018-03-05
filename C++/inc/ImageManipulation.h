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
#include <ctime>
#include <climits>

#ifndef __IMAGEMANIPULATION_H__
#define __IMAGEMANIPULATION_H__

using namespace std;

// CHANGE PHYSICAL ASPECTS LIKE DIMENSIONS, ORIENTATION ETC. OF IMAGE

/////////////////////////////////////////////	IMAGE MANIPULATION	/////////////////////////////////////////////////////////////////
//Flip the image about Y axis
vector<vector<vector<unsigned char>>> HorizontalFlip(vector<vector<vector<unsigned char>>> ImageVector);

//Flip the image about X axis
vector<vector<vector<unsigned char>>> VerticalFlip(vector<vector<vector<unsigned char>>> ImageVector);

//Scaling image using Bilinear Interpolation
vector<vector<vector<unsigned char>>> BilinearInterpolation(vector<vector<vector<unsigned char>>> ImageVector, int targetHeight, int targetWidth);

//Rotate image by 90°, 180° and 270°
vector<vector<vector<unsigned char>>> RotateImage (vector<vector<vector<unsigned char>>> ImageVector, int angle);

//Warp OR De-Warp an imgae to circle/ellipse using Elliptical Grid Mapping
// WARP - warpThis = TRUE | DE-WARP - warpThis = FALSE
//vector<vector<vector<unsigned char>>> ImageWarp_EllipticalGrid(vector<vector<vector<unsigned char>>> ImageVector, bool warpThis);
vector<vector<vector<unsigned char>>> ImageWarp_EllipticalGrid(vector<vector<vector<unsigned char>>> ImageVector, bool warpThis);

//Multiply matrix A by matrix B, beware of order
vector<vector<float>> MatrixMultiplication (vector<vector<float>> matrix_A, vector<vector<float>> matrix_B);

//Rotate image at any arbitary angle, angle is in degrees
vector<vector<vector<unsigned char>>> RotateImage_AnyAngle (vector<vector<vector<unsigned char>>> ImageVector, float angle);

#endif