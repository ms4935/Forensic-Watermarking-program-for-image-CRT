#pragma once
#include "stdafx.h"
#include "Headers.h"


void calculate_SVD(Mat& src, Mat& U, Mat& S, Mat& V)
{
	Mat S_zero = Mat(src.rows, src.cols, src.type());
	S_zero = Scalar(0);
	V = V.t();
	SVD::compute(src, S, U, V, 4);  // full SVD
	for (int i = 0; i < src.rows; i++)
		S_zero.at<float>(i, i) = S.at<float>(i, 0);

	S = S_zero;
}


 //PSNR 값 계산
void getPSNR(Mat& Marked_Img)
{
	Mat Original;
	Mat Marked;
	Mat s1;

	// Original = imread("Lena.png");		// select original image
	Original = imread("Plane.png");		// select original image
	Marked_Img.copyTo(Marked);

	absdiff(Original, Marked, s1);	// | Original - Marked |
	s1.convertTo(s1, CV_32F);		// cannot make a square on 8 bits
	s1 = s1.mul(s1);				// | Original - Marked |^2
	
	Scalar s = sum(s1);				// element sum per channel
	
	double sse = s.val[0] + s.val[1] + s.val[2];	// sum channels
	
	double mse = sse / (double)(Original.channels() * Original.total());
	double psnr = 10.0 * log10((255 * 255) / mse);
	cout << "PSNR : " << psnr << endl;
}

// NCC 값 계산
void getNCC()
{
	Mat Original_QRcode;
	Mat Extracted_QRcode;
	int x, y;
	int cnt = 0;
	//while (cnt < 3)
	//{
	Original_QRcode = imread("Original_QRCODE.png");
	/*if(cnt == 0)
	   Extracted_QRcode = imread("HH_QRcode.png");
	else if (cnt == 1)
	   Extracted_QRcode = imread("HL_QRcode.png");
	else if (cnt == 2)*/
	//Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DWT_DCT_LH_QRcode.png"); // dwt-dct
	Extracted_QRcode = imread("LH_Extracted_QRcode.png"); //dwt-dct-crt
	//Extracted_QRcode = imread("C:/Users/KJY/Desktop/result/DCT_CRT_QRcode.png");  // dct-crt

	float Value = 0.0f;
	float avgOriginal = 0.0f;
	float stdOriginal = 0.0f;
	float avgExtracted = 0.0f;
	float stdExtracted = 0.0f;

	for (y = 0; y < 100; y++)
	{
		for (x = 0; x < 100; x++)
		{
			avgOriginal += Original_QRcode.at<char>(y, x); // 원 영상의 모든 픽셀 값의
			avgExtracted += Extracted_QRcode.at<char>(y, x); // 추출된 영상의 모든 픽셀 값의 합
		}
	}
	
	avgOriginal /= 10000;
	avgExtracted /= 10000;

	stdOriginal = geStdDevValues(Original_QRcode); // 원 영상 밝기의 표준편차
	stdExtracted = geStdDevValues(Extracted_QRcode); // 추출된 영상 밝기의 표준편차

	for (y = 0; y < 100; y++)
	{
		for (x = 0; x < 100; x++)
		{
			Value += ((Original_QRcode.at<char>(y, x) - avgOriginal) * (Extracted_QRcode.at<char>(y, x) - avgExtracted)) / (stdOriginal * stdExtracted);
		}
	}

	Value /= 10000;
	/*if (cnt == 0)
	   cout << "HH NCC : " << Value << endl;
	else if (cnt == 1)
	   cout << "HL NCC : " << Value << endl;
	else if (cnt == 2)*/
	cout << "NCC : " << Value << endl;
	//   cout << "*************DWT-DCT-CRT LH NCC : " << Value << endl;
	//cout << "*************DCT-CRT NCC : " << Value << endl;
	//cnt++;
 //}
}

float geStdDevValues(Mat& src)
{
	int imageWidth = src.cols;
	int imageHeight = src.rows;

	int x, y;
	float meanValues = 0.0f;
	float tempMeanValues = 0.0f;
	float stdDevValues = 0.0f;
	float tempstdDevValues = 0.0f;

	for (y = 0; y < imageHeight; y++)
	{
		for (x = 0; x < imageWidth; x++)
		{
			float data1 = src.at<char>(y, x);
			tempMeanValues += data1;
		}
	}
	meanValues = tempMeanValues / (float)(imageHeight * imageWidth);

	for (y = 0; y < imageHeight; y++)
	{
		for (x = 0; x < imageWidth; x++)
		{
			float data2 = meanValues - src.at<char>(y, x);
			tempstdDevValues += data2 * data2;
		}
	}
	tempstdDevValues /= (float)(imageHeight * imageWidth);
	stdDevValues = (float)sqrt(tempstdDevValues);

	return stdDevValues;
}

//Salt&Pepper
void SaltandPepper(Mat& img, double noise_ratio) {
	int rows = img.rows;
	int cols = img.cols;
	int ch = img.channels();
	int num_of_noise_pixels = (int)((double)(rows*cols*ch)*noise_ratio);

	for (int i = 0; i < num_of_noise_pixels; i++) {
		int r = rand() % rows;
		int c = rand() % cols;
		int _ch = rand() % ch;
		uchar* pixel = img.ptr<uchar>(r) + (c*ch) + _ch;
		*pixel = (rand() % 2 == 1) ? 255 : 0;
	}
}