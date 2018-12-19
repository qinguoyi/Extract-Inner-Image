#include <iostream>
#include <opencv2/opencv.hpp>  
#include <time.h>

using namespace cv;
using namespace std;

Vec3b RandomColor(int value);
Mat tiqu(Mat src_bit, Mat src_bit1);
Mat tiqu_b(Mat src_bit, Mat src_bit1);

int main()
{
	clock_t start, end;
	start = clock();
	Mat src, src1, src_gray, src_th, src_open;
	src = imread("src.png");
	if (!src.data)
	{
		printf("load image error！ \n");
		return -1;
	}
	
	src.copyTo(src1);
	cvtColor(src, src_gray, CV_RGB2GRAY);
	threshold(src_gray, src_th, 0, 255, CV_THRESH_OTSU);
	src_th = 255 - src_th;

	Mat src_3;
	cvtColor(src_th, src_3, CV_GRAY2RGB);

	float maxValue = 0;    
	Mat imageThin(src_th.size(), CV_32FC1); 
	distanceTransform(src_th, imageThin, CV_DIST_L2, 3);  
	Mat distShow;
	distShow = Mat::zeros(src_th.size(), CV_8UC1);

	for (int i = 0; i<imageThin.rows; i++)
	{
		for (int j = 0; j<imageThin.cols; j++)
		{
			if (imageThin.at<float>(i, j)>maxValue)
			{
				maxValue = imageThin.at<float>(i, j);    
			}
		}
	}
	for (int i = 0; i<imageThin.rows; i++)
	{
		for (int j = 0; j<imageThin.cols; j++)
		{
			if (imageThin.at<float>(i, j)>maxValue / 1.9)
			{
				distShow.at<uchar>(i, j) = 255;   
			}
		}
	}

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(distShow, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	Mat imageContours = Mat::zeros(src.size(), CV_8UC1);       
	Mat marks(src.size(), CV_32S);     
	marks = Scalar::all(0);
	int index = 0;
	int compCount = 0;
	for (; index >= 0; index = hierarchy[index][0], compCount++)
	{ 
		drawContours(marks, contours, index, Scalar::all(compCount + 1), 1, 8, hierarchy);
		drawContours(imageContours, contours, index, Scalar(255), 1, 8, hierarchy);
	}

	watershed(src_3, marks);
	Mat PerspectiveImage = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i<marks.rows; i++)
	{
		for (int j = 0; j<marks.cols; j++)
		{
			int index = marks.at<int>(i, j);
			if (marks.at<int>(i, j) == -1)
			{
				PerspectiveImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
			else
			{
				PerspectiveImage.at<Vec3b>(i, j) = RandomColor(index);
			}
		}
	}
	//imshow("caisetu", PerspectiveImage);
	//waitKey(0);
	
	cvtColor(PerspectiveImage, PerspectiveImage, CV_RGB2GRAY);
	threshold(PerspectiveImage, PerspectiveImage, 0, 255, CV_THRESH_OTSU);

	vector<vector<Point>> contours_fenshui;
	findContours(PerspectiveImage, contours_fenshui, RETR_LIST, CHAIN_APPROX_NONE);
	drawContours(src, contours_fenshui, -1, Scalar(0, 0, 255), 2);

	Mat src_thh;
	threshold(src_gray, src_thh, 0, 255, CV_THRESH_OTSU);
	Mat src_and = 255 - src_thh; 
	//imshow("src_and", src_and);
	Mat src_new = Mat::zeros(src.rows, src.cols, CV_8UC1);
	src_new = 255 - src_new;

	for (size_t i = 0; i < contours_fenshui.size(); i++)
	{
		cv::Point root_points[1][10000];
		cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

		for (size_t j = 0; j < contours_fenshui[i].size(); j++)
		{
			root_points[0][j] = cv::Point(contours_fenshui[i][j].x, contours_fenshui[i][j].y);
			mask.at<uchar>(contours_fenshui[i][j].y, contours_fenshui[i][j].x) = 255;
		}
		const cv::Point* ppt[1] = { root_points[0] };
		int npt[] = { contours_fenshui[i].size() };
		fillPoly(mask, ppt, npt, 1, cv::Scalar(255, 255, 255));

		cv::Mat src_bit = mask & src_and;
		src_and = 255 - src_bit & src_and;

		Mat src_bit1;
		src_bit.copyTo(src_bit1);
		if (countNonZero(src_bit) > 10)
		{
			Mat src_temp = tiqu(src_bit, src_bit1);
			src_new = src_new & src_temp;
		}
	}
	Mat src_final;
	src_and.copyTo(src_final);
	if (countNonZero(src_and) > 10)
	{
		Mat src_temp = tiqu(src_and, src_final);
		src_new = src_new & src_temp;
	}
	 
	
	src_new = 255 - src_new;
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(src_new, src_new, element);
	imshow("dst", 255 - src_new);
	waitKey(0);
	vector<vector<Point>>contours_new;
	cv::findContours(src_new, contours_new, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cout << contours_new.size() << endl;
	end = clock();
	cout << end - start << endl;
	return 0;  
}
Vec3b RandomColor(int value)   
{
	value = value % 120 + 100;    
	RNG rng;
	int aa = rng.uniform(0, value);
	int bb = rng.uniform(0, value);
	int cc = rng.uniform(0, value);
	return Vec3b(aa, bb, cc);
}

Mat tiqu(Mat src_bit, Mat src_bit1){
	Mat src_temp;
	vector<vector<Point>>contours_fenshui;
	cv::findContours(src_bit, contours_fenshui, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::vector<cv::Point> root_points;
	for (size_t i = 0; i < contours_fenshui.size(); i++){
		
		cv::Mat mask = cv::Mat::zeros(src_bit.rows, src_bit.cols, CV_8UC1);

		for (size_t j = 0; j < contours_fenshui[i].size(); j++){
			root_points.push_back(cv::Point(contours_fenshui[i][j].x, contours_fenshui[i][j].y));
			mask.at<uchar>(contours_fenshui[i][j].y, contours_fenshui[i][j].x) = 255;
		}
		cv::Point *temp = (cv::Point*) malloc(root_points.size()*sizeof(cv::Point));
		cv::Point* ppt[1] = {temp};
		
		for (size_t index = 0; index < root_points.size(); index++)
			ppt[0][index] = root_points[index];
		const cv::Point* ppt_[1] = { ppt[0] };
		
		int npt[] = { contours_fenshui[i].size() };
		fillPoly(mask, ppt_, npt, 1, cv::Scalar(255, 255, 255));
		Mat mask2 = 255 - mask;	
		cv::Mat src_bit2 = mask & src_bit1;
		src_temp = src_bit2| mask2;
		free(temp);
	}
	return  src_temp;
}

Mat tiqu_b(Mat src_bit, Mat src_bit1){
	Mat src_temp;
	vector<vector<Point>>contours_fenshui;
	cv::findContours(src_bit, contours_fenshui, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cv::Point root_points[1][10000];
	
	for (size_t i = 0; i < contours_fenshui.size(); i++){
		cv::Mat mask = cv::Mat::zeros(src_bit.rows, src_bit.cols, CV_8UC1);

		for (size_t j = 0; j < contours_fenshui[i].size(); j++){
			root_points[0][j] = (cv::Point(contours_fenshui[i][j].x, contours_fenshui[i][j].y));
			mask.at<uchar>(contours_fenshui[i][j].y, contours_fenshui[i][j].x) = 255;
		}
		const cv::Point* ppt_[1] = { root_points[0] };
		
		int npt[] = { contours_fenshui[i].size() };
		fillPoly(mask, ppt_, npt, 1, cv::Scalar(255, 255, 255));
		Mat mask2 = 255 - mask;
		cv::Mat src_bit2 = mask & src_bit1;
		src_temp = src_bit2 | mask2;
	}
	return  src_temp;
}
