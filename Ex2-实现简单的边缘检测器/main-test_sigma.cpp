#include "Canny.h"
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char *argv[]) {


	/*1. ���Ը�˹�뾶��Ӱ��*/
	/*������ ��˹ģ��ͼ���ݶȷ�ֵͼ�����������ͼ����ֵ����ͼ���޳��̱�Եͼ*/
	Canny c;
	float sigma = 0.5;   // ��˹�뾶��0.5 �� 5
	
	for (int i = 0; i < 10; i++) {
		c.setSigma(sigma);
		c.canny("lena.bmp");
		sigma += 0.5;
	}
	system("pause");
}