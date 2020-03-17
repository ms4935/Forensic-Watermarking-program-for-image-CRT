#pragma once
#include "stdafx.h"
#include "Headers.h"
#define EMBED_VALUE 45
#define DC_BOUNDARY 4080
#define m 38
#define n 107
#define D 106
//////////////////////////////////////////////////////////////////////////////////////////
//// 삽입
//////////////////////////////////////////////////////////////////////////////////////////
Mat EmbedWatermark(Mat& HostImg, Mat& QrImg)
{
	return Embed(HostImg, QrImg);
}

/* DWT-DCT-CRT */
float Embed_CRT(int dc, int value)
{
	int Z = dc; // dc값 가져옴
	int p = Z % m;
	int q = Z % n;

	int d = abs(p - q);
	int b = p + q;

	//Watermark bit = '0'
	if (value == -3) // black
	{
		if (b < (D + 35) / 2) // d < D/2       b < (D+35) / 2
		{
			return (float)Z;
		}
		else
		{
			for (int j = 8; j < 256; j = j + 8)
			{
				if (Z + j < DC_BOUNDARY)
				{
					p = (Z + j) % m;
					q = (Z + j) % n;
					d = abs(p - q);
					b = p + q;
					if (b < (D + 35) / 2)
					{
						Z = Z + j;
						return (float)Z;
						break;
					}
				}
				if (Z - j > 0)
				{
					p = (Z - j) % m;
					q = (Z - j) % n;
					d = abs(p - q);
					b = p + q;
					if (b < (D + 35) / 2)
					{
						Z = Z - j;
						return (float)Z;
						break;
					}
				}
			}
		}
	}
	else
	{
		if (b >= (D + 35) / 2) // b >= (D+35) / 2    d >= D / 2
		{
			return (float)Z + EMBED_VALUE;
		}
		else
		{
			for (int j = 8; j < 256; j = j + 8)
			{
				if (Z + j < DC_BOUNDARY)
				{
					p = (Z + j) % m;
					q = (Z + j) % n;
					d = abs(p - q);
					b = p + q;
					if (b >= (D + 35) / 2)
					{
						Z = Z + j;
						return (float)Z + EMBED_VALUE;
						break;
					}
				}
				if (Z - j > 0)
				{
					p = (Z - j) % m;
					q = (Z - j) % n;
					d = abs(p - q);
					b = p + q;
					if (b >= (D + 35) / 2)
					{
						Z = Z - j;
						return (float)Z + EMBED_VALUE;
						break;
					}
				}
			}
		}
	}

}

Mat Embed(Mat& HostImg, Mat& QrImg)
{
	Mat yuv;
	vector<Mat> yuv_arr(3);
	Mat WT_result;
	Mat IWT_result;
	Mat Qr_Pixel = Mat(QrImg.rows, QrImg.cols, QrImg.type()); // 32x32 QRcode 각 픽셀 값을 255, 0으로 저장할 행렬 변수 생성

	// QR의 데이터를 0과 255로 설정
	for (int y = 0; y < QrImg.rows; y++)
	{
		for (int x = 0; x < QrImg.cols; x++)
		{
			Qr_Pixel.at<uchar>(y, x) = ((int)QrImg.at<uchar>(y, x) > 125) ? 255 : 0;
		}
	}

	cvtColor(HostImg, yuv, COLOR_RGB2YCrCb);    // RGV to YCrCb
	split(yuv, yuv_arr); // 채널 분리

	WT(yuv_arr[0], WT_result, 1); // Y채-널을 대상으로 1단계 DWT 진행

	// 부대역의 계수를 저장할 행렬 변수    
    //Mat HH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
    //Mat HL_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());
	Mat LH_subband = Mat(WT_result.cols / 2, WT_result.rows / 2, WT_result.type());

	 //HH_subband = WT_result(Rect(WT_result.cols / 2, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2));
	 //HL_subband = WT_result(Rect(WT_result.cols / 2, 0, WT_result.cols / 2, WT_result.rows / 2));
	 LH_subband = WT_result(Rect(0, WT_result.rows / 2, WT_result.cols / 2, WT_result.rows / 2)); // real LH
     ///LL_subband = WT_result(Rect(0, 0, WT_result.rows / 2, WT_result.cols / 2)); // LL

	// DCT를 진행할 8x8 크기의 블럭들
	Size blockSize(8, 8);
	//vector<Mat> HH_blocks; // 각 부대역의 블럭들
	//vector<Mat> HL_blocks;
	vector<Mat> LH_blocks;
	int value[1024]; // QR의 삽입 값을 저장할 배열
	int i = 0;
	// 256x256 크기의 부대역을 1024개의 8x8 블럭 사이즈로 분할
	for (int y = 0; y < 256; y += blockSize.height)
	{
		for (int x = 0; x < 256; x += blockSize.width)
		{
			Rect rect = Rect(x, y, blockSize.width, blockSize.height);
			//HH_blocks.push_back(Mat(HH_subband, rect));
			//HL_blocks.push_back(Mat(HL_subband, rect));
			LH_blocks.push_back(Mat(LH_subband, rect));
			// QR의 삽입 값을 지정
			value[i++] = ((int)Qr_Pixel.at<uchar>((int)(y / 8), (int)(x / 8)) > 125 ? 3 : -3);
		}
	}

	// 1024개의 8*8 블록에 dct 적용
	int enter = 0;
	for (int i = 0; i < 1024; i++)
	{
		//dct(HL_blocks[i], HL_blocks[i]);
		//dct(HH_blocks[i], HH_blocks[i]);
		dct(LH_blocks[i], LH_blocks[i]);
		enter++;
		if (enter == 32) {
			enter = 0;
		}
	}

	// 각 부대역의 1024개의 8*8 블럭들을 대상으로 워터마크 데이터 삽입 진행
	//1024개의 각 8*8블럭 DC계수(Z)에 CRT 적용하여 워터마크 삽입
	enter = 0;
	for (int i = 0; i < 1024; i++)
	{
		//HH_blocks[i].at<float>(0, 0) = Embed_CRT((int)HH_blocks[i].at<float>(0, 0), value[i]);
		//HL_blocks[i].at<float>(0, 0) = Embed_CRT((int)HL_blocks[i].at<float>(0, 0), value[i]);
		LH_blocks[i].at<float>(0, 0) = Embed_CRT((int)LH_blocks[i].at<float>(0, 0), value[i]);
		//      cout << (int)LH_blocks[i].at<float>(0, 0) << " ";


		enter++;
		if (enter == 32) {
			enter = 0;
		}
		//dct(HH_blocks[i], HH_blocks[i], DCT_INVERSE);
		//dct(HL_blocks[i], HL_blocks[i], DCT_INVERSE);
		dct(LH_blocks[i], LH_blocks[i], DCT_INVERSE);
	}

	// IWT 수행
	IWT(WT_result, IWT_result, 1);
	IWT_result.convertTo(yuv_arr[0], CV_8U);
	merge(yuv_arr, yuv);

	cvtColor(yuv, yuv, COLOR_YCrCb2RGB); // YCrCb to RGB

	// 압축률 지정을 위한 부분
	vector<int> param75 = vector<int>(2);
	param75[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param75[1] = 75;//default(95) 0-100

	vector<int> param95 = vector<int>(2);
	param95[0] = 1;// CV_IMWRITE_JPEG_QUALITY;
	param95[1] = 95;//default(95) 0-100

	imwrite("Marked_image_CRT.png", yuv);
	imwrite("Marked_image_CRT_75.jpg", yuv, param75);
	imwrite("Marked_image_CRT_95.jpg", yuv, param95);

	////////////////////// 압축 공격 이외의 공격 ///////////////////////////
/*	Mat attack = imread("[blackgreygradient]DWT_DCT_CRT_LH.png");
	Mat GblurredImg1, GblurredImg2, Ablur, Mblur;
	Mat SPimg;

	GaussianBlur(attack, GblurredImg1, Size(3, 3), 0);
	GaussianBlur(attack, GblurredImg2, Size(5, 5), 0);
	blur(attack, Ablur, Size(3, 3));                  //Average blurring
	medianBlur(attack, Mblur, 3);

	SPimg = attack.clone();
	SaltandPepper(SPimg, 0.03);

	imwrite("2Gaussian Blurring 3x3.png", GblurredImg1);
	imwrite("2Gaussian Blurring 5x5.png", GblurredImg2);
	imwrite("2Average Blurring 3x3.png", Ablur);
	imwrite("2Median Blurring 3x3.png", Mblur);
	imwrite("2Salt and Pepper 3%.png", SPimg);

*/
	return yuv;
}