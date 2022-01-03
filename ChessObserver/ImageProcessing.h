#pragma once

#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

cv::Mat  process(cv::Mat );
cv::Mat  findLines(cv::Mat );
cv::Mat transformImage(cv::Mat , int,int );