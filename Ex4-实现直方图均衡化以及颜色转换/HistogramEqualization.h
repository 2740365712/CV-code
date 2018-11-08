#ifndef  HISTOGRAM_EQUALIZATION_H
#define  HISTOGRAM_EQUALIZATION_H

#include <iostream>
#include <vector>
#include <string>
#include "CImg.h"
using namespace std;
using namespace cimg_library;

/*************************************************************

	Author: qiuyh

	Contact: 576261090@qq.com

	Date: 18/11/5

	Description: Histogram equalization on gray and color image.

**************************************************************/


class HistogramEqualization
{
	int number; // �ļ����
	int show;   // Ϊ1��ʾͼƬ  
public:
	HistogramEqualization() {
		number = 0;
		show = 1;
	}

	/*����ͼ��ĻҶ�ֱ��ͼ*/
	CImg<unsigned int> DrawHistogram(CImg<unsigned int> image);

	/*��ɫͼתΪ�Ҷ�ͼ*/
	CImg<unsigned int> Rgb2Gray(CImg<unsigned int> RGBImage);

	/*ֱ��ͼ���Ҷ�ֵ������������*/
	CImg<unsigned int> GetHistogram(CImg<unsigned int> InputImage);

	/*ֱ��ͼ���⻯*/
	CImg<unsigned int> HistogramEqualizationMethod(CImg<unsigned int> InputImage, CImg<unsigned int>  Histogram);

	/*�Ҷ�ͼ���⻯�ӿں���*/
	CImg<unsigned int> HistogramEqualizationOnGrayImage(string ImageFileName);

	/*��ɫͼ���⻯-���ڵ���ɫͨ���ķ���*/
	CImg<unsigned int> Hist_Equal_ColorImage_OneColorChannel(string ImageFileName);

	/* ����õ�����ͨ����ƽ��ֱ��ͼ*/
	CImg<unsigned int> GetAverageHistogram(CImg<unsigned int> img1, CImg<unsigned int> img2, CImg<unsigned int> img3);

	/*��ɫͼ���⻯-����������ɫͨ��ƽ���Ҷ�ֱ��ͼ�ķ���*/
	CImg<unsigned int> Hist_Equal_ColorImage_ThreeColorChannels(string ImageFileName);

	/*��ɫͼ���⻯-����HSI�ռ��µ�ֱ��ͼ����*/
	CImg<double> Hist_Equal_ColorImage_HSISpace(string ImageFileName);
};
#endif // ! HISTOGRAM_EQUALIZATION_H

