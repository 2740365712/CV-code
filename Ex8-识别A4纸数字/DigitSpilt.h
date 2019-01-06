#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class DigitSpilt
{
	CImg<double> originImage;
	CImg<double> adaptiveImage;
	int border = 20;
public:

	// ����Ӧ��ֵ�ָ�
	CImg<double> adaptiveThreshold(CImg<double> a4Paper);

	//�Զ����ַ������޸�������
	CImg<double> dilation(CImg<double> image);

	// �Էָ�ͼ����д�ֱֱ��ͼͶӰ
	CImg<double> hor_projection(CImg<double> digitImage);

	// ��ȡ����ͼ��
	CImg<double> lineImage(int y1, int y2, CImg<double> digitImage);

	// �Դ�ֱ�ָ���ͼ�������ֱֱ��ͼͶӰ
	CImg<double> ver_projection(CImg<double> digitImage);

	// ��ˮƽֱ��ͼ���л��֣��õ��������ֵ����½�
	vector<int> hor_partion(CImg<double> histogram);

	// ������ֱֱ��ͼ�Ļ����ߣ����ֵ����ַ�
	vector<int> ver_partion(CImg<double> histogram);

	//�Ե���ͼ�������ͨ������� �ָ��ַ��������ַ�������x������x���б�
	vector<int> areaLabel(CImg<double> image);

	// �������ߣ��з�ÿһ��ͼ��
	vector<CImg<double>> cutImage(CImg<double> horImage, vector<int> horLineSet);
};

