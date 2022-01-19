#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <limits>

#include "ImageProcessing.h"

#include <set>

bool debugMode = true;

//this function will be using for detecting and isolating 
//the chessboard we have for an easy processing of the data on it
Mat transformImage(Mat img, int minimumThreshold, int maximumThreshold) {

	RNG rng(12345);
	Mat gray_img, canny_img, masked_img, destination;
	vector<vector<Point> > contours;
	vector<vector<Point> > cont_max;
	vector<Point2f> corners;
	vector<Vec4i> hierarchy;

	//some pre processing for accurate results
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	equalizeHist(gray_img, gray_img);
	GaussianBlur(gray_img, gray_img, Size(5, 5), 0);

	//Here we apply the canny transform
	Canny(gray_img, canny_img, minimumThreshold, maximumThreshold);

	//Here we apply erode and dilate 
	Mat element = getStructuringElement(0, Size(2 * 0 + 1, 2 * 0 + 1), Point(0, 0));
	erode(canny_img, canny_img, element);
	dilate(canny_img, canny_img, Mat(), Point(-1, -1), 1);

	//finding contours and drawing them
	findContours(canny_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	Mat mask = Mat::zeros(img.size(), CV_8UC3);

	if (debugMode) {
		for (size_t i = 0; i < cont_max.size(); i++) {
			Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

		}

	}
	cont_max = maxContourFinder(contours);
	drawContours(mask, cont_max, 1, Scalar(255, 255, 255), cv::FILLED);


	masked_img = Mat::zeros(img.size(), CV_8UC3);
	img.copyTo(masked_img, mask);   //Here we apply the mask to the image

	if (debugMode) {
		//imshow("Image Mask", mask);
		//imshow("Masked Image ", masked_img);

	}

	//We return the image but only the chess table
	return masked_img;

}

//??
vector<Vec2f> lineRefinement(vector<Vec2f> lines, Mat img) {

	Mat aux = Mat::zeros(img.size(), img.type());

	aux = lineDisplay(lines, aux);

	Mat elementDilate = getStructuringElement(0, Size(10, 10), Point(0, 0));
	Mat elementErode = getStructuringElement(0, Size(15, 15), Point(0, 0));

	dilate(aux, aux, elementDilate, Point(-1, -1), 1);
	erode(aux, aux, elementErode);

	Mat edge = Mat(img.size(), CV_8UC1);
	Mat cdst;

	Canny(aux, edge, 100, 200);
	cvtColor(edge, cdst, COLOR_GRAY2BGR);
	HoughLines(edge, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection

	aux = lineDisplay(lines, aux);

	//imshow("Line refinement", aux);

	return lines;

}

vector<Vec2f> findLines(cv::Mat img) {
	Mat edge = Mat(img.size(), CV_8UC1);
	Mat cdst;
	vector<Vec2f> lines; // will hold the results of the detection

	Canny(img, edge, 100, 200);
	cvtColor(edge, cdst, COLOR_GRAY2BGR);
	HoughLines(edge, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection

	return lines;
}

vector<struct Line> point2Lines(vector<Vec2f> lines) {

	float aux_angle, aux_len;
	struct Line aux_line;
	Point pt1, pt2;
	vector<Line> points;
	for (size_t i = 0; i < lines.size(); i++)
	{
		aux_len = lines[i][0];
		aux_angle = lines[i][1];

		double a = cos(aux_angle);
		double b = sin(aux_angle);

		double x0 = a * aux_len;
		double y0 = b * aux_len;

		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		aux_line.p1 = pt1;
		aux_line.p2 = pt2;

		points.push_back(aux_line);
	}

	return points;

}


vector<Point>  intersectLines(vector<Line> lines) {
	// a line is defined by 2 points
	vector<Point> intersections;

	//this code checks if two lines have the same angle
	//if not an intersection point is created and added in the intersections vector

	double m1, m2;
	double c1, c2;

	for (size_t i = 0; i < lines.size(); i++) {
		for (size_t j = i + 1; j < lines.size(); j++) {

			m1 = numeric_limits<double>::max();
			m2 = numeric_limits<double>::max();

			//first line
			Point X1, X2;
			X1.x = lines[i].p1.x;	X1.y = lines[i].p1.y;
			X2.x = lines[i].p2.x;	X2.y = lines[i].p2.y;

			if (abs(X1.x - X2.x) > 0.001) {    //prima linie nu e orizontala
				m1 = ((double)X2.y - X1.y) / ((double)X2.x - X1.x);
				c1 = X1.y - m1 * X1.x;

			}

			//second line
			Point X3, X4;
			X3.x = lines[j].p1.x;	X3.y = lines[j].p1.y;
			X4.x = lines[j].p2.x;	X4.y = lines[j].p2.y;

			if (abs(X3.x - X4.x) > 0.001) {   //a doua linie e orizontala
				m2 = (X4.y - X3.y) / (X4.x - X3.x);
				c2 = X3.y - m2 * X3.x;

			}

			if (m1 != m2) {
				if (m1 != numeric_limits<double>::max() && m2 != numeric_limits<double>::max()) {
					Point inter;

					if (m2 < m1) {
						inter.x = (c2 - c1) / (m1 - m2);
						inter.y = m1 * inter.x + c1;

					}
					else if (m2 > m1) {
						inter.x = (c1 - c2) / (m2 - m1);
						inter.y = m2 * inter.x + c2;

					}

					intersections.push_back(inter);

				}
				else if (m1 = numeric_limits<double>::max() && m2 != numeric_limits<double>::max()) { //Prima dreapta e orizontala
					Point inter;
					inter.x = X1.x;
					inter.y = m2 * inter.x + c2;
					intersections.push_back(inter);

				}
				else if (m2 = numeric_limits<double>::max() && m1 != numeric_limits<double>::max()) {
					Point inter;
					inter.x = X3.x;
					inter.y = m1 * inter.x + c1;
					intersections.push_back(inter);

				}
			}

		}

	}
	return intersections;


}


Polygon4 extremePoints(vector<Point> intersections_raw, Size img, int offset)
{
	//theoretical ideal opposite to what the result should be like
	//example :
	//top left should be like (0,0) and the search starts at (inf,inf)
	Point topL = Point(numeric_limits<int>::max(), numeric_limits<int>::max());
	Point topR = Point(0, numeric_limits<int>::max());
	Point bottomL = Point(numeric_limits<int>::max(),0);
	Point bottomR = Point(0,0);

	vector<Point> intersections;
	//removing the points that are in the offset area from the intersections vector
	for(int i = 0 ; i < intersections_raw.size();i++)
	{
		//condition to add a point
		if(	intersections_raw[i].x > offset && 
			intersections_raw[i].x < img.width - offset &&
			intersections_raw[i].y >offset &&
			intersections_raw[i].y < img.height - offset)
			intersections.push_back(intersections_raw[i]);

	}

	for(int i = 0 ; i < intersections.size(); i++)
	{
		//looking for the top left point
		if(intersections[i].x <= topL.x && intersections[i].y <= topL.y)
			topL = intersections[i];

		//looking for the top right point
		else if(intersections[i].x >= topR.x && intersections[i].y <= topR.y)
			topR = intersections[i];

		//looking for the bottom left point
		else if(intersections[i].x <= bottomL.x && intersections[i].y >= bottomL.y)
			bottomL = intersections[i];
		
		//looking for the bottom right point
		else if(intersections[i].x >= bottomR.x && intersections[i].y >= bottomR.y)
			bottomR = intersections[i];
		
	}

	Polygon4 extremes(topL, topR, bottomL, bottomR);
	
	return extremes;
}

void gridMake(Polygon4 ext, Polygon4 grid[][8]){

	int segmentWidthTop = (ext.topRight.x - ext.topLeft.x)  / 8;
	int segmentWidthBottom = (ext.bottomRight.x - ext.bottomLeft.x) / 8 ;

	//<--
	int segmentHeightLeft = (ext.bottomLeft.y - ext.topLeft.y) / 8 ;
	//-->
	int segmentHeightRight = (ext.bottomRight.y - ext.topRight.y) / 8;


	for (int x = 0; x < 8; ++x) { //x = 0
		for (int y = 0; y < 8; ++y) { //y = 0

			//Incepem din coltul sus-stanga
			int interpolatedWidth = ( segmentWidthTop * (8 - x) + segmentWidthBottom * x ) / 8;
			int interpolatedHeight =( segmentHeightLeft * (8 - x) + segmentHeightRight * x ) / 8;

			Point refference_point;
			refference_point.x = x * interpolatedWidth + ext.topLeft.x ;
			refference_point.y = y * interpolatedHeight + ext.topLeft.y ;

			//Aici avem colturile fiecarui poligon
			Point top_left = refference_point;
			Point top_right = refference_point + Point(interpolatedWidth, 0);
			Point bot_left = refference_point + Point(0, interpolatedHeight);
			Point bot_right = refference_point + Point(interpolatedWidth, interpolatedHeight);

			grid[x][y] = Polygon4(top_left, top_right, bot_left, bot_right);
		}
	}

	return;

}


void tileCutter(Mat img, Polygon4 grid[][8], Mat tiles[][8] ) {

	for (int x = 0; x < 8; ++x) { //x = 0
		for (int y = 0; y < 8; ++y) { //y = 0
			Polygon4 currentPolygon = grid[x][y];

			int tileHeight = currentPolygon.bottomLeft.y - currentPolygon.topLeft.y;
			int tileWidth = currentPolygon.topRight.x - currentPolygon.topLeft.x;

			if(tileHeight > 0 && tileWidth > 0){
				Rect crop_region(currentPolygon.topLeft.x , currentPolygon.topLeft.y, tileWidth, tileHeight);


				Mat tile = img(crop_region);
				tiles[x][y] = tile;
			}
		}
	}
}



void emptyTileFounder( Mat tiles[][8] ) {


	Mat tileHSV;
	cvtColor(tiles[0][0], tileHSV, COLOR_BGR2HSV);

	//values used to calculate the histogram
	int h_bins = 50, s_bins = 60;
	int histSize[] = { h_bins, s_bins };
	// hue varies from 0 to 179, saturation from 0 to 255

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
	// Use the 0-th and 1-st channels
	int channels[] = { 0, 1 };
	Mat tile_histogram;

	calcHist(&tileHSV, 1, channels, Mat(), tile_histogram, 2, histSize, ranges, true, false);
	normalize(tile_histogram, tile_histogram, 0, 1, NORM_MINMAX, -1, Mat());

	imshow("scared", tile_histogram);

}





//void analizeTile(Polygon4 tile,Mat img)
//{
//	for(int x = tile.topLeft.x ; x < tile.topRight.x; x++)
//	{
//		for(int y = tile.topLeft.y ; y < tile.bottomLeft.y; y++ )
//		{
//			
//
//		}
//	}
//
//
//}
//
//
//void analizeGrid(Polygon4 grid[][8],Mat img)
//{
//	for(int x = 0 ; x < 8; x++){
//		for(int y = 0 ; y < 8; y++)
//		{
//			//analizing region at the position
//			analizeTile(grid[x][y],img);
//
//
//		}
//	}
//
//
//
//}





Mat displayPoints(vector<Point> points, Mat aux) {
	for (size_t i = 0; i < points.size(); i++) {
		circle(aux, points[i], 2, Scalar(0, 0, 255), 5, 1);

	}
	return aux;

}


Mat lineDisplay(vector<Vec2f> lines, Mat aux) {
	vector<Line> auxLines = point2Lines(lines);

	for (size_t i = 0; i < lines.size(); i++) {
		line(aux, auxLines[i].p1, auxLines[i].p2, Scalar(255, 255, 255), 3, LINE_AA);

	}
	return aux;

}

//vector<Vec2f> likeRefinement(vector<Vec2f> lines) {
//
//	vector<Vec2f> newlines;
//	bool changed = true;
//
//	float thetaError = 0.6f;//??
//	float rhoError = 40.0f;
//
//	int maxIter = 1;
//	int iter = 0;
//
//	while (changed && iter < maxIter){
//		newlines.clear();
//		changed = false;
//		iter++;
//
//		for (size_t i = 0; i < lines.size(); i++) {
//			float rho1 = lines[i][0];
//			float theta1 = lines[i][1];
//			for (size_t j = i + 1; j < lines.size(); j++) {
//				float rho2 = lines[j][0];
//				float theta2 = lines[j][1];
//
//				if (abs(theta1 - theta2) < thetaError && abs(rho1 - rho2) < rhoError) {
//					changed = true;
//
//					Vec2f avarage = Vec2f((rho1 + rho2) / 2, (theta1 + theta2) / 2);  //Omori linile din care se nasc medile
//					newlines.push_back(avarage);
//				}
//			}
//		}
//		lines.clear();
//		lines = vector<Vec2f>(newlines.begin(), newlines.end());
//	}
//
//	return lines;
//}

vector<vector<Point> > maxContourFinder(vector<vector<Point> > contours) {
	vector<Point> hull, aux; //used for the creation of the final contour
	int maxContour_i = 0;
	double maxArea, newArea;

	//convex hull creates a polygon that is surrounding the contour
	convexHull(contours[0], hull);
	maxArea = contourArea(hull);

	//comparing the areas between the hulls that are found to determine the largest object in the image
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

	//the function returns the contour that corresponds to the hull
	vector<vector<Point> > output;
	output.push_back(contours.at(maxContour_i));
	output.push_back(hull);
	return  output;

}

