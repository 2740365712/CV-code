#pragma once
/*******************************************************

Author: qiuyihao

Contact: 576261090@qq.com

Date created: 18/12/16

Date finished: 

����˵��������һ�ź���A4ֽ����Ƭ
		1. ͨ��ͼ��ָ���ȡͼ���Ե
		2. ����仯�õ�A4ֽ�ĸ���Ե
		3. ��A4ֽ���н���,��������

*******************************************************/

#include <iostream>
#include <string>
#include "CImg.h"
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <math.h>
#include "utils.h"
using namespace std;
using namespace cimg_library;

class CorrectionForA4Paper
{
	CImg <double> originImage;   /*�洢ԭͼ*/
	
	CImg <double> resultImage;   /*�洢������ͼ��*/

	CImg <double> houghSpaceImage; /*����ռ�ͼ��*/


	bool PRINT;                  /*��ӡ��־*/

	int otsuThreshold;           /*Ostu��ֵ�ָ�ķָ���ֵ*/
	int houghThreshold;          /*����ռ����ֵ*/
	int houghPointDistance;      /*����ռ��е�ľ����ֵС�ڸ�ֵ����Ϊͬһ����*/

	// Polar coordinate intersection at x  
	const int CrossX(int theta, int distance, int x) {
		double angle = (double)theta*cimg::PI / 180.0;
		double m = -cos(angle) / sin(angle);
		double b = (double)distance / sin(angle);
		return m * x + b;
	}

	// Polar coordinate intersection at y  
	const int CrossY(int theta, int distance, int y) {
		double angle = (double)theta*cimg::PI / 180.0;
		double m = -cos(angle) / sin(angle);
		double b = (double)distance / sin(angle);
		return ((double)(y - b) / m);
	}

	const int calDistance(Point a, Point b) {
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}

public:
	CorrectionForA4Paper() {
		otsuThreshold = 1;
		houghThreshold = 300;
		houghPointDistance = 50;
		PRINT = true;
	}

	/*תΪ�Ҷ�ͼ��*/
	CImg<double> ToGrayImage(CImg<double> image);

	/*�޸�ostu����ʼ��ֵ*/
	void setOstThreshold(int threshold);

	/*Ostu����ȡ�����ֵ*/
	int getBestThresholdWithOStu(CImg<double> image);

	/*���������ֵ���зָ�*/
	CImg<double> getSegmentedImage(CImg<double> image, int threshold);

	/*����8����ķ�����ȡ��ֵ��ͼ���Ե*/
	CImg<double> getEdgeImage(CImg<double> image);

	/*���ڻ���任���ڲ����ռ��н���ͶƱ*/
	CImg<double> houghTransform(CImg<double> image);

	/* ��ͶƱ�õĻ���ռ��У�ɸѡ���ĸ������㣬��ΪA4ֽ�ĵ���, ��󷵻������ĸ��ǵ������*/
	vector<Point> getLinesFromHoughSpace(CImg<double> houghSpaceImage, CImg<double> _orignImage, int threshold);

	/*����A4ֽ�ǵ㣬��ȡA4ֽ��Ե, ���ؿ�����ȷ���ɾ��ε��ĸ�˳���*/
	vector<Point> drawEdgeOnA4Paper(CImg<double>& drawImage, vector<Point> pointSet);

	/*����ԭͼ���ĸ��㣬���з���任�õ�A4ֽ�����Ľ��*/
	CImg<double> correctA4Paper(CImg<double> originImage, vector<Point> pointSet);

	/*�������ܵ���ڣ����������ͼ�����������������ͼ��*/
	void Correction(string inputImageName, string outputImageName);

};

