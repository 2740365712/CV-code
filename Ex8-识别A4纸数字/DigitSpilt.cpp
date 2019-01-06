#include "DigitSpilt.h"

CImg<double> DigitSpilt::adaptiveThreshold(CImg<double> a4Paper)
{
	int w = a4Paper._width;
	int h = a4Paper._height;
	

	originImage = a4Paper;

	CImg<double> integralImage(a4Paper._width, a4Paper._height, 1, 1, 0);

	// ��ȡ����ͼ��
	cout << "��ȡ����ͼ��" << endl;
	for (int i = 0; i < w; i++) {
		int val = 0;
		for (int j = 0; j < h; j++) {
			val += a4Paper(i, j, 0);
			if (i == 0)
				integralImage(i, j, 0) = val;
			else
				integralImage(i, j, 0) = integralImage(i - 1, j, 0) + val;
		}
	}
	int t = 15;
	int s = h / 8;
	CImg<double> outputImage = a4Paper;
	// 8���������ֵ,Ȼ����ֵ�ָ�
	cout << "����Ӧ�ָ�.." << endl;
	cimg_forXY(a4Paper, x, y) {
		if (x < 2 || x > a4Paper._width - 2 || y < 2 || y > a4Paper._height - 2) {
			outputImage(x, y, 0) = 0;
			continue;
		}
		int x1 = x - s / 2;
		int x2 = x + s / 2;
		int y1 = y - s / 2;
		int y2 = y + s / 2;
		if (x1 <= 0) x1 = 1;
		if (x2 <= 0) x2 = 1;
		if (y1 <= 0) y1 = 1;
		if (y2 <= 0) y2 = 1;
		if (x1 >= w) x1 = w - 1;
		if (x2 >= w) x2 = w - 1;
		if (y1 >= h) y1 = h - 1;
		if (y2 >= h) y2 = h - 1;
		int count = (x2 - x1) * (y2 - y1);
		int sum = integralImage(x2, y2, 0) - integralImage(x2, y1 - 1, 0) - integralImage(x1 - 1, y2, 0) + integralImage(x1 - 1, y1 - 1, 0);
		
		if (a4Paper(x, y, 0) * count <= sum * (100 - 15) / 100) {
			outputImage(x, y, 0) = 255;
		}
		else
			outputImage(x, y, 0) = 0;
	}
	cimg_forXY(outputImage, x, y) {
		if (x <  border || x > w - border || y <  border || y > h - border)
			outputImage(x, y, 0) = 0;
	}
	adaptiveImage = outputImage;
	return outputImage;
}

CImg<double> DigitSpilt::dilation(CImg<double> image)
{
	int w = image._width;
	int h = image._height;
	/*
		1 1 1
		1 0 1
		1 1 1 
	*/
	//image.display();
	CImg<double> img = image;
	cimg_forXY(image, x, y) {
		if (x >= w - 1 || y >= h - 1 || x <= 0 || y <= 0)
			continue;
		if (image(x, y, 0) == 255)
			continue;
		int num = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if (image(x + i, y + j, 0) == 255)
					num++;
			}
		}
		if (num >= 4)
			img(x, y, 0) = 255;
	}
	//img.display();
	return img;
}

CImg<double> DigitSpilt::hor_projection(CImg<double> digitImage)
{
	int w = digitImage._width;
	int h = digitImage._height;
	CImg<double> histogram(w, h, 1, 1, 255);
	for (int i = 0; i < h; i++) {
		int num = 0;
		for (int j = 0; j < w; j++) {
			if (digitImage(j, i, 0) == 255)
				num++;
		}
		for (int j = 0; j < num; j++) {
			histogram(j, i, 0) = 0;
		}
	}
	//histogram.display();
	return histogram;
}

CImg<double> DigitSpilt::lineImage(int y1, int y2, CImg<double> digitImage)
{
	if (y2 > digitImage._height) y2 = digitImage._height;
	if (y1 > digitImage._height) y1 = digitImage._height - 30;
	int h = y2 - y1;
	int w = digitImage._width;
	CImg<double> result(w, h, 1, 1, 0);
	for (int i = y1; i < y2; i++) {
		for (int j = 0; j < w; j++) {
			result(j, i - y1, 0) = digitImage(j, i, 0);
		}
	}
	return result;
}

CImg<double> DigitSpilt::ver_projection(CImg<double> digitImage)
{
	int w = digitImage._width;
	int h = digitImage._height;
	CImg<double> histogram(w, h, 1, 1, 255);
	for (int i = 0; i < w; i++) {
		int num = 0;
		for (int j = 0; j < h; j++) {
			if (digitImage(i, j, 0) == 255)
				num++;
		}
		for (int j = 0; j < num; j++) {
			histogram(i, j, 0) = 0;
		}
	}
	//histogram.display();
	return histogram;
}


vector<int> DigitSpilt::hor_partion(CImg<double> histogram)
{
	int w = histogram._width;
	int h = histogram._height;
	vector<int> lineSet;
	bool whiteFlag = true;
	int mountNum = 0;
	for (int i = 0; i < h; i++) {
		int line = i;
		int num = 0;
		for (int j = 0; j < w; j++) { //  ͳ�ƺ�ɫ����
			if (histogram(j, i, 0) == 0) {
				num++;
			}
			else {
				break;
			}
		}
		if (num != 0) {
			if (whiteFlag) {  // �Ӱ׵��ڵı߽�
				if(line -1 < 0)
					lineSet.push_back(0); // ��¼����
				if (line - 1 >= 0) {
					if (lineSet.size() == 0) {
						lineSet.push_back(line - 1);
					}
					else if(lineSet[lineSet.size() - 1] != -1)  //����ǰ��ת�ڵ��߷ŵ��ϴδ���¼���ߵ�λ��
						lineSet.push_back(line - 1); // ��¼����
					else if (lineSet[lineSet.size() - 1] == -1) {
						lineSet[lineSet.size() - 1] = line - 1;
					}
					//cout << line - 1 << endl;
				//	cout << "-----------------" << endl;
				}
				
				
			}
			whiteFlag = false;
			mountNum += num;
		}
		else if(num == 0 && whiteFlag == false) {  // �Ӻڵ���
			if (line - lineSet[lineSet.size() - 1] >= 1 && mountNum > 10) { // ������� 
				whiteFlag = true;
				lineSet.push_back(line); // ��¼����
				//cout << line << " " << mountNum << endl;
				mountNum = 0;
				//cout << "-----------------" << endl;
			}
			else {  // �����ٷ�
				// ��һ��������Ҫɾ��,�˴���Ϊ-1 
				mountNum = 0;
				lineSet[lineSet.size() - 1] = -1;
				whiteFlag = true;
			}
			
		}
	}
	if ( lineSet.size() != 0 && lineSet[lineSet.size() - 1] == -1) {
		lineSet.pop_back();
	}
	//cout << " ---- " << endl << lineSet.size() << "  === " << h << endl;
	for (int i = 0; i < lineSet.size(); i++) {
		//cout << lineSet[i] << " ";
	}
	for (int i = 0; i < lineSet.size(); i++) {
		if (lineSet[i] == -1) continue;
		if (i % 2 == 0 && lineSet[i] >= 2) {

			if(i == 0) lineSet[0] = lineSet[0] - 2;
			else {
				lineSet[i] = lineSet[i] - 2;
			}
		}
		else {
			if (i == lineSet.size() - 1 && lineSet[i] < h - 2) {
				lineSet[i] = lineSet[i] + 2;
				//cout << lineSet[i] << "  + " << endl;
			}
			else {
				lineSet[i] = lineSet[i] + 2;
			}
		}
	}
	//cout << " ---- " << endl << lineSet.size() << "  === " << h << endl;

	
	//histogram.display();
	return lineSet;
}

vector<int> DigitSpilt::ver_partion(CImg<double> histogram)
{
	int w = histogram._width;
	int h = histogram._height;
	vector<int> lineSet;
	bool whiteFlag = true;
	for (int i = 0; i < w; i++) {
		int line = i;
		int num = 0;
		for (int j = 0; j < h; j++) {
			if (histogram(i, j, 0) == 0)
				num++;
			else {
				break;
			}
		}
		if (num != 0) {
			if (whiteFlag) {
				if (line - 1 < 0)
					lineSet.push_back(0);
				if (line - 1 >= 0)
					lineSet.push_back(line - 1);
			}
			whiteFlag = false;
		}
		else if (num == 0 && whiteFlag == false) {
			whiteFlag = true;
			lineSet.push_back(line);
		}
	}

	for (int i = 0; i < lineSet.size(); i++) {
		if (i % 2 == 0) {
			if (lineSet[i] - 2 >= 0) {
				if(i == 0)
					lineSet[i] = lineSet[i] - 2;
				if (i != 0 && lineSet[i] - 2 > lineSet[i - 1])
					lineSet[i] = lineSet[i] - 2;
			}		
		}
		else {
			if (lineSet[i] + 2 <= w - 2) {
				if (i == lineSet.size() - 1)
					lineSet[i] = lineSet[i] + 5;
				if (i != lineSet.size() - 2 && lineSet[i] + 2 < lineSet[i + 1])
					lineSet[i] = lineSet[i] + 2;
			}
		}
	}
	return lineSet;
}

vector<CImg<double>> DigitSpilt::cutImage(CImg<double> horImage, vector<int> horLineSet)
{
	int w = horImage._width;
	int h = horImage._height;

	vector<CImg<double>> res;
	for (int i = 0; i < horLineSet.size() / 2 ; i++) {
		CImg<double> digitImage(horLineSet[i*2 + 1] - horLineSet[i * 2] + 1, h, 1, 1, 0);
		for (int j = horLineSet[i * 2]; j < horLineSet[i * 2 + 1]; j++) {
			for (int k = 0; k < h; k++) {
				digitImage(j - horLineSet[i * 2], k, 0) = horImage(j, k, 0);
			}
		}
	     // ����ɨ���У�ͳ�Ƴ��ֵ�һ�к�ɫ���ص����λ��
		
		int first = 0;
		int last = 0;
		bool firstFlag = false;
		bool lastFlag = false;
		for (int m = 0; m < digitImage._height; m++) {
			for (int n = 0; n < digitImage._width; n++) {
				if (digitImage(n, m) == 255 && firstFlag == false) {
					firstFlag = true;
					if( m - 1 >= 0) first = m - 1;
					else  first = 0;
				}
				if (digitImage(n, digitImage._height - m - 1) == 255 && lastFlag == false) {
					lastFlag = true;
					if (m + 1 < digitImage._height) last = digitImage._height - m + 1;
					else last = digitImage._height - 1;
				}
			}
			if (lastFlag && firstFlag) break;
		}
		
		//cout << last << " + " << first << endl;
		CImg<double> resImage(digitImage._width, last - first + 1, 1, 1, 0);
		for (int m = first; m < last; m++) {
			for (int n = 0; n < digitImage._width; n++) {
				resImage(n, m - first + 1, 0) = digitImage(n, m, 0);
			}
		}
		//resImage.display();
		int paddingFlag = 0;
		int final_size = 0;
		int padding = 0;
		if (resImage._width > resImage._height) {
			final_size = resImage._width;
			paddingFlag = 0;
			padding = (final_size - resImage._height) / 2;
		}
		else {
			final_size = resImage._height;
			paddingFlag = 1;
			padding = (final_size - resImage._width) / 2;
		}
		
		CImg<double> finalImage(final_size, final_size, 1, 1, 0);
		
		cimg_forXY(resImage, x, y) {
			if (paddingFlag == 0) {
				finalImage(x, y + padding) = resImage(x, y);
			}
			else if (paddingFlag == 1) {
				finalImage(x + padding, y) = resImage(x, y);
			}
		}
		
		res.push_back(finalImage);
	}
	return res;
}


/***********************
https://zh.wikipedia.org/wiki/%E8%BF%9E%E9%80%9A%E5%88%86%E9%87%8F%E6%A0%87%E8%AE%B0

�������Ҵ��ϵ���ɨ�裬�������ر���0���䣬����1ֵʱ����������8����ֻ�����ѱ�ɨ��ĵ����ص㣬����ߡ����ϡ��Ϻ��������ĸ����������ֵ��

������ĸ������ֵ����0����ô��λ�þʹ���һ���µı�ţ���ԭ����ϼ�1����
������ĸ�����ķ�0ֵ������ţ���һ������ô��λ�ñ�ž���������ķ�0��ţ�
������ĸ�����ķ�0ֵ��������ͬ�ı�ţ���ô��λ�ñ�ž�ѡ����֮һ������¼��������ͬ�ı�ţ���Ϊ�������������ͨ�ģ�����Ϊ��ͬ�ı�ţ���

**************************/
vector<int> DigitSpilt::areaLabel(CImg<double> image)
{
	int label = 0;
	int w = image._width;
	int h = image._height;
	CImg<double> labelImage(w, h, 1, 1, label);
	vector<vector<int>> table;  // ��¼ͬ����һ��ı��
	
	cimg_forXY(image, x, y) {
		if (x == 0 || y == 0 || y == h - 1)
			continue;
		if (image(x, y, 0) == 0)
			continue;

		// ��鵱ǰ���ص����ϣ��ϣ��������ĸ�����
		// �ĸ�����Ϊ0���½����
		if (labelImage(x - 1, y) == 0 && labelImage(x - 1, y - 1) == 0 && labelImage(x, y - 1) == 0 && labelImage(x + 1, y - 1) == 0) {
			labelImage(x, y) = label + 1;
			label++;
			continue;
		}

		// �ж��ĸ������0ֵ�ı�������Ŀ
		vector<int> fourPoints;
		fourPoints.push_back(labelImage(x - 1, y - 1, 0));
		fourPoints.push_back(labelImage(x - 1, y, 0));
		fourPoints.push_back(labelImage(x - 1, y + 1, 0));
		fourPoints.push_back(labelImage(x, y - 1, 0));

		int diff1 = 0;
		int diff2 = 0;
		for (int i = 0; i < fourPoints.size(); i++) {
			if (fourPoints[i] == 0) continue;
			if (fourPoints[i] != 0) {
				if (diff1 == 0) {
					diff1 = fourPoints[i]; // ��¼��һ���0ֵ���
				}
				if (diff1 != 0 && diff1 != fourPoints[i] && diff2 == 0)
					diff2 = fourPoints[i];  // ��¼�ڶ����0ֵ���
			}
		}
		if (diff1 != 0 && diff2 == 0) { 
			labelImage(x, y, 0) = diff1;
		}
		if (diff1 != 0 && diff2 != 0) {
			labelImage(x, y, 0) = diff1;
			vector<int> labels;
			labels.push_back(diff1);
			labels.push_back(diff2);
			table.push_back(labels);
		}
	}
	
	for (int i = 0; i < table.size(); i++) {
		cout << i << " :";
		for (int j = 0; j < table[i].size(); j++) {
			cout << " " << table[i][j];
		}
		cout << endl;
	}


	vector<int> digitThreshold;
	//image.display();
	return digitThreshold;
}

