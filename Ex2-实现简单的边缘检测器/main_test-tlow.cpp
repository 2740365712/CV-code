#include "Canny.h"
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char *argv[]) {
	
	/* ���Ե���ֵ��ͼ���Ӱ�죬�仯��Χ�� 5 �� 35*/
	Canny c;
	float tlow = 5;
	float thigh = 40;
	for (int i = 0; i < 7; i++) {
		c.setTLowandTHigh(thigh, tlow);
		c.canny("lena.bmp");
		tlow += 5;
	}
	system("pause");
}