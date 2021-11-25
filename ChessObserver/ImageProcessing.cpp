

#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include "ImageProcessing.h"

using namespace std;
using namespace cv;


Mat process(Mat img)
{
	try
	{
		//Mat img = imread("Source_Images\\test_board.jpeg");
		if (img.empty()){
			throw "this is bad";
		}
		else
		{

			//imshow("chessboard test", img);
			//waitKey();
			Mat grayed_out_img;

			// COLOR_BGR2GRAY - > Conversion of BGR color spaces to grayscale
			cvtColor(img, grayed_out_img, COLOR_BGR2GRAY);
			Mat thresh_result;

			//imshow("greyed out", grayed_out_img);
			//waitKey();

			//the segmentation itself is done with a threshhold 
			threshold(grayed_out_img, thresh_result, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

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

			//imshow("denoised threshold", denoised_thresh);
			//waitKey();

			Mat dilated_;
			dilate(denoised_thresh, dilated_, Mat(), Point(-1, -1), morph_size);

			//imshow("dilated threshold", dilated_);
			//waitKey();

			Mat rough_distances_;
			distanceTransform(denoised_thresh, rough_distances_, DIST_L2, 3);

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
			subtract(dilated_, distances_, board_points);

			return denoised_thresh;

			/*

			int threshval = 128;
			Mat marks(distances_.size(), CV_16F);

			int nLabels = connectedComponents(distances_, marks, 8);
			add(marks, 1, marks);


			for (int i = 0; i < board_points.cols; i++)
			{
				for (int j = 0; j < board_points.rows; j++)
				{
					if (board_points.at<uchar>(i, j) - '0' == 255)
						marks.at<uchar>(i, j) = 0 + '0';
				}
				cout << endl;

			}

			//imshow("mark points", marks);
			//waitKey();

			//marks[board_points == 255] = 0;
			watershed(img, marks);


			imshow("result", img);
			waitKey();

			*/

		}

	}
	catch (cv::Exception& e)
	{
		cerr << " \n\n\n\n\n\n\n\t\t EXCEPTIONS:\n" << e.msg << endl; // output exception message
	}


}