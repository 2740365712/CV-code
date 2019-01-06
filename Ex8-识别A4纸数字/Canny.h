#pragma once
/*****************************************************************
Author: qiuyh
Date: 2018/10/12
Purpose: ʵ�� canny-edge-detector
*******************************************************************/

#ifndef CANNY_H
#define CANNY_H
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "CImg.h"
#include <string>
using namespace std;
using namespace cimg_library;
#define PI 3.141592
#define POSSIBLEEDGE 255
#define NOTEDGE 0


class Canny {
private:
	string inputFileName;                    /*ԴͼƬ�ļ�������*/
	char output[100];                        /*����������ͼƬ����*/

	float sigma;                             /*��˹�˲���׼��*/
	float tLow;	                             /*ȷ������ֵ�Ĳ���*/
	float tHigh;                             /*ȷ������ֵ�Ĳ���*/

	int highthreshold;	                     /*����ֵ*/
	int lowthreshold;                        /*����ֵ*/

	float  BOOSTBLURFACTOR;                  // ����ģ������
	int width;                               /*����ͼ��Ŀ�*/
	int height;                              /*����ͼ��ĸ�*/

	CImg<short int> SrcImage;                /*��ʼͼ��*/
	CImg<short int> grayImage;               /*��ʼ�Ҷ�ͼ����*/
	CImg<short int> smoothImage;		     /*��˹ƽ����ͼ�����*/
	CImg<short int> deltaXImage;             /*x����ƫ��ͼ�����*/
	CImg<short int> deltaYImage;             /*y����ƫ��ͼ�����*/
	CImg<short int> dirRadianImage;          /*�ݶȷ���ͼ�����*/
	CImg<short int> magnitudeImage;          /*�ݶȷ�ֵͼ�����*/
	CImg<short int> nonMaxSuppImage;         /*��������ƺ��ͼ�����*/
	CImg<short int> resultEdgeImage;         /*��Ե�����ͼ�����*/
	CImg<short int> cutShortEdgeImage;       /*ɾ���̱�Ե���ͼ��*/
	bool DEBUG;                              /*��Ϊ�棬����debugģʽ����ӡ��Ϣ*/
	int number;                              /*һ��canny�����ļ������*/
										     // ����һ�������
	struct XY {
		int col;
		int row;
	};

public:
	// ��ʼ���࣬��Ҫ�Ǽ��Ĳ���
	Canny() {
		DEBUG = false;
		sigma = 1;   // sig  1.0 ~ 1.4
		number = 1;
		tLow = 10;   // 6 7
		tHigh = 60;
		BOOSTBLURFACTOR = 0.2485;  // 0.28 // sobel ���� 6 // 0.35
		//����ͼ������г�ʼ��
	}
	
	// nΪ���ӡ��Ϣ
	void ForDebug(bool n) {
		DEBUG = n;  
	}
	
	// �޸ı�׼��
	void setSigma(float n) {
		sigma = n;
	}
	// �޸ĸߵ���ֵ
	void setTLowandTHigh(float thigh, float tlow) {
		tHigh = thigh;
		tLow = tlow;
	}
	// �޸ĸ�˹ģ������
	void setBoost(float h) {
		BOOSTBLURFACTOR = h;
	}

	/*��ȡpgm�Ҷ�ͼ,��ʵ���ȡһ����ɫ��BMPͼ��inputFileNameȻ����ת��Ϊ�Ҷ�ͼpgm��ʽ��image*/
	int readGrayImage(string inputFileName);

	/*д��bmp�Ҷ�ͼ��������Ե��Ľ�����뵽���ͼ���У���ʾ*/
	int writeGrayImage(string outputFileName, CImg<short int> image);

	/*��˹ƽ�����ԻҶ�ͼ���н��룬ȥ����Ƶ���������˴�ʹ�ÿɷ���ĸ�˹�˲�����������������������*/
	void gaussianSmooth(CImg<short int>* image,float sigma, CImg<short int>* smootheDim);

	/*�����˹��*/
	void makeGaussianKernel(float sigma, float **kernel, int *windowsize);

	/*����xy����ĵ����� ���x, y�����ƫ����ͼ*/
	void derrivativeXY(CImg<short int> *smootheDim, 
		CImg<short int> * delta_x, CImg<short int>  * delta_y);

	/*���� �ݶȷ����ˮƽ�н�*/
	double angle_radians(double x, double y);

	/* �����ݶȷ�������ݶȷ���ͼ*/
	void radianDirection(CImg<short int> *delta_x,CImg<short int>* delta_y,
		CImg<short int>* dir_radians, int xdirtag, int ydirtag);

	/*����xy�ݶȷ�ֵ*/
	void magnitudeXY(CImg<short int>* delta_x, 
		CImg<short int>* delta_y, CImg<short int>* magnitude);

	/* ���������*/
	void nonMaxSuppression(CImg<short int>* magnitude,
		CImg<short int>* delta_x, CImg<short int>* delta_y, CImg<short int> *nonMaxSupp);
	
	/*�ͺ���ֵ��*/
	void applyHysteresis(CImg<short int> *magnitude, CImg<short int> *nonMaxSupp,
		float tlow, float thigh, CImg<short int> *edge);

	/*��Ե����*/ 
	void cutShortEdge(CImg<double> *edge, CImg<double>* resultEdgeImage, int cutLen);

	/*���ܺ���*/
	CImg<short int> canny(string inputFileName);
	
};


#endif