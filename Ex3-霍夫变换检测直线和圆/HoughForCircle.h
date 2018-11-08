#pragma once
#ifndef _HOUGH_FOR_CIRCLE_H_
#define _HOUGH_FOR_CIRCLE_H_

#include "Canny.h"
#include "CImg.h"
#include <vector>
#include <math.h>
#include <time.h>
class HoughForCircle {
private:
	/*�����ݽṹ*/
	struct Point {                         /* ��Ե��ṹ��,���꣨x,y) */
		int x;
		int y;
		float gradientDirectionValue;       /*��Ե����ݶ�ֵ*/
		bool foundedFlag;                   /*ͶƱ�ı�־��Ϊ����õ�ͶƱ��Ч*/
	};
	vector<Point> edgePointSet;            /* ��Ե�㼯�� */
	/*Բ���ݽṹ*/
	struct Circle {                        /* Բ�ṹ�壬 Բ�ģ�a,b), �뾶 r*/
		int a;
		int b;
		int r;
	};
	vector<Circle> haveVerifiedCircleSet;    /*�洢�������������Բ*/
	 
	/*ͼ������ض���*/
	Canny detector;                         /*��Ե�����*/
	CImg<short int> edgeImage;              /* ����canny�õ��ı�Եͼ */
	CImg<short int> originImage;            /*ԭͼ*/
	CImg<short int> houghImage;             /* ����任���ͼ����*/
	CImg<short int> deltaXImage;            /*x,y����ƫ��ͼ*/
	CImg<short int> deltaYImage;         
	Circle beingVerifiedCircle;             /* ��ǰ������Բ */

	/*��Ҫ����*/
	int voteNumThreshold;                          /* ���Բ�ļ����ֵ*/
	float voteNumThresholdRate;                      /* ���Բ�ļ��ϵ��������ͼ��ߴ���ȷ��*/

	int maxTimes;                                /*ѭ�����������������Բ��������ֹ��־*/
	float maxTimesRate;                            /*�������������ϵ��������ͼ��ߴ���ȷ����������*/
	int stopFlag;                                /*��ֹ��־*/

	int upperOfLosing;                          /*�������Բ�ĸ���������*/

	int circleCenterDistanceThreshold;           /*ѡȡ��ʱ�����ݶ�����ֱ��������һ���д��߽���ľ���С�������ֵ������Ϊ�������������*/

	float edgeWidth;                             /*����ı�Ե���ֵ*/
	int circleCenterDistance;                    /*������Բ�ĵľ���ֵ*/
	

	/**/
	int numOfCircle;                           /*ͼƬ��Բ�ĸ���*/
	bool DEBUG;                                /* debug��Ҫ��Ϊ���ӡִ�����*/

	int width;                                 /*ͼ����*/
	int height;                                /*ͼ��߶�*/


public:

	HoughForCircle() {
		voteNumThreshold = 0;
		voteNumThresholdRate = 0.60;  //voteNumThreshold = ceil( 2 * PI * r * rate);
		maxTimesRate = 0.1;
		edgeWidth = 2;
		upperOfLosing = 1000;
		stopFlag = 0;
		circleCenterDistance = 10;   
		DEBUG = false;
		circleCenterDistanceThreshold = 20;
		srand((unsigned int)time(0));
	}

	/*�޸Ĳ���*/
	void setRate(float r) {
		maxTimesRate = r;
	}

	void setDebug(bool tf) {
		DEBUG = tf;
	}

	void setEdgeWidth(int e) {
		edgeWidth = e;
	}

	/*����ͼ����������ͼƬ*/
	void saveImage(string fileName, CImg<short int> img);

	/*��ȡ��Ե��ֵͼ�����ҽ���Ե������洢����,ͬʱ�����ݶȣ�Ҳ�����ݶ���Ϣ*/
	void saveEdgePoint(CImg<short int> edgeImage, vector<Point>* edgePointSet);

	/*�ж������Ƿ��ߣ����߷����棬������Ҫ����ѡ��*/
	bool judgeOnLine(int x1, int y1, int x2, int y2, int x3, int y3);

	/*��ѡ������Ե�㣬�������Բ��λ��(a,b)���뾶r*/
	void calculate_a_b_r(Circle* beingVerifiedCircle, vector<Point> edgePointSet, vector<Circle> haveVerifiedCircleSet);

	/*�ж�����õ���Բ�Ƿ��뵱ǰ���ɹ�Բ�������������ô������ѡ*/
	bool chooseCircle(Circle begingVerifiedCircle, vector<Circle> haveVerifiedCircle);

	/*���ò���3��Բ��λ�ð뾶���ж����б�Ե�㵽Բ�ľ��룬����ͶƱ
	Ʊ������һ��ֵʱ��Ϊ����ʵ��Բ����������3�����Բ�ͼ�������3.��һ����Բ������ֹ����*/
	void vote(int voteNumThreshold, vector<Point> edgePointSet, Circle * beingVerifiedCircle, vector<Circle> haveVerifiedCircleSet);

	/*�����������*/
	void inputCircleEquation(vector<Circle> haveVerifiedCircleSet);

	/* ���ݲ������̻���Բ��*/
	void drawCircleByEquation(vector<Circle> haveVerifiedCircleSet, CImg<short int> * houghImage);

	/*����ͼ��Բ�ı�Ե*/
	void drawCircleEdge(vector<Point> edgePointSet, vector<Circle> haveVerifiedCircleSet, CImg<short int>* houghImage);

	/*���ܲ��蹦�ܺ���*/
	void hough(string fileName);

};


#endif // !_HOUGH_FOR_CIRCLE_H_
