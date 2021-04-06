#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iomanip>


using namespace std;
using namespace cv;


class SAD
{
public:
	SAD(int _winSize, int _maxDis) :winSize(_winSize), maxDis(_maxDis) {}
	Mat compSAD(Mat&L, Mat&R);//计算SAD
private:
	int winSize;//卷积核尺寸
	int maxDis;//最大视差，即视差搜索范围
};

Mat SAD::compSAD(Mat&L, Mat&R)
{
	int Height = L.rows;
	int Width = L.cols;
	Mat Kernel_L(Size(winSize, winSize), CV_8U, Scalar::all(0));//CV_8U:表示每个像素对应1字节，无符号，范围为0~255的值，设置初始像素值全为零
	Mat Kernel_R(Size(winSize, winSize), CV_8U, Scalar::all(0));
	Mat Disparity(Height, Width, CV_8U, Scalar(0));

	for (int i = 0; i < Width - winSize; ++i)
	{
		for (int j = 0; j < Height - winSize; ++j)
		{
			Kernel_L = L(Rect(i, j, winSize, winSize));//在左图内取边长为winSize的卷积核
			Mat sad_Vec(1, maxDis, CV_32F, Scalar(0));//定义绝对值和向量，用于存放绝对值和

			for (int k = 0; k < maxDis; ++k) {
				int x = i - k;
				if (x >= 0)
				{
					Kernel_R = R(Rect(x, j, winSize, winSize));//在右图内取边长为winSize的卷积核
					Mat Dif;
					absdiff(Kernel_L, Kernel_R, Dif);//两卷积核对应元素相减，取绝对值，相减后的形成的矩阵元素每列相加，将结果赋值给Dif列向量
					Scalar ADD = sum(Dif);//将列向量中的所有元素值相加，结果赋值给ADD
					//列向量就是代价矩阵d轴的那一条，

					float a = ADD[0];
					sad_Vec.at<float>(k) = a;//将绝对值和赋值给绝对值和向量中的第k个元素
				}
			}
			Point minLoc;//定义point对象
			minMaxLoc(sad_Vec, NULL, NULL, &minLoc, NULL);
			//视差计算了：将sad_Vec中对应绝对值最小值的索引赋值给minLoc

			int loc = minLoc.x;//读取索引中的横坐标，即绝对值和最小时的列数，即虚拟视差k
			Disparity.at<char>(j, i) = loc * 15;//为了使视差图对比度更大，更明显，将每个视差元素扩大15倍

			//double rate = double(i) / (Width);//记录完成度
			//cout << "已完成" << setprecision(2) << rate * 100 << "%" << endl;//输出窗口会显示已完成的进度
		}
	}
	return Disparity;
}

int main()
{
	Mat left = imread("C:\\Users\\PC\\Desktop\\art-2005\\view1.png", CV_LOAD_IMAGE_GRAYSCALE);//读取图片，并转换为灰度图像
	Mat right = imread("C:\\Users\\PC\\Desktop\\art-2005\\view5.png", CV_LOAD_IMAGE_GRAYSCALE);
	//-------图像显示-----------
	namedWindow("leftimag");
	imshow("leftimag", left);

	namedWindow("rightimag");
	imshow("rightimag", right);
	//--------由SAD求取视差图-----
	Mat Disparity;
	//Mat Disparity1;
	SAD mySAD(7, 34); //原来是,cones:7, 35  teddy-2003:7, 30
	Disparity = mySAD.compSAD(left, right);
	//medianBlur(Disparity, Disparity1,3);//代价聚合后处理
	
	//-------结果显示------
	namedWindow("Disparity");
	imshow("Disparity", Disparity);

	//-------保存视差图------
	imwrite("C:\\Users\\PC\\Desktop\\art-2005\\SAD.png", Disparity);

	waitKey(0);
	return 0;
}