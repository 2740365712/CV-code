#pragma once
/*******************************************************

Author: qiuyihao

Contact: 576261090@qq.com

*******************************************************/

#include <iostream>
#include <string>
#include "CImg.h"
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>
#include "utils.h"

using namespace std;
using namespace cimg_library;

#define MyPI 3.141592
#define A4width 420    //A4ֽ��
#define A4height 594   //A4ֽ��
#define MyScalar 2       //A4ֽ����ͼ�����A4ֽ��/�ߵı���

class CorrectionForA4Paper
{
	CImg <double> originImage;   /*�洢ԭͼ*/
	
	CImg <double> resultImage;   /*�洢������ͼ��*/

	CImg <double> houghSpaceImage; /*����ռ�ͼ��*/

	vector<Point> orderPointSet;

	bool PRINT;                  /*��ӡ��־*/

	int otsuThreshold;           /*Ostu��ֵ�ָ�ķָ���ֵ*/
	int houghThreshold;          /*����ռ����ֵ*/
	int houghPointDistance;      /*����ռ��е�ľ����ֵС�ڸ�ֵ����Ϊͬһ����*/
	int _distance; 
	int _voteNum; 
	// Polar coordinate intersection at x  
	const int CrossX(int theta, int distance, int x) {
		double angle = (double)theta * MyPI / 180.0;
		double m = -cos(angle) / sin(angle);
		double b = (double)distance / sin(angle);
		return m * x + b;
	}

	// Polar coordinate intersection at y  
	const int CrossY(int theta, int distance, int y) {
		double angle = (double)theta * MyPI / 180.0;
		double m = -cos(angle) / sin(angle);
		double b = (double)distance / sin(angle);
		return ((double)(y - b) / m);
	}

	const int calDistance( Point a,Point b) {
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}

public:
	CorrectionForA4Paper() {
		otsuThreshold = 1;
		houghThreshold = 400; //250 
		houghPointDistance = 400;
		_distance = 300;
		_voteNum = 0;
		PRINT = true;
	}

	/*תΪ�Ҷ�ͼ��*/
	CImg<double> ToGrayImage(CImg<double> image);

	vector<int> getInterpolationRGB(double scrX, double scrY);

	/*���ڻ���任���ڲ����ռ��н���ͶƱ*/
	CImg<double> houghTransform(CImg<double> image);

	/* ��ͶƱ�õĻ���ռ��У�ɸѡ���ĸ������㣬��ΪA4ֽ�ĵ���, ��󷵻������ĸ��ǵ������*/
	vector< Point> getLinesFromHoughSpace(CImg<double> houghSpaceImage, CImg<double> _orignImage, int threshold);

	/*����A4ֽ�ǵ㣬��ȡA4ֽ��Ե, ���ؿ�����ȷ���ɾ��ε��ĸ�˳���*/
	vector< Point> drawEdgeOnA4Paper(CImg<double>& drawImage, vector< Point>PointSet);

	/*����ԭͼ���ĸ��㣬���з���任�õ�A4ֽ�����Ľ��*/
	CImg<double> correctA4Paper(CImg<double> originImage, vector< Point>PointSet);

	/*�������ܵ���ڣ����������ͼ�����������������ͼ��*/
	CImg<double> correct(string inputImageName, CImg<short int> edgeImage);

	vector< Point> getOrderPoints() {
		return orderPointSet;
	}
};

