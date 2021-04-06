#include <cstdio>   
#include <cstring>   
#include <iostream>   
#include<cv.h>   
#include<highgui.h>   
#include <cmath>   
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iomanip>

using namespace std;
using namespace cv;



//改1
const int Width = 463;
const int Height = 370;
int Ddynamic[Width][Width];

int  main()
{
	/*Half of the window size for the census transform*/

//改2，teddy + cones:原来11,
	int hWin = 11; 


	int compareLength = (2 * hWin + 1)*(2 * hWin + 1);

	cout << "hWin: " << hWin << ";  " << "compare length:  " << compareLength << endl;
	cout << "belief propagation test" << endl;

	IplImage * leftImage = cvLoadImage("C:\\Users\\PC\\Desktop\\art-2005\\s\\view1.png", 0);
	IplImage * rightImage = cvLoadImage("C:\\Users\\PC\\Desktop\\art-2005\\s\\view5.png", 0);

	// IplImage * leftImage = cvLoadImage("left.bmp",0);                              
	// IplImage * rightImage = cvLoadImage("right.bmp",0);   

	int imageWidth = leftImage->width;
	int imageHeight = leftImage->height;



	IplImage * DPImage = cvCreateImage(cvGetSize(leftImage), leftImage->depth, 1);
	//IplImage * MatchLevelImage = cvCreateImage(cvGetSize(leftImage),leftImage->depth,1);   

	unsigned char * pPixel = NULL;
	unsigned char  pixel;
	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			pPixel = (unsigned char *)DPImage->imageData + i * DPImage->widthStep + j;
			*pPixel = 0;
		}
	}

	int minDBounds = 0;
	int maxDBounds = 31;

	cvNamedWindow("Left", 1);
	cvNamedWindow("Right", 1);
	//cvNamedWindow("Depth", 1);
	//cvNamedWindow("MatchLevel", 1);

	cvShowImage("Left", leftImage);
	cvShowImage("Right", rightImage);

	int minD = 0;
	int maxD = 31;
	//假设图像是经过矫正的，那么每次都只是需要搜搜同一行的内容   
	int max12Diff = 10;

	for (int i = 0; i < imageWidth; i++)
	{
		Ddynamic[0][i] = 0;
		Ddynamic[i][0] = 0;
	}

	unsigned char * pLeftPixel = NULL;
	unsigned char * pRightPixel = NULL;
	unsigned char leftPixel = 0;
	unsigned char rightPixel = 0;
	int m, n, l;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			for (int k = j + minD; k <= j + maxD; k++)
			{
				if (k < 0 || k >= imageWidth)
				{

				}
				else {
					pLeftPixel = (unsigned char*)leftImage->imageData + i * leftImage->widthStep + k;
					pRightPixel = (unsigned char*)rightImage->imageData + i * rightImage->widthStep + j;
					leftPixel = *pLeftPixel;
					rightPixel = *pRightPixel;

					if (abs(leftPixel - rightPixel) <= max12Diff)
					{
						Ddynamic[j + 1][k + 1] = Ddynamic[j][k] + 1;
					}
					else if (Ddynamic[j][k + 1] > Ddynamic[j + 1][k])
					{
						Ddynamic[j + 1][k + 1] = Ddynamic[j][k + 1];
					}
					else {
						Ddynamic[j + 1][k + 1] = Ddynamic[j + 1][k];
					}

					//cout<<Ddynamic[j +1][k+1]<<"  ";   
				}

			}
			//cout<<"\n";   
		}
		//逆向搜索，找出最佳路径   
		m = imageWidth;
		n = imageWidth;
		l = Ddynamic[imageWidth][imageWidth];
		while (l > 0)
		{
			if (Ddynamic[m][n] == Ddynamic[m - 1][n])
				m--;
			else if (Ddynamic[m][n] == Ddynamic[m][n - 1])
				n--;
			else
			{
				//s[--l]=a[i-1];   
				pPixel = (unsigned char *)DPImage->imageData + i * DPImage->widthStep + m;
				*pPixel = (n - m) * 8;
				l--;
				m--;
				n--;
			}
		}

		//cvWaitKey(0);   

	}

	cvShowImage("disparity", DPImage);


	//-------保存视差图-----------
//Mat src;
//src = cvarrToMat(DPImage);

//cvSaveImage("disparity.jpg",DPImage,);
//cvReleaseImage(&DPImage);

	Mat src;
	src = cvarrToMat(DPImage);

	imwrite("C:\\Users\\PC\\Desktop\\art-2005\\s\\DP.png", src);

	cvWaitKey(0);
	//cvReleaseImage(&DPImage);
	return 0;
}









