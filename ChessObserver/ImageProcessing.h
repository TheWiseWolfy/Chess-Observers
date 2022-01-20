#pragma once

#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>


struct Tile {
	bool value;
	int x;
	int y;
};

struct Line {
	cv::Point p1;
	cv::Point p2;
};


struct Polygon4 {

	cv::Point topLeft;
	cv::Point topRight;
	cv::Point bottomLeft;
	cv::Point bottomRight;

	Polygon4() {
		topLeft = cv::Point(0,0);
		topRight = cv::Point(0, 0);
		bottomLeft = cv::Point(0, 0);
		bottomRight = cv::Point(0, 0);
	};


	Polygon4(cv::Point _topLeft, cv::Point _topRight, cv::Point _bottomLeft, cv::Point _bottomRight) {
		topLeft = _topLeft;
		topRight = _topRight;
		bottomLeft = _bottomLeft;
		bottomRight = _bottomRight;
	};
};


using namespace std;
using namespace cv;

Mat  process(Mat );
Mat transformImage(Mat , int,int );

vector<Vec2f> findLines(Mat img);
vector<struct Line> point2Lines(vector<Vec2f> lines);
vector<Vec2f> lineRefinement(vector<Vec2f> lines, Mat img);
Mat displayPoints(vector<Point> points, Mat aux);
vector<Point>  intersectLines(vector<Line> lines);
Mat lineDisplay(vector<Vec2f> lines, Mat aux);
Polygon4 extremePoints(vector<Point> intersections, Size img, int offset);
void tileCutter(Mat img, Polygon4 grid[][8], Mat tiles[][8]);
void gridMake(Polygon4 ext, Polygon4 grid[][8]);
vector<vector<Point>> maxContourFinder(vector<vector<Point> > contours);
Mat tileProcessing(Mat img, Mat tiles[][8],Polygon4 grid[][8]);
bool connComponent(Mat img, int T);