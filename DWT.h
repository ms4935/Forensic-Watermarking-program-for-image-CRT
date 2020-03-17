#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


float   sgn(float x);
float   soft_shrink(float d, float T);
float   hard_shrink(float d, float T);
float   Garrot_shrink(float d, float T);

static void      cvHaarWavelet(Mat &src, Mat &dst, int NIter);
static void      cvInvHaarWavelet(Mat &src, Mat &dst, int NIter, int SHRINKAGE_TYPE = 0, float SHRINKAGE_T = 50);
