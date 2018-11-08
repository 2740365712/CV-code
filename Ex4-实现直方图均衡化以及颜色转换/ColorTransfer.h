#ifndef COLOR_TRANSFER_H
#define COLOR_TRANSFER_H
#include <iostream>
#include <vector>
#include <string>
#include "CImg.h"
using namespace std;
using namespace cimg_library;

/*************************************************************

	Author: qiuyh

	Contact: 576261090@qq.com

	Date: 18/11/6

	Description: Histogram equalization on gray and color image.

**************************************************************/



class ColorTransfer
{
	int number;  // ͼ���ļ����
	int show;   // Ϊ1 ��ʾͼ��
public:
	ColorTransfer() {
		number = 0;
		show = 1;
	}
	// RGB  CImg����תlab�ռ�
	CImg<double> RGB2LAB(CImg<unsigned int> rgbImage);
	// ����ת��ͼ��ɫ�ʴ���ͼ
	CImg<double> colorTransferOnLAB(CImg<double> sourceImage, CImg<double> targetImage);
	// lab 2 lab
	CImg<unsigned int> LAB2RGB(CImg<double> labImage);
	// ��ɫ���ݽӿں���
	CImg<unsigned int> colorTransfer(string sourceImageFileName, string targetImageFileName);
};

#endif