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

//For reading files in a folder
#include <dirent.h>

#ifndef __FILEIO_H__
#define __FILEIO_H__

using namespace std;

/////////////////////////////////////////////	FILE I/O	/////////////////////////////////////////////////////////////////
//Check file validity
bool FileOk(string fileName);

//Find file size
long FileSize(string fileName);

//Image input
vector<vector<vector<unsigned char>>> ImageInput(string fileName, int height, int width, int BytesPerPixel);

//Image output
bool ImageOutput(string fileName, vector<vector<vector<unsigned char>>>ImageVector);

//Get entries/file-names of a directory
vector <string> OpenFolder (string folderPath);

#endif