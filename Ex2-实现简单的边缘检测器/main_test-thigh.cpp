#include "Canny.h"
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char *argv[]) {
	
	/* ���� ����ֵ��ͼ���Ӱ�죬�仯��Χ�� 40 �� 200*/
	Canny c;
	float tlow = 30;
	float thigh = 40;
	for (int i = 0; i < 8; i++) {
		c.setTLowandTHigh(thigh, tlow);
		c.canny("lena.bmp");
		thigh += 20;
	}
	system("pause");
}