#include "CImg.h"
#include <iostream>
#include<math.h>
#include<string>

/**********************************
*	Author: qiuyihao
*	Date:2018/09/11
*	Task: The first homework of CV 
**********************************/

#define cimg_display_type 0;
#define PI 3.1415926535

using namespace std;
using namespace cimg_library;

/****************************************************************************
* ����ҵ�е�ÿһ��������������˷�װ��
* �����ڻ�ͼ��������ʹ��CImg�⺯���Լ����ر�������ʵ��
* ÿһ������ķ�������ǰ׺step + nָ�����n
* ��3��4��5�������ַ���ʵ�֣���׺������withCImgFunc��noCImgFunc
* Ȼ��ʹ�� exe_hw_withCImgFunction��exe_hw_noCImgFunction()�Զ�ִ��2~6�����衣
*****************************************************************************/

class drawBmp {
private:
	CImg<unsigned char> SrcImg;     //���ڱ�����Ҫ����ͼ��Ķ���
	string textOnBmp;               //ͼƬ�ϵ������������������޵���CImg����
	string savedFileName;           //ָ������ͼƬ���ļ���
	string initialFileName;         //��ʼ��ʱͼƬ���ļ���
public:
	//�Զ��幹�캯���������ļ�����ָ��������ͼƬ��ͬʱdisplay
	drawBmp(string name) {
		try
		{
			initialFileName = name;
			savedFileName = "2";
			SrcImg.load_bmp(name.c_str());
			//step1 ��ȡ����ʾͼ��
			SrcImg.display("original image");
		}
		catch (const std::exception&)
		{
			cout << "��ǰ·��û�и�ͼƬ" << endl;
		}
	}
	
	// �����û�ͼ�������Զ�ִ�еڶ��������ģ��岽��
	void exe_hw_withCImgFunction() {
		reset();
		this->textOnBmp = "Call drawing function";
		this->savedFileName = "2_withCImgFunction.bmp";
		step2_changedColor();
		step3_drawBlueCircle_withCImgFunc();
		step4_drawYellowCircle_withCImgFunc();
		step5_drawBlueLine_withCImgFunc();
		step6_saveBmp();
	}

	//�������û�ͼ������ �Զ�ִ�еڶ��������ģ��壬����
	void exe_hw_noCImgFunction() {
		reset();
		this->textOnBmp = "No call drawing function";
		this->savedFileName = "2_noCImgFunction.bmp";
		step2_changedColor();
		step3_drawBlueCircle_noCImgFunc();
		step4_drawYellowCircle_noCImgFunc();
		step5_drawBlueLine_noCImgFunc();
		step6_saveBmp();
	}

	// ���¼���ԭʼͼƬ
	void reset() {
		SrcImg.load_bmp(initialFileName.c_str());
	}

	// ��ɫ�����Ϊ��ɫ ��ɫ�����Ϊ��ɫ
	void step2_changedColor() {
		//step1 �õ�ͼ��ĳ��ȿ��
		int width = SrcImg._width;
		int height = SrcImg._height;
		cout << "The width of the image is " << width << endl;
		cout << "The height of the image is " << height << endl;

		//step2 ɨ��ͼ���ж���ɫ
		//��ɫ�����Ϊ��ɫ����ɫ�����Ϊ��ɫ
		// x��y ����ͼ�����꣬����ԭ��λ�����Ͻǣ�����Ϊx�ᣬ����Ϊy��
		cimg_forXY(SrcImg, x, y) {
			// ��ɫ�����Ϊ��ɫ
			if (SrcImg(x, y, 0) == 255 && SrcImg(x, y, 1) == 255 && SrcImg(x, y, 2) == 255) {
				SrcImg(x, y, 0) = 255;
				SrcImg(x, y, 1) = SrcImg(x, y, 2) = 0;
			}
			// ��ɫ�����Ϊ��ɫ
			else if (SrcImg(x, y, 0) == 0 && SrcImg(x, y, 1) == 0 && SrcImg(x, y, 2) == 0) {
				SrcImg(x, y, 0) = 0;
				SrcImg(x, y, 1) = 255;
				SrcImg(x, y, 2) = 0;
			}
		}
		// ��ʾ��ɫ���
		SrcImg.display("Changed color from image");
	}

	// ����ɫ��Բ�������б�Ϊ Բ������x=50��y=50���뾶=30����ɫ��͸����
	void step3_drawBlueCircle_withCImgFunc() {
		unsigned char blue[] = { 0, 0, 255 };
		this->SrcImg.draw_circle(50, 50, 30, blue, 1);
		this->SrcImg.display("draw blue circle");
	}

	// ����ɫ��Բ�������б�Ϊ Բ������x=50��y=50���뾶=30����ɫ�ƣ�͸����
	void step3_drawBlueCircle_noCImgFunc() {
		unsigned char blue[] = { 0, 0, 255 };
		cimg_forXY(SrcImg, x, y) {
			// �ж�������룬С��30�ģ���ɫ��Ϊ��ɫ
			if ((abs(x - 50)*abs(x - 50) + abs(y - 50)* abs(y - 50)) <= 900) {
				SrcImg(x, y, 0) = SrcImg(x, y, 1) = 0;
				SrcImg(x, y, 2) = 255;
			}
		}
		SrcImg.display("draw blue circle not CImg Function.");
	}

	// ����ɫ��Բ,�����б�Ϊ Բ������x=50��y=50���뾶=3����ɫ�ƣ�͸����
	void step4_drawYellowCircle_withCImgFunc() {
		unsigned char yellow[] = { 255, 255, 0 };
		SrcImg.draw_circle(50, 50, 3, yellow, 1);
		SrcImg.display("draw yellow circle");
	}

	// ����ɫ��Բ,�����б�Ϊ Բ������x=50��y=50���뾶=3����ɫ�ƣ�͸����
	void step4_drawYellowCircle_noCImgFunc() {
		unsigned char yellow[] = { 255, 255, 0 };
		cimg_forXY(SrcImg, x, y) {
			// �ж�������룬С��30�ģ���ɫ��Ϊ��ɫ
			if ((abs(x - 50)*abs(x - 50) + abs(y - 50)* abs(y - 50)) <= 9) {
				SrcImg(x, y, 0) = SrcImg(x, y, 1) = 255;
				SrcImg(x, y, 2) = 0;
			}
		}
		SrcImg.display("draw yellow circle not CImg Function");
	}

	//��ͼ�ϻ���һ����Ϊ100 ��ֱ�߶Σ��������Ϊ(0, 0)�������Ϊ35 �ȣ�ֱ�ߵ���ɫΪ��ɫ��
	void step5_drawBlueLine_withCImgFunc() {
		//�������Ǻ��������յ�����
		int x0 = 100 * cos(35.0 * PI / 180);
		int y0 = 100 * sin(35.0 * PI / 180);
		//cout << x0 << "==========================" << y0 << endl;
		// Ϊ��ֱ�߲���������resetSrcImg����
		reset();
		unsigned char blue[] = { 0, 0, 255 };
		SrcImg.draw_line(0, 0, x0, y0, blue);
		SrcImg.display("draw blue line");
	}

	//��ͼ�ϻ���һ����Ϊ100 ��ֱ�߶Σ��������Ϊ(0, 0)�������Ϊ35 �ȣ�ֱ�ߵ���ɫΪ��ɫ��
	void step5_drawBlueLine_noCImgFunc() {
		// Ϊ��ֱ�߲���������resetSrcImg����
		reset();
		//�������Ǻ��������յ�����
		unsigned char blue[] = { 0, 0, 255 };
		double k = tan(35.0 * PI / 180); //����б��
		int y = 0; //������
		// ����DDA�㷨 �ο����� http://www.cnblogs.com/flying-roc/articles/1913385.html
		// ��б��С��1ʱ����������Ϊx�ᣬ��������Ϊy��
		cimg_forX(SrcImg, x) {
			if (x < (100 * cos(35.0*PI / 180))) {
				y = (int)(k * 1.0 * x);
				SrcImg(x, y, 0) = SrcImg(x,y,1) = 0;
				SrcImg(x, y, 2) = 255;
			}
			
		}
		SrcImg.display("draw blue line not with  CImg fucntion");
	}

	// ����ͼƬ������CImg������
	void step6_saveBmp() {
		// ��������
		unsigned char white[] = { 255,255,255 };
		// ������(100, 100)����һ����ɫ����
		SrcImg.draw_text(SrcImg._width / 2, 10, textOnBmp.c_str(), 2, white);
		SrcImg.save(savedFileName.c_str());
		cout << "The picture is saved as bmp. " << endl;
	}
};

int StringToInt(string num) {
	if (num == "1") return 1;
	if (num == "2") return 2;
	if (num == "3") return 3;
}




int main() {
	drawBmp bmp("1.bmp");
	cout << "ͼƬ�Ѽ��غ���ʾ" << endl;
	string num;
	cout << "��������Ӧ������ִ����Ӧ����" << endl;
	cout << "1. �Զ�ִ������2~6������3~5����Cimg����" << endl;
	cout << "2. �Զ�ִ������2~6������3~5ʹ�ñ������ظı���ɫʵ��" << endl;
	cout << "3. ����" << endl;
	while (1) {
	cin >> num;
	int Integer = StringToInt(num);
	switch (Integer)
	{
		case 1:
			bmp.exe_hw_withCImgFunction();
		break;
		case 2:
			bmp.exe_hw_noCImgFunction();
		break;
		case 3:
			exit(0);
		default:
		break;
	}
	}
	return 0;
}