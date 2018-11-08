/**************************************************
* Author: qiuyh
* Date: 2018/10/19
* Notes:  �Զ�ֵͼ����л���任�����������RGBͼ
****************************************************/
#ifndef _HOUGH_FOR_LINE_H_
#define _HOUGH_FOR_LINE_H_
#include <iostream>
#include <string>
#include <stdlib.h>
#include "CImg.h"
#include "Canny.h"
#include <math.h>
#include <vector>
#include <time.h>

#define PI 3.141592
using namespace cimg_library;
using namespace std;

class HoughForLine {
private:
	int number;                        /*ͼƬ���*/
	int Width;                         /*ͼ���Ⱥ͸߶�*/
	int Height;              


	CImg<short int> detectedEdgeImage; /*��Ե����Ķ�ֵͼ��*/
	CImg<short int> deltaXImage;	   /*x�����ƫ��ͼ*/
	CImg<short int> deltaYImage;       /*y�����ƫ��ͼ*/
	CImg<short int> originImage;       /*ԭͼ*/

	struct dot {                       /*���ص����꣬�ݶȷ���ֵ*/
		int x;
		int y;    
		int gradientDirectionValue;  
		bool foundedFlag;               /*Ϊ�棬���ʾ�õ��Ѿ�����ĳһ��ֱ�ߣ�ʧȥͶƱȨ�ˡ�*/
	};

	struct rgbLine{                     /*rgb�ռ�ֱ��б�ʽؾ�,ͶƱ��*/
		float k1;
		float k2;                       /* y = (k1 / k2) * x + b*/
		float b;   
		float x0;                      /*x0: ��k2��0 �ǣ� x = x0, �������������*/
		bool isLine;
		float sita;   // ֱ����б��
		int x_sita;   // ����ռ��е�x������
		float dm;    // ֱ�ߵ�Բ�ĵľ���
		int y_dm;	 // ����ռ��е�y������
		int voteNum;

	};

	vector<dot> edgeDotsSet;              /*�洢��Ե��*/
	vector<rgbLine> rgbLineSet;            /*�洢rgb�ռ�ֱ�ߵ�б�ʺͽؾ�,ͶƱ��*/

	struct Line {
		float sita;   // ֱ����б��
  		int x_sita;   // ����ռ��е�x������
		float dm;    // ֱ�ߵ�Բ�ĵľ���
		int y_dm;	 // ����ռ��е�y������
		int voteNum;  
	};
	vector<Line> houghLineSet; 
	int houghWidthUnitNum;
	int houghHeightUnitNum;
	int houghWidth;
	int houghHeight;

	/*����*/
	int minDistance;                      /*���㹹���ߵ���С����*/
	float minDistanceRate;                /* ���ݱ�Ե����Ŀȷ����С����ı���ϵ��*/

	int similiarLineThreshold;           /*����ֱ����ֵ*/
	int directionThreshold;               /*�ݶȷ�����ֵ*/
	int slopeThreshold;                   /*б����ֵ*/

	int voteNumThreshold;                 /*ͶƱ��ֵ*/
	float voteNumThresholdRate;           /*���ݱ�Ե����Ŀ��ȷ��ͶƱ��ֵ��ϵ��*/

	int MaxChooseLineNum;                 /*����ҵ�������*/
	float MaxChooseLineRate;              /*����ҵ������������ϵ��*/
	int limitedLine;

	Canny detector;                      /*��Ե�����*/
	int thigh;
	int tlow;
	float smoothFactor; 

	bool DEBUG; 
public:

	HoughForLine() {
		DEBUG = true;
		limitedLine = 4;
		directionThreshold = 1;    // ������ɸѡ���ݶȲ���ĵ�  
		slopeThreshold = 0.2;       // б�ʲ��ֵ��б�ʲ��ڸ�ֵ���£���Ϊ���ߣ�������־��������ߵ�������Կ�������
		similiarLineThreshold = 10;  // ֱ�����ƶ�: ���ֽ϶��ص���ֱ�ߵ�ʱ�����Ҫ���
		
		// ����ͼ��ߴ�ȷ�����㹹������̾��� 	minDistance = sqrt(Width * Width + Height * Height) / 20;
		minDistanceRate = 0.1;
		voteNumThresholdRate = 0.005;      // ͶƱ��ֵ����ͼ��߶ȿ����Ŀȷ��
		MaxChooseLineRate = 1;        // ����ͼ��߶ȿ����Ŀȷ���ҵ�������
		number = 1;
		srand((unsigned int)time(NULL));
		
		smoothFactor = 3;

		 houghWidthUnitNum = 180;
		 houghHeightUnitNum = 400 ;
		 houghWidth = PI;
		 houghHeight = 3000;

	}

	void setVoteNumRate(float r) {
		voteNumThresholdRate = r;
	}

	void setMaxChoose(float r) {
		MaxChooseLineRate = r;
	}
	void sethighAndlow(float low, float high) {
		thigh = high;
		tlow = low;
	}
	// �޸ĸ�˹ģ������
	void setBoost(float h) {
		smoothFactor = h;
	}

	/*����ͼƬ*/
	void saveImage(string ImageName, CImg<short int> image);

	/*�洢��Ե��*/
	void saveEdgePoint(CImg<short int> detectedEdgeImage, CImg<short int> deltaXImage,
		CImg<short int> deltaYImage, vector<dot>* edgeDotsSet);

	/*�ж������Ƿ���*/
	bool judgeOnTheSameLine(int x1, int y1, int x2, int y2, int x3, int y3);

	/*���ѡȡ������Ե�㣬����ͶƱֵ�ߵ�ֱ��*/
	void chooseProperPointForLine(vector<dot>* edgeDotsSet);

	/*���ֱ�߷���*/
	void outputLineEquation();

	/*���ݷ��̻���ͼ��*/
	void drawLine(vector<rgbLine> rgbLineSet);

	/*���Ƴ�ֱ�߷����ϵı�Ե��*/
	void drawEdge(vector<dot> edgeDotsSet);

	/*�����ǵ�*/
	void drawCrossPoint();

	/*ִ������hough�任�Ĺ���*/
	void hough(string inputFileName);

};

#endif