///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Anurag Syal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FileIO.h"

using namespace std;

/////////////////////////////////////////////	FILE I/O	/////////////////////////////////////////////////////////////////
//Check file validity
bool FileOk(string fileName){
	const char* inputFile = fileName.c_str();
	FILE *filePointer = fopen(inputFile, "rb");;
	if (!(filePointer)){
		return false;
	}
	return true;
}

//Find file size
long FileSize(string fileName){
	const char* inputFile = fileName.c_str();
	FILE *filePointer = fopen(inputFile, "rb");
	long fileLength=0;
	
	fseek(filePointer, 0, SEEK_END);
	fileLength = ftell(filePointer);
	rewind(filePointer);
	fclose(filePointer);
	
	return fileLength;
}

//Image input
vector<vector<vector<unsigned char>>> ImageInput(string fileName, int height, int width, int BytesPerPixel){
	
	const char* inputFile = fileName.c_str();
	unsigned char *Imagedata = (unsigned char*)malloc(height*width*BytesPerPixel*sizeof(unsigned char));
	
	//Always use binary method for reading/writing files
	//Othwerwise extra bits are included which are platform dependent
	FILE *filePointer = fopen(inputFile, "rb");
	fread(Imagedata, sizeof(unsigned char), height*width*BytesPerPixel, filePointer);
	fclose(filePointer);
	
	vector<vector<vector<unsigned char>>> ImageVector;
	
	for(int i=0; i<height; i++){
		vector<vector<unsigned char>> ImageColumn;
		for(int j=0; j<width; j++){
			vector<unsigned char> RGBPixel;
			for(int k=0; k<BytesPerPixel; k++){
				RGBPixel.push_back(*(Imagedata+(i*width+j)*BytesPerPixel+k));
			}
			ImageColumn.push_back(RGBPixel);
		}
		ImageVector.push_back(ImageColumn);
	}
	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<BytesPerPixel; k++){
				unsigned char temp = ImageVector[i][j][k];
			}
		}
	}
	
	return ImageVector;
}

//Image output
bool ImageOutput(string fileName, vector<vector<vector<unsigned char>>>ImageVector){
	
	if(fileName.size()==0){
		return false;
	}
	
	//Always use binary method for reading/writing files
	//Othwerwise extra bits are included which are platform dependent
	const char* output_file = fileName.c_str();
	ofstream outputfile(output_file, ios::binary);
	
	for(vector<vector<vector<unsigned char>>>::iterator itRow = ImageVector.begin(); itRow!=ImageVector.end(); ++itRow){
		vector<vector<unsigned char>> ImageRow = *itRow;
		for(vector<vector<unsigned char>>::iterator itColumn = ImageRow.begin(); itColumn!=ImageRow.end(); ++itColumn){
			vector<unsigned char> ImageColumn = *itColumn;
			for(vector<unsigned char>::iterator itChannel = ImageColumn.begin(); itChannel!=ImageColumn.end(); ++itChannel){
				outputfile << *itChannel;
			}
		}
	}
    
    return true;
}

//Get entries/file-names of a directory
vector <string> OpenFolder (string folderPath) {

	DIR *workingDirectory;
	dirent *directoryEntries;
	vector <string> files;

	//cout << "\nChecking folder\n";

	workingDirectory = opendir(folderPath.c_str());

	if(workingDirectory == NULL){
		cout << "Error (" << errno << ") opening " << folderPath << endl;
		closedir(workingDirectory);
		return files;
	}

	while ((directoryEntries = readdir(workingDirectory)) != NULL) {
		string fileName = directoryEntries -> d_name;
		if(fileName == "." || fileName == "..")
			continue;
		files.push_back(fileName);
		//cout << directoryEntries->d_name << "\n";
	}

	closedir(workingDirectory);
	return files;

}
