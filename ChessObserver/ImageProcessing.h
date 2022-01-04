#pragma once

#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

struct Line {
	cv::Point p1;
	cv::Point p2;
};


using namespace std;
using namespace cv;

Mat  process(Mat );
Mat transformImage(Mat , int,int );

vector<Vec2f> findLines(Mat img);
vector<struct Line> point2Lines(vector<Vec2f> lines);

vector<Point>  intersectLines(vector<Line> lines);
Mat lineDisplay(vector<Vec2f> lines, Mat aux);

vector<vector<Point>> maxContourFinder(vector<vector<Point> > contours);