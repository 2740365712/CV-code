#include "CImg.h"
#include <iostream>
#include<math.h>
#include<string>
#include<vector>
#define cimg_display_type 0;
#define PI 3.1415926535

using namespace std;
using namespace cimg_library;

/**************************************************************************
*	Author: qiuyihao
*	Date:2018/09/14
*	Task: Test the drawing algorithm at the first homework of CV
*   method: ��drawBmpTest���У���װ���ĸ���ͼ�������ֱ��Ǹ������ַ�ʽ
*          ������CImg�������Լ��Լ�ʵ�ֵķ����������Ի�Բ�Լ���ֱ�ߵ�Ч��
*           Ȼ��ֱ�Ա������㷨֮������Ӻ�������̽��CImg �ڲ���ʵ�ַ�ʽ
***************************************************************************/


class drawBmpTest {
private:
	CImg<unsigned char> SrcImg;     //���ڱ�����Ҫ����ͼ��Ķ���
	int width;						//����ͼƬ�Ŀ��
	int height;						//����ͼƬ�ĸ߶�
public: 
	drawBmpTest() {
		reset();  // ��ʼ����������
	}

	// ����draw_circle������Բ, 
	// ��Ҫָ�������б�Բ��λ��(xpos,ypos)���Լ��뾶r, ͼ����ɫ
	void drawCircle_withCImgFunc(int xpos, int ypos, int r, const char* color) {
		// �ж�����Ϸ���
		if ((xpos + r > width) || (xpos - r < 0) || (ypos + r) > height || (ypos - r < 0)) {
			cout << "���겻�Ϸ�" << endl;
			return;
		}
		this->SrcImg.draw_circle(xpos, ypos, r, color, 1);
	//	this->SrcImg.display("draw circle with draw_circle function");
	}

	// ����ŷ�Ͼ��뻭Բ
	// �����б�Բ��λ�ã�xpos,ypos) �Լ��뾶r, color
	void drawCircle_noCImgFunc(int xpos, int ypos, int r, const char* color) {
		// �ж�����Ϸ���
		if ((xpos + r > width) || (xpos - r < 0) || (ypos + r) > height || (ypos - r < 0)) {
			cout << "���겻�Ϸ�" << endl;
			return;
		}
		cimg_forXY(SrcImg, x, y) {
			// �ж�������룬С��30�ģ���ɫ�ı�
			if ((abs(x - xpos)*abs(x - xpos) + abs(y - ypos)* abs(y - ypos)) <= r*r) {
				SrcImg(x, y, 0) = color[0];
				SrcImg(x, y, 1) = color[1];
				SrcImg(x, y, 2) = color[2];
			}
		}
		//SrcImg.display("draw circle not CImg Function.");
	}

	//����draw_line����ֱ��
	// �����б�Ϊ�������(startX,startY)���յ�����(endX, endY)��color
	void drawLine_withCImgFunc(int startX, int startY, int endX,int endY, const char* color) {
		//�ж�����Ϸ���
		if ((startX > width) || (startX < 0) || (startY > height) || (startY < 0)) {
			cout << "���겻�Ϸ�" << endl;
			return;
		}
		if ((endX > width) || (endX < 0) || (endY > height) || (endY < 0)) {
			cout << "���겻�Ϸ�" << endl;
			return;
		}
		unsigned char blue[] = { 0, 0, 255 };
		SrcImg.draw_line(startX, startY, endX, endY, color);
		//SrcImg.display("draw line with CImgfunction draw_line");
	}

	// ����DDA�㷨 y = ax + b ����x,y���������������
	// �����б�Ϊ�������(startX,startY)���յ�����(endX, endY) color 
	void drawLine_noCImgFunc(int startX, int startY, int endX, int endY, const char* color) {
		//�ж�����Ϸ���
		if ((startX > width) || (startX < 0) || (startY > height) || (startY < 0) || (endX > width) || (endX < 0) || (endY > height) || (endY < 0)) {
			cout << "���겻�Ϸ�" << endl;
			return;
		}
		float x, y, xincre, yincre; // x y  �����Լ� x��y����
		int k = abs(endX - startX);
		if (abs(endY - startY) > k) {
			k = abs(endY - startY);
		}
		//����x,y����
		xincre = (float)(endX - startX) / k;
		yincre = (float)(endY - startY) / k;
		// ��ʼ��������
		x = startX;
		y = startY;
		// ɨ��
		for (int i = 1; i < k; i++) {
			SrcImg(x, y, 0) = color[0];
			SrcImg(x, y, 1) = color[1];
			SrcImg(x, y, 2) = color[2];
			x += xincre;
			y += yincre;
		}
	}

	// ���ڴ�ӡ���ڱ���
	void display(string str) {
		SrcImg.display(str.c_str());
	}
	// ���¼���ͼƬ��Դ
	void reset() {
		try
		{
			SrcImg.load("1.bmp");
			width = SrcImg._width;
			height = SrcImg._height;
			cimg_forXY(SrcImg, x, y) {
				SrcImg(x, y, 0) = 0;
				SrcImg(x, y, 1) = 0;
				SrcImg(x, y, 2) = 0;
			}

		}
		catch (const std::exception&)
		{
			cout << "��ǰ·��������ͼƬ 1.bmp" << endl;
		}
	}

};


int stringToInt(string num) {
	if (num == "1") return 1;
	if (num == "2") return 2;
	if (num == "3") return 3;
	if (num == "4") return 4;
	if (num == "5") return 5;
	if (num == "6") return 6;
	if (num == "7") return 7;
}

int main() {
	drawBmpTest testCase;
	vector<char*> colorSet;
	char yellow[] = {255, 255, 0};
	char blue[] = { 0, 0, 255};
	char red[] = { 255, 0, 0};
	char green[] = { 0,255, 0 };
	char white[] = { 255,255,255 };
	colorSet.push_back(yellow);
	colorSet.push_back(blue);
	colorSet.push_back(red);
	colorSet.push_back(green);
	colorSet.push_back(white);

	string choose;
	cout << "��������Ӧ������ִ����Ӧ����" << endl;
	cout << "1. CImg��draw_circle��������Ա�,�ı�뾶��С���бȽ�" << endl;
	cout << "2. �Լ�ʵ�ֵ�draw_circle_noCImgFunc��������Աȣ��ı�뾶��С���бȽ�" << endl;
	cout << "3. CImg��draw_line��������Ա�,�ı䳤�Ⱥ�ƫ�ƽǶȽ��бȽ�" << endl;
	cout << "4. �Լ�ʵ�ֵ�draw_line_noCimgFunc��������Ա�,�ı䳤�Ⱥ�ƫ�ƽǶȽ��бȽ�" << endl;
	cout << "5. CImg���Լ�ʵ�ֵ�draw_circle�������жԱ�" << endl;
	cout << "6. CImg���Լ�ʵ�ֵ�draw_line�������жԱ�" << endl;
	cout << "7. ����" << endl;
	int num = 0;
	while (1) {
		cin >> choose;
		switch (stringToInt(choose))
		{
			// ��������
		case 1:
			for (int i = 1; i < 21; i++) {
				testCase.drawCircle_withCImgFunc(200, 200, 105 - i * 5,colorSet.at(i%5) );
			}
			testCase.display("draw circle with cimg function");
			testCase.reset();
			break;
		case 2:
			for (int i = 1; i < 21; i++) {
				testCase.drawCircle_noCImgFunc(200, 200, 105 - i * 5, colorSet.at(i % 5));
			}
			testCase.display("draw circle not with cimg function");
			testCase.reset();
			break;
		case 3:
			for (int i = 1; i < 10; i++) {
				for (int j = 1; j < 10; j++) {
					//cout << ++num << endl;
					testCase.drawLine_withCImgFunc(25, 25, i * 25,j * 25, colorSet.at(i % 5));
				}
			}
			testCase.display("draw line with cimg funciton");
			testCase.reset();
			break;
		case 4:
			for (int i = 1; i < 10; i++) {
				for (int j = 1; j < 10; j++) {
					//cout << ++num << endl;
					testCase.drawLine_noCImgFunc(25, 25, i * 25, j * 25, colorSet.at(i % 5));
				}
			}
			testCase.display("draw line not with cimg funciton");
			testCase.reset();
			break;
		case 5:
			for (int i = 1; i <= 20; i++) {
				testCase.drawCircle_withCImgFunc(100, 100, 110 - i * 5, colorSet.at(i % 5));
			}
			for (int i = 1; i <= 20; i++) {
				testCase.drawCircle_noCImgFunc(300, 300, 110 - i * 5, colorSet.at(i % 5));
			}
			testCase.display("comparision of drawing circles");
			testCase.reset();
			break;
		case 6:
			for (int i = 1; i <= 20; i++) {
				testCase.drawLine_withCImgFunc(200, 200, 50,i * 20, colorSet.at(i % 5));
			}
			for (int i = 1; i <= 20; i++) {
				testCase.drawLine_noCImgFunc(200, 200, 350, i * 20, colorSet.at(i % 5));
			}
			testCase.display("comparision of drawing line");
			testCase.reset();
			break;
		case 7:
			exit(0);
			break;
		default:
			break;
		}
	}
	
}