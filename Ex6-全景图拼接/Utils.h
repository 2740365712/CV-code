#pragma once
/*************************************************************************

Author: qiuyh

Creating Date: 18/11/27

Description: Implement some global functions and variable.

***************************************************************************/
#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <iomanip>
#include "CImg.h"


using namespace std;
using namespace cimg_library;

#define DEBUG 1


// �洢��Ӧ����
struct HomographyMatrix {
	float a11, a12, a13, a21, a22, a23, a31, a32;
	HomographyMatrix(float _a11, float _a12, float _a13, float _a21, 
		float _a22, float _a23, float _a31, float _a32):
		a11(_a11), a12(_a12), a13(_a13), a21(_a21), a22(_a22),
		a23(_a23),a31(_a31), a32(_a32){}

	void print() {
		cout <<setw(18) << a11 << setw(18) << a12 << setw(18) << a13 << endl;
		cout << setw(18) <<  a21 << setw(18) << a22 << setw(18) << a23 << endl;
		cout <<setw(18) <<  a31 << setw(18) << a32 << setw(18) << 1 << endl;
	}
};


// ����ͼƬ�ļ���·������ȡ����Ҫƴ��ͼƬ��·��������һ�����·����������
vector<string>& ReadImageFileName(string imageFileFolderPath, vector<string>& imageFilePathVector);

void getAllFiles(string path, vector<string> &files);

//  �������ֵ
int random(int min, int max);

// ���㵥Ӧ������κ��x����
float get_warped_x(float x, float y, HomographyMatrix H);

// ���㵥Ӧ������κ�y������
float get_warped_y(float x, float y, HomographyMatrix H);

// ת��Ϊ������
CImg<float> cylinderProjection(const CImg<float> &src);