#include "Canny.h"
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char *argv[]) {
	
	/* ���� ģ�����Ӷ�ͼ���Ӱ�죬�仯��Χ�� 0.2 �� 2*/
	Canny c;
	float blurFactor = 0.2;
	for (int i = 0; i < 9; i++) {
		c.setBoost(blurFactor);
		c.canny("lena.bmp");
		blurFactor += 0.2;
	}

	system("pause");
}