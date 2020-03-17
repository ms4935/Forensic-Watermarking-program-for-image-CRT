
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <cstdlib>
#include <bitset>

using namespace std;
using namespace cv;

Mat		EmbedWatermark(Mat& original_img, Mat& barcode);		// embed
void	ExtractWatermark(Mat& marked_img);						// extract

Mat		Embed(Mat& original_img, Mat& barcode);
float	Embed_CRT(int dc, int value);

void	Extract(Mat& Marked_Image);
float	Extract_CRT(Mat blocks);

void	WT(Mat& img, Mat& dst, int NIter);
void	IWT(Mat& dst, Mat& idst, int NIter);

void getPSNR(Mat& Marked_Img);

void getNCC();
float geStdDevValues(Mat& src);



// SVD
void calculate_SVD(Mat& src, Mat& U, Mat& S, Mat& V);

//Salt&Pepper
void SaltandPepper(Mat& img, double noise_ratio);