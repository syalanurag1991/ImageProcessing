#include "surf_matching.hpp"

using namespace cv;
using namespace cv::xfeatures2d;



void display_SURF_features(const char *filename)
{
	Mat input_img = imread(filename);
	char figure_name[80];
	char output_filename[80];
	int minHessian = 10400;

	Ptr<SURF> detector = SURF::create(minHessian);

	std::vector<KeyPoint> input_keypoints;
	Mat output;

	detector->detect(input_img, input_keypoints);

	drawKeypoints(input_img, input_keypoints, output, Scalar(0,0,255),4);

	sprintf(figure_name, "SURF Keypoints in %s", filename);
	imshow( figure_name, output );

	sprintf(output_filename, "%s_SURF_Keypoints.png", filename);

	imwrite(output_filename, output);

	waitKey(0);
}


void SURF_feature_matching(const char *img1_filename, const char *img2_filename)
{
	Mat img1 = imread(img1_filename);
	Mat img2 = imread(img2_filename);
	int minHessian = 10000;
	char output_figurename[80];
	char output_filename[80];

	Ptr<SURF> detector = SURF::create(minHessian);

	std::vector<KeyPoint> kp_img1;
	std::vector<KeyPoint> kp_img2;

	Mat dsc_img1;
	Mat dsc_img2;


	detector->detectAndCompute(img1, Mat(), kp_img1, dsc_img1);
	detector->detectAndCompute(img2, Mat(), kp_img2, dsc_img2);

	FlannBasedMatcher matcher;
	std::vector<DMatch> matches;
	matcher.match(dsc_img1, dsc_img2, matches);

	double max_dist = 0; 
	double min_dist = DBL_MAX;

	/* Quick calculation of max and min distances between keypoints */
	for(int i = 0; i < dsc_img1.rows; i++) { 
		double dist = matches[i].distance;
		if(dist < min_dist) {
			min_dist = dist;
		}
		if(dist > max_dist) {
			max_dist = dist;
		}
	}

	std::vector<DMatch> good_matches;
	for(int i = 0; i < dsc_img1.rows; i++) { 
		if(matches[i].distance < 2.5 * min_dist) { 
			good_matches.push_back(matches[i]); 
		}
	}
	matches.clear();
	
	Mat img_matches;	

	

	drawMatches(img1, kp_img1, img2, kp_img2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	dsc_img1.release();
	dsc_img2.release();
	kp_img1.clear();
	kp_img2.clear();
	good_matches.clear();

	sprintf(output_figurename, "SURF matching between %s and %s\n", img1_filename, img2_filename);
	sprintf(output_filename, "SURF_Match_%s_and_%s.png", img1_filename, img2_filename);
	imshow( output_figurename, img_matches);
	imwrite(output_filename, img_matches);

}

