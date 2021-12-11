#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "ImageProcessing.h"

using namespace std;
using namespace cv;

vector<vector<Point> > maxContourFinder(vector<vector<Point> > contours)
{
	vector<Point> hull, aux;//used for the creation of the final contour
	int maxContour_i = 0;
	double maxArea, newArea;

	convexHull(contours[0], hull);
	maxArea = contourArea(hull);

	for (int i = 1; i < contours.size(); i++)
	{
		convexHull(contours[i], aux);
		newArea = contourArea(aux);
		if (newArea > maxArea)
		{
			hull = aux;
			maxArea = newArea;
			maxContour_i = i;
		}
	}

	vector<vector<Point> > output;
	output.push_back(contours.at(maxContour_i));
	output.push_back(hull);
	return  output;

}


vector<Point2f> selectPoints(vector<Point2f> corners){
    ///this function is for finding the corner points in the image ( intersections of lines)
    vector<Point2f> res;
    Point2f p;

    int nx,ny;
	int ov = 20;
	int ev = 100;
    float x,y,x2,y2;

    for(int i=0;i<corners.size();i++){
        
		nx = 0;
        ny = 0;
        p = corners[i];
        x = p.x;
        y = p.y;

        for(int j=0;j<corners.size();j++)
		{
            
			x2 = corners[j].x;
            y2 = corners[j].y;
            if(abs(x-x2) < ev)
                nx++;
            if(abs(y-y2) < ev)
                ny++;
        }
		//selecting things that have over ov overlaps
        if(nx > ov && ny > ov)
            res.push_back(p);
    }
    return res;
}


Mat transformImage(Mat img, int minimumThreshold, int maximumThreshold)
{

//this function will be using for detecting and isolating 
//the chessboard we have for an easy processing of the data on it
 
	RNG rng(12345);
	Mat gray_img, canny_img, drawing,masked_img,destination;
	vector<vector<Point> > contours;
	vector<vector<Point> > cont_max;
	vector<Point2f> corners;
	vector<Vec4i> hierarchy;

	//some pre processing for accurate results
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	equalizeHist(gray_img, gray_img);

	GaussianBlur(gray_img, gray_img, Size(3, 3), 0);
	Canny(gray_img, canny_img, minimumThreshold, maximumThreshold);

	//erode(canny_img, canny_img, Mat(), Point(-1, -1), 1);

	Mat element = getStructuringElement(0, Size(2 * 0 + 1, 2 * 0 + 1), Point(0, 0));
	erode(canny_img, canny_img, element);
	dilate(canny_img, canny_img, Mat(), Point(-1, -1), 1);

	drawing = Mat::zeros(canny_img.size(), CV_8UC3);


	//finding contours and drawing them
	findContours(canny_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	cont_max = maxContourFinder(contours);
	Mat mask = Mat::zeros(img.size(), CV_8UC3);

	for (size_t i = 0; i < cont_max.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(mask, cont_max, (int)i, Scalar(255, 255, 255), cv::FILLED);
	}

	masked_img = Mat::zeros(img.size(), CV_8UC3);
	img.copyTo(masked_img, mask);


	imshow("Image Mask", mask);
	imshow("Masked Image ", masked_img);

	
 	destination = masked_img.clone();
	mask = Mat::zeros(masked_img.rows,masked_img.cols,CV_8UC1);
    //goodFeaturesToTrack(destination,corners,90,0.01,10,Mat(),3,false,0.04 ); //apply Shi-Thomasi corner detection
    //lock = selectPoints(corners);
    cvtColor(mask,mask,COLOR_GRAY2BGR);
	
	imshow("point", mask);
	
	//imshow("canny", canny_img);
   // imshow("contours", drawing);

	return masked_img;
}

vector<Vec2f> lineRefinement(vector<Vec2f> lines)
{
	for(int i  = 0 ; i < lines.size() ; i++)
	{
		//do a monke flip

	}

	return lines;
}

Mat findLines(Mat img) {

	Mat edge = Mat(img.size(), CV_8UC1);
	Mat cdst;
	vector<Vec2f> lines; // will hold the results of the detection

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


	HoughLines(edge, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection

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

	imshow("Imagine cu linii", aux);

	return cdst;
}
