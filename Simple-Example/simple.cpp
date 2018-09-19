#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("src.png");

	Mat src_gray, src_bit;
	cvtColor(src, src_gray, CV_BGR2GRAY);
	threshold(src_gray, src_bit, 0, 255, CV_THRESH_OTSU);
	src_bit = 255 - src_bit;
	Mat src_bit1;
	src_bit.copyTo(src_bit1);
	cv::Mat src_temp;
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(src_bit, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cv::Mat mask_final = cv::Mat::ones(src_bit.rows, src_bit.cols, CV_8UC1);
	mask_final = 255 - mask_final;
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::Point root_points[1][10000];
		cv::Mat mask = cv::Mat::zeros(src_bit.rows, src_bit.cols, CV_8UC1);

		//imshow("mask_fianl", mask_final);
		//waitKey(0);
		for (size_t j = 0; j < contours[i].size(); j++)
		{
			root_points[0][j] = cv::Point(contours[i][j].x, contours[i][j].y);
			mask.at<uchar>(contours[i][j].y, contours[i][j].x) = 255;
		}
		const cv::Point* ppt[1] = { root_points[0] };
		int npt[] = { contours[i].size() };
		fillPoly(mask, ppt, npt, 1, cv::Scalar(255, 255, 255));


		cv::Mat mask2 = 255 - mask;
		//imshow("src_temp", mask);
		//waitKey(0);
		cv::Mat src_bit2 = mask & src_bit1;
		src_temp = src_bit2 | mask2;

		mask_final = src_temp & mask_final;
		//mask_final = 255 - mask_final;
	}
	imshow("mask_final", mask_final);
	waitKey(0);
	return 0;
}