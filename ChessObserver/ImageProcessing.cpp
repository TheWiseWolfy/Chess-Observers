

#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "ImageProcessing.h"

using namespace std;
using namespace cv;


Mat process(Mat img) {

	try
	{
		if (img.empty())
		{
			cout << "esti imbecil" << endl;
			throw "BAD TIME AHEAD";
		}
		else
		{

			//a contrast fix
			Mat aux = Mat::zeros(img.size(), img.type());
			double alpha = 1.0;
			int beta = 0;

			for (int y = 0; y < img.rows; y++) {
				for (int x = 0; x < img.cols; x++) {
					for (int c = 0; c < img.channels(); c++) {
						aux.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * img.at<Vec3b>(y, x)[c] + beta);
					}
				}
			}
			//imshow("chessboard test", img);
			//waitKey();

			img = aux;

			//imshow("contrast enhaced", img);
			//waitKey();

			Mat grayed_out_img;

			// COLOR_BGR2GRAY - > Conversion of BGR color spaces to grayscale
			cvtColor(img, grayed_out_img, COLOR_BGR2GRAY);
			Mat thresh_result;

			//imshow("greyed out", grayed_out_img);
			//waitKey();

			//the segmentation itself is done with a threshhold 
			threshold(grayed_out_img, thresh_result, 0 , 255, THRESH_BINARY_INV + THRESH_OTSU);

			//imshow("threshold", thresh_result);
			//waitKey();

			//morphological transformation, for accentuating the general shape of an object
			//the resulting image should have less noise than the initial threshold

			//structural element for building the transformations
			
			//morph pick argument

			//1 - > erosion
			//2 - > dilation
			//3 - > opening( THIS IS WHAT WE USE !!)
			//4 - > closing
			//5 - > Morphological Gradient
			//6 - > Top Hat
			//7 - > Black Hat

			int operation = 3;

			//morph size should be changed accordingly to the source image for the best result, so this value might have to go throught some changes in the future
			int morph_size = 3;

			Mat kernel = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
			Mat denoised_thresh;

			morphologyEx(thresh_result, denoised_thresh, operation, kernel);
			morphologyEx(denoised_thresh, denoised_thresh, 2, kernel);

			//imshow("denoised threshold", denoised_thresh);
			//waitKey();

			Mat dilated_;
			dilate(denoised_thresh, dilated_, Mat(), Point(-1, -1), morph_size);

			//imshow("dilated threshold", dilated_);
			//waitKey();

			Mat rough_distances_;
			
			distanceTransform(denoised_thresh, rough_distances_, DIST_L2, 3);
			
			normalize(denoised_thresh, denoised_thresh, 0.0, 1.0, NORM_L2);

			//getting the maximum value from the rough distance

			double minVal;
			double maxVal;
			Point minLoc;
			Point maxLoc;

			minMaxLoc(rough_distances_, &minVal, &maxVal, &minLoc, &maxLoc);

			Mat distances_;
			threshold(rough_distances_, distances_, 0.7 * maxVal, maxVal, 0);

			//imshow("distances", distances_);
			//waitKey();

			//stupid code honestly, but it seems to fix the dimension error i encountered with the substract function
			imwrite("Source_Images\\background.jpeg", dilated_);
			imwrite("Source_Images\\points.jpeg", distances_);
			dilated_ = imread("Source_Images\\background.jpeg", IMREAD_GRAYSCALE);
			distances_ = imread("Source_Images\\points.jpeg", IMREAD_GRAYSCALE);

			Mat board_points;
			//subtract(dilated_,100, dilated_);
			subtract(dilated_, distances_, board_points);
			
			//subtract(grayed_out_img, distances_, board_points);
			
			Size patternsize(8, 8);
			vector<Point2f> corners;

			bool patternfound = findChessboardCorners(board_points, patternsize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
			if (patternfound)
			{
				cornerSubPix(board_points, corners, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.001));
				drawChessboardCorners(img, patternsize, Mat(corners), patternfound);
				imshow("zeama de petrisor", img);
				waitKey();
			}
			else
				cout << "\n\nesti fraier" << endl;

			return board_points;

		}
		
	}
	catch (cv::Exception& e)
	{
		cerr << " \n\n\n\n\n\n\n\t\t EXCEPTIONS:\n" << e.msg << endl; // output exception message
	}



	return Mat();
 }


Mat process2(Mat img) {

	Mat edge = Mat(img.size(), CV_8UC1);
	Mat cdst;


	//a contrast fix
	Mat aux = Mat::zeros(img.size(), img.type());
	double alpha = 1.0;
	int beta = 0;

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			for (int c = 0; c < img.channels(); c++) {
				aux.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * img.at<Vec3b>(y, x)[c] + beta);
			}
		}
	}



	Canny(aux, edge, 100, 200);

	cvtColor(edge, cdst, COLOR_GRAY2BGR);


	vector<Vec2f> lines; // will hold the results of the detection

	HoughLines(edge	, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection
	// Draw the lines

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;

		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		line(aux, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
	}


	//	// Probabilistic Line Transform
	//vector<Vec4i> linesP; // will hold the results of the detection

	//HoughLinesP(edge, linesP, 1, CV_PI / 180, 50, 50, 10); // runs the actual detection

	//// Draw the lines
	//for (size_t i = 0; i < linesP.size(); i++)
	//{
	//	Vec4i l = linesP[i];
	//	line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
	//}

	imshow("costiPanel", aux);

	return cdst;
}
