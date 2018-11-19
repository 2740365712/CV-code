
/************************************************

Author :  qyh

Date: 2018/11/11

Description: Draw triangle mesh and do local warping
			 to transform picture A to picture B.

**************************************************/

#ifndef MORPHING_WITH_LOCAL_WARPING_H
#define MORPHING_WITH_LOCAL_WARPING_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "CImg.h"
#include <fstream>
#include <stdlib.h>

using namespace cimg_library;
using namespace std;

class MorphingWithLocalWarping
{
private:
	/*to save the featured points*/
	struct Point{
		int x;
		int y;
		Point(int _x, int _y) :x(_x), y(_y) {};
	};

	struct Edge {
		Point a;
		Point b;
		int index[2];
		Edge(Point _a, Point _b, int indexA, int indexB) : a(_a), b(_b) {
			index[0] = indexA;
			index[1] = indexB;
		};
		bool operator==(const Edge e) {
			if (e.a.x == this->a.x && e.a.y == this->a.y && e.b.x == this->b.x && e.b.y == this->b.y) {
				return true;
			}
			if (e.a.x == this->b.x && e.a.y == this->b.y && e.b.x == this->a.x && e.b.y == this->a.y) {
				return true;
			}
			return false;
		}
	};

	struct Triangle {
		Point a;  /*��������*/
		Point b;
		Point c;
		int index[3]; /*���������������㼯�е�����*/
		Triangle(Point _a, Point _b, Point _c, int a_index, int b_index, int c_index):
		a(_a), b(_b), c(_c){
			index[0] = a_index;
			index[1] = b_index;
			index[2] = c_index;  // ��¼�������������㼯�е�����
		}
		/*�������γɵ��������*/
		int cross3(const Point &a, const Point &b, const Point &p) {
			return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
		}
		/*���������*/
		double GetDistance(Point A, Point B) {
			double dis = (A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y);
			return sqrt(dis);
		}
		/*�жϵ��Ƿ������εĶ���*/
		bool isVertex(Point p) {
			if (p.x == a.x && p.y == a.y)
				return true;
			if (p.x == b.x && p.y == b.y)
				return true;
			if (p.x == c.x && p.y == c.y)
				return true;
			return false;
		}

		bool operator==(const Triangle t) {  //���������==
			// ...��ȷ����Ķ�Ӧ��ϵ��������3��= 6�����ܡ�����
			if (this->a.x == t.a.x && this->a.y == t.a.y && this->b.x == t.b.x && this->b.y == t.b.y
				&& this->c.x == t.c.x && this->c.y == t.c.y) {
				return true;  // a b c
			}
			if (this->a.x == t.a.x && this->a.y == t.a.y && this->b.x == t.c.x && this->b.y == t.c.y
				&& this->c.x == t.b.x && this->c.y == t.b.y) {
				return true;  // a c b
			}
			if (this->a.x == t.b.x && this->a.y == t.b.y && this->b.x == t.a.x && this->b.y == t.a.y
				&& this->c.x == t.c.x && this->c.y == t.c.y) {
				return true;   // b a c 
			}
			if (this->a.x == t.b.x && this->a.y == t.b.y && this->b.x == t.c.x && this->b.y == t.c.y
				&& this->c.x == t.a.x && this->c.y == t.a.y) {
				return true;  // b c a 
			}
			if (this->a.x == t.c.x && this->a.y == t.c.y && this->b.x == t.a.x && this->b.y == t.a.y
				&& this->c.x == t.b.x && this->c.y == t.b.y) {
				return true;   // c a b
			}
			if (this->a.x == t.c.x && this->a.y == t.c.y && this->b.x == t.b.x && this->b.y == t.b.y
				&& this->c.x == t.a.x && this->c.y == t.a.y) {
				return true;   // c b a
			}
			return false;
		}
		bool isInTriangle(const Point& p); /*�жϵ��Ƿ�����������*/
		bool isInOuterCircle(const Point &p); /*�жϵ��Ƿ������������Բ��*/
	};

	CImg<double> SrcImage;       // Դͼ��
	CImg<double> DstImage;       // Ŀ��ͼ��   
	CImg<double> MorphingImage;  // �ں�ͼ��
	CImg<double> SrcMeshImage;   // Դ����ͼ��
	CImg<double> DstMeshImage;   // Ŀ������ͼ��

	//������Ĵ洢ʱ�Ķ�Ӧ��ϵ����һ��
	vector<Point*> SrcPointSet;    // Դͼ�������㼯
	vector<Point*> DstPointSet;    // Ŀ��ͼ�������㼯
	
	//�����εĴ洢λ�ö�Ӧ��ϵ��˳�򱣳�һ��
	vector<Triangle*> SrcTriangleSet;  // Դͼ�������μ�
	vector<Triangle*> DstTriangleSet;  // Ŀ��ͼ�������μ�

	int frame;
	string SrcPointFilePath;
	string DstPointFilePath;

	bool Debug = 0;

public:
	/*get the pictures and the featured dots set, or set the frame*/
	MorphingWithLocalWarping(string SrcImg, string SrcPointSetFilePath, string DstImg,
		string DstPointSetFilePath, int _frame = 11) {
		SrcImage.load_bmp(SrcImg.c_str());
		DstImage.load_bmp(DstImg.c_str());
		SrcMeshImage = SrcImage;
		DstMeshImage = DstImage;
		MorphingImage = SrcImage;
		this->frame = _frame;
		this->SrcPointFilePath = SrcPointSetFilePath;
		this->DstPointFilePath = DstPointSetFilePath;
		Debug = 1;
	}
	
	/*˫���Բ�ֵ����*/
	double BilinearFilter(CImg<double> Image, double x, double y, int channel);

	/*read the file , get the points data and save*/
	CImg<double> GetFeaturePoints(CImg<double> Image, vector<Point*>* PointSet, 
		string PointsSetFilePath);

	/*calculate the triangle set with pointset*/
	void CalculateTriangles(vector<Point*>* SrcPoints, vector<Triangle*>* SrcTriangles,
		vector<Point*>* DstPoints, vector<Triangle*>* DstTriangles);

	void DrawPoints(CImg<double>* Image, vector<Point*> PointSet);

	/*Draw Triangles with Delaunay method*/
	void DrawTriangles(CImg<double>* Image, vector<Triangle*> TriangleSet);

	/*get the 3 * 3transforming matrix from the srctriangle to the dsttriangle*/
	vector<vector<double>>* GetTransformMatrixOfTriangle(const Triangle* SrcTriangle, 
		const Triangle* DstTriangle, vector<vector<double>>* matrix);

	/*get the transition between src and dst image*/
	Triangle* GetTransitionOnTriangle(Triangle* SrcTriangle, 
		Triangle* DstTriangle, float ratio, Triangle* MorphTriangle);
	
	/*morphing the middle image with the triangle sets*/
	CImg<double>* MorphingTriangleSets(const vector<Triangle*> SrcTriangleSet, 
		const vector<Triangle*> DstTriangleSet, CImg<double>* SrcImage, 
		CImg<double>* DstImage, CImg<double>* MorphingImage, double ratio);

	/*�Զ���ִ����������*/
	void StartMorphing();
};
#endif
