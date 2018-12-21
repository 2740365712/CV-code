#include "CorrectionForA4Paper.h"

bool compare(Point a, Point b) {
	return a.value < b.value;
}
// �õ�����任�ľ���
vector<vector<double>>* GetTransformMatrixOfTriangle
(const Triangle * SrcTriangle, const Triangle * DstTriangle, vector<vector<double>>* matrix) {
	// transform src to dst

	int u0 = SrcTriangle->a.x;
	int v0 = SrcTriangle->a.y;
	int u1 = SrcTriangle->b.x;
	int v1 = SrcTriangle->b.y;
	int u2 = SrcTriangle->c.x;
	int v2 = SrcTriangle->c.y;
	int x0 = DstTriangle->a.x;
	int y0 = DstTriangle->a.y;
	int x1 = DstTriangle->b.x;
	int y1 = DstTriangle->b.y;
	int x2 = DstTriangle->c.x;
	int y2 = DstTriangle->c.y;
	// |A|
	int detA;
	detA = u0 * v1 + u1 * v2 + u2 * v0 - u2 * v1 - u0 * v2 - u1 * v0;
	// inv(A) * detA
	int A11, A12, A13, A21, A22, A23, A31, A32, A33;
	A11 = v1 - v2;
	A21 = -(v0 - v2);
	A31 = v0 - v1;
	A12 = -(u1 - u2);
	A22 = u0 - u2;
	A32 = -(u0 - u1);
	A13 = u1 * v2 - u2 * v1;
	A23 = -(u0*v2 - u2 * v0);
	A33 = u0 * v1 - u1 * v0;
	vector<double> row1;
	vector<double> row2;
	vector<double> row3;
	row1.push_back((double)(x0*A11 + x1 * A21 + x2 * A31) / detA);
	row1.push_back((double)(x0*A12 + x1 * A22 + x2 * A32) / detA);
	row1.push_back((double)(x0*A13 + x1 * A23 + x2 * A33) / detA);
	row2.push_back((double)(y0*A11 + y1 * A21 + y2 * A31) / detA);
	row2.push_back((double)(y0*A12 + y1 * A22 + y2 * A32) / detA);
	row2.push_back((double)(y0*A13 + y1 * A23 + y2 * A33) / detA);
	row3.push_back((double)(A11 + A21 + A31) / detA);
	row3.push_back((double)(A12 + A22 + A32) / detA);
	row3.push_back((double)(A13 + A23 + A33) / detA);
	matrix->push_back(row1);
	matrix->push_back(row2);
	matrix->push_back(row3);
	return matrix;
}

CImg<double> CorrectionForA4Paper::ToGrayImage(CImg<double> image)
{
	if (PRINT) cout << "ת��Ϊ�Ҷ�ͼ......" << endl;
	CImg<double> grayImage(image._width, image._height, 1, 1, 0);
	cimg_forXY(image, x, y) {
		grayImage(x, y, 0) = image(x, y, 0) * 0.299 + image(x, y, 1) * 0.587 + image(x, y, 2) * 0.114;
	}
	return grayImage;

}

void CorrectionForA4Paper::setOstThreshold(int threshold)
{
	if (threshold > 255 || threshold < 0) {
		cout << "ERROR: the threshold should be in [0 255]." << endl;
	}
	otsuThreshold = threshold;
}

int CorrectionForA4Paper::getBestThresholdWithOStu(CImg<double> image)
{
	if (PRINT) cout << "ʹ��Ostu��ȡ�����ֵ..." << endl;
	int threshold = 1;  // ��ʼ�ָ���ֵ
	int width = image._width;
	int height = image._height;
	int numOfPixels = width * height;
	
	double maxG = 0; // Ostu ������
	double resultThreshold = 0;  //

	for (int i = 50; i < 150; i++) {
		int numOfFrontPoints = 0;    // ǰ������Ŀ
		double rateOfFrontPoints = 0.0; // ǰ������Ŀ����
		int sumOfGrayValueOfFrontPoints = 0.0;// ǰ����Ҷ�ֵ�ܺ�
		double averageOfGrayValueOfFrontPoints = 0.0; // ǰ����Ҷ�ֵ

		int numOfBackPoints = 0; // ��������Ŀ
		double rateOfBackPoints = 0.0; // ��������Ŀ����
		int  sumOfGrayValueOfBackPoints = 0.0; // ������Ҷ�ֵ�ܺ�
		double averageOfGrayValueOfBackPoints = 0.0; // ������Ҷ�ֵ
		
		cimg_forXY(image, x, y) { // ͳ��ǰ���㱳����ĻҶ�ֵ����Ŀ
			if (image(x, y, 0) < threshold) {
				sumOfGrayValueOfBackPoints += image(x, y, 0);
				numOfBackPoints++;   // �����ϰ�������С����ֵ
			}
			else {
				sumOfGrayValueOfFrontPoints += image(x, y, 0);
				numOfFrontPoints++;  
			}
		}

		averageOfGrayValueOfFrontPoints = 1.0 * sumOfGrayValueOfFrontPoints / numOfFrontPoints;
		averageOfGrayValueOfBackPoints = 1.0 * sumOfGrayValueOfBackPoints / numOfBackPoints;

		rateOfFrontPoints = 1.0 * numOfFrontPoints / numOfPixels;
		rateOfBackPoints = 1.0 * numOfBackPoints / numOfPixels;

		double g = rateOfBackPoints * rateOfFrontPoints * (averageOfGrayValueOfBackPoints - averageOfGrayValueOfFrontPoints)
			*  (averageOfGrayValueOfBackPoints - averageOfGrayValueOfFrontPoints);

		if (g > maxG) { // ���·ָ���ֵ���Լ�����g
			resultThreshold = threshold;
			maxG = g;
		}
		threshold++;
	}

	return resultThreshold;
}

CImg<double> CorrectionForA4Paper::getSegmentedImage(CImg<double> image, int threshold)
{
	if (PRINT) cout << "������ֵ�ָ�..." << endl;
	int width = image._width;
	int height = image._height;

	CImg<double> segmentedImage(width, height, 1, 1, 0);

	cimg_forXY(image, x, y) {
		if (image(x, y, 0) < threshold) {
			segmentedImage(x, y, 0) = 0;
		}
		else {
			segmentedImage(x, y, 0) = 255;
		}
	}

	return segmentedImage;
}

CImg<double> CorrectionForA4Paper::getEdgeImage(CImg<double> image)
{
	if (PRINT) cout << "��ȡ��Ե..." << endl;
	int width = image._width;
	int height = image._height;

	CImg<double> resultImage(width, height, 1, 1, 0);

	cimg_forXY(image, x, y) {
		bool white = false;
		if (image(x, y, 0) == 255) {
			white = true;
		}
		
		for (int i = -1; white && (i < 2); i++) {
			bool flag = false;
			for (int j = -1; j < 2; j++) {
				if (x + i < 0 || x + i >= width || y + j < 0 || y + j >= height)
					continue;
				if (image(x + i, y + j, 0) == 0) {
					flag = true;
					break;
				}

			}
			if (flag) {
				resultImage(x, y, 0) = 255;
			}
		}
	}
	return resultImage;

}

// 
CImg<double> CorrectionForA4Paper::houghTransform(CImg<double> image)
{
	if (PRINT) cout << "����任..." << endl;
	int width = image._width;
	int height = image._height;
	int sqrt_xy = sqrt(width * width + height * height);

	
	CImg<double> tmp(360, sqrt_xy , 1, 1, 0);
	houghSpaceImage = tmp;  // ��ʼ������ռ�

	/*
	double sin_value[180] = {};
	double cos_value[180] = {};

	int p = 0;
	int i = 0;
	for (int i = - 90; i < 90; i++) {
		sin_value[i + 90] = sin(i * cimg::PI / 180);
		cos_value[i + 90] = cos(i * cimg::PI / 180);
	}*/

	cimg_forXY(image, x, y) {
		if (image(x, y, 0) != 255)
			continue;
		for (int i = 0; i < 360; i++) { 
			double theta = (double)i * cimg::PI / 180.0;
			int p = (int)(x*cos(theta) + y * sin(theta));
			if (p >= 0 && p < sqrt_xy){
				houghSpaceImage(i, p, 0)++;

			}
				
		}
	}

	houghSpaceImage.display();
	return houghSpaceImage;
}

vector<Point> CorrectionForA4Paper::getLinesFromHoughSpace(CImg<double> houghSpaceImage, CImg<double> _orignImage, int threshold)
{
	CImg<double> drawImage = _orignImage;
	
	vector<Point> pointSet;   // ��Ŵӻ���ռ�����ȡ�ķ�ֵ��

	// �ӻ���ռ�����ȡ��ֵ�㣬�õ�ֱ�ߵļ����귽��
	const int ymin = 0;
	const int ymax = originImage._height - 1;
	const int xmin = 0;
	const int xmax = originImage._width - 1;
	cimg_forXY(houghSpaceImage, x, y) {
		
		if (houghSpaceImage(x, y, 0) > threshold) {
			bool flag = false;
		
			for (int i = 0; i < pointSet.size(); i++) {
				if (pow(pointSet[i].x - x, 2) + pow(pointSet[i].y - y, 2) < houghPointDistance) {
					flag = true;
					if (pointSet[i].value < houghSpaceImage(x, y, 0)) {
						pointSet[i] = *(new Point(x, y, houghSpaceImage(x, y, 0)));
						cout << "Point; " << x << " " << y << " " << houghSpaceImage(x, y, 0) << endl;
					}
				}
			}
			if (flag == false) {
				pointSet.push_back(*(new Point(x, y, houghSpaceImage(x, y, 0))));
				cout << "Point; " << x << " " << y << " " << houghSpaceImage(x, y, 0) << endl;
			}

			// ���������תΪ�ѿ��������ֱ�ߣ���x ��y��Ľ�������
			/*
			const int x0 = CrossY(x, y, ymin);  // ֱ���� y = yminֱ�ߵĽ��������
			const int x1 = CrossY(x, y, ymax); // 
			const int y0 = CrossX(x, y, xmin); // 
			const int y1 = CrossX(x, y, xmax);


			if (x0 >= xmin && x0 <= xmax ||              // ��ʾ��ֱ���Ƿ���ͼ����  
				x1 >= xmin && x1 <= xmax ||
				y0 >= ymin && y0 <= ymax ||
				y1 >= ymin && y1 <= ymax) {
				for (int i = 0; i < pointSet.size(); i++) {
					if (pow(pointSet[i].x - x, 2) + pow(pointSet[i].y-y, 2) < houghPointDistance ) {
						flag = true;
						if (pointSet[i].value < houghSpaceImage(x, y, 0)) {
							pointSet[i] = *(new Point(x, y, houghSpaceImage(x, y, 0)));
							cout << "Point replace; " << x << " " << y << " " << houghSpaceImage(x, y, 0) << endl;
						}
					}
				}
				if (flag == false) {
					pointSet.push_back(*(new Point(x, y, houghSpaceImage(x, y, 0))));
					cout << "Point; " << x << " " << y << " " << houghSpaceImage(x, y, 0) << endl;
				}
			}*/
		}
	}

	/*ȷ���㼯��ǰ4����*/
	/*
	if (pointSet.size() >= 4) {
		int rank1to4[4] = { 0,1,2,3 };  //���ǰ�ĸ�ֵ���ĵ�
		Point points[4] = {};
		sort(pointSet.begin(), pointSet.end(), compare);
		vector<Point> tmpPointSet;
		tmpPointSet.push_back(pointSet[0]);
		tmpPointSet.push_back(pointSet[1]);
		tmpPointSet.push_back(pointSet[2]);
		tmpPointSet.push_back(pointSet[3]);
		pointSet.clear();
		pointSet = tmpPointSet;
	}*/
	
	vector<Line> lineSet;  // �ѿ�������ϵ��ֱ�߷��̼���
	// ������תΪ�ѿ�������ϵ
	for (int i = 0; i < pointSet.size(); i++) {
		double angle = (double)pointSet[i].x * cimg::PI / 180.0;
		double k = -cos(angle) / sin(angle);
		double b = (double)pointSet[i].y / sin(angle);
		lineSet.push_back(*(new Line(k, b)));
		cout << "ֱ�߷���Ϊ y = (" << k << ") x + ( " << b << ")" << endl;
	}

	//����
	const double line_color[] = { 255, 0, 0 };
	for (int i = 0; i < lineSet.size(); i++) {
		const int x0 = double(ymin - lineSet[i].b) / lineSet[i].k;
		const int y0 = xmin * lineSet[i].k + lineSet[i].b;

		const int x1 = double(ymax - lineSet[i].b) / lineSet[i].k;
		const int y1 = xmax * lineSet[i].k + lineSet[i].b;
		
		if (abs(lineSet[i].k) > 1) {
			drawImage.draw_line(x0, ymin, x1, ymax, line_color);

		}
		else {
			drawImage.draw_line(xmin, y0, xmax, y1, line_color);
		}
	}


	// ����ֱ�߽�������
	cout << "��������: " << endl;
	vector<Point> intersectionSet;  // ֱ�߽��㼯��
	for (int i = 0; i < lineSet.size(); i++) {
		for (int j = 0; j < lineSet.size(); j++) {
			double k_i = lineSet[i].k;
			double b_i = lineSet[i].b;
			double k_j = lineSet[j].k;
			double b_j = lineSet[j].b;

			double x = (b_j - b_i) / (k_i - k_j);   // ��������
			double y = (k_i * b_j - k_j * b_i) / (k_i - k_j);

			if (x >= 0 && x < _orignImage._width && y >= 0 && y < _orignImage._height) {
				bool flag = false;
				for (int k = 0; k < intersectionSet.size(); k++) {
					if ((x - intersectionSet[k].x) * (x - intersectionSet[k].x) +
						(y - intersectionSet[k].y) * (y - intersectionSet[k].y) < houghPointDistance * houghPointDistance) {
						flag = true;
						break;
					}
				}
				if (!flag) {
					intersectionSet.push_back(*(new Point(x, y, 0)));
					cout << "( " << x << " ," << y << " )" << endl;
				}
			}
		}
	}
	// ����
	double point_color[] = { 0, 255, 0 };
	for (int i = 0; i < intersectionSet.size(); i++) {
		drawImage.draw_circle(intersectionSet[i].x, intersectionSet[i].y, 15, point_color);
	}

	drawImage.display();

	return drawEdgeOnA4Paper(drawImage, intersectionSet);
}


vector<Point> CorrectionForA4Paper::drawEdgeOnA4Paper(CImg<double> &drawImage, vector<Point> pointSet)
{
	if (pointSet.size() != 4){
		cout << "A4ֽ ������4���ǵ�, �������Ч" << endl;
		Sleep(4000);
		exit(0);
	}
	vector<Point> orderPointSet;
	orderPointSet.push_back(pointSet[0]);
	bool crossFlag[4] = { true, false, false, false };  // �� flag0 �Խ� ���Ϊtrue
	int crossPoint_Index = 0;
	for (int i = 1; i < 4; i++) {
		float k = 1.0 * (orderPointSet[0].y - pointSet[i].y) / (orderPointSet[0].x - pointSet[i].x);
		float b = orderPointSet[0].y - k * orderPointSet[0].x;
		int flag = 0;  // ��־�Ƿ�Ϊ�Խǵ�
		for (int j = 1; j < 4; j++) {
			if (j == i) continue;
			float y = k * pointSet[j].x + b;
			if (pointSet[j].y - y > 0) {
				flag++;
			}
			else {
				flag--;
			}
		}
		if (flag == 0) {
			// �ҵ�00�ĶԽǵ� 
			crossPoint_Index = i;
			crossFlag[crossPoint_Index] = true;
			break;
		}
	}
	int stop = 0;  // ȷ���öԽǵ㣬ȷ��˳��
	for (int i = 1; i < 4; i++) {
		if (crossFlag[i] == false) {
			orderPointSet.push_back(pointSet[i]);
			if(stop == 0) orderPointSet.push_back(pointSet[crossPoint_Index]);
			crossFlag[i] = true;
			if(stop == 0)i = 1;
			stop = 1;
		}
	}
	
	for (int i = 0; i < 4; i++) {
		int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
		if (i == 3) {
			x1 = orderPointSet[3].x;
			y1 = orderPointSet[3].y;
			x2 = orderPointSet[0].x;
			y2 = orderPointSet[0].y;
		}
		else {
			x1 = orderPointSet[i].x;
			y1 = orderPointSet[i].y;
			x2 = orderPointSet[i + 1].x;
			y2 = orderPointSet[i + 1].y;
		}
		double line_color[] = { 0, 0, 255 };
		drawImage.draw_line(x1, y1, x2, y2, line_color);
	}

	char text_color[3] = { 128,128,128 };
	for (int i = 0; i < orderPointSet.size(); i++) {
		cout << " order ( " << orderPointSet[i].x << " , " << orderPointSet[i].y << " ) " << endl;
		char text[30] = { 0 };
		sprintf_s(text, "%d. (%d, %d)", i, orderPointSet[i].x, orderPointSet[i].y);
		drawImage.draw_text(orderPointSet[i].x, orderPointSet[i].y, text, 300, text_color);
	}
	drawImage.display();
	return orderPointSet;
}



CImg<double> CorrectionForA4Paper::correctA4Paper(CImg<double> originImage, vector<Point> pointSet)
{
	/////// ȷ��A4ֽ�����Ͻǵĵ�
	int minIndex = 0; //
	int minDistance = 1000000;
	for (int i = 0; i < pointSet.size(); i++) {  // ������ԭ�������ԭ��ѡȡA4ֽ�����Ͻǵĵ�
		int distance = sqrt(pointSet[i].x * pointSet[i].x + pointSet[i].y * pointSet[i].y);
		if (distance < minDistance) {
			minIndex = i;
			minDistance = distance;
		}
	}
	vector<Point> leftUpPointFirstSet(4);  // �洢�����Ͻǿ�ʼ�ĵ㼯�ϣ�˳ʱ�������ʱ��
	int tmpIndex = minIndex;
	for (int i = 0; i < 4; i++) {
		leftUpPointFirstSet[i] = pointSet[tmpIndex++];
		tmpIndex %= 4;
	}

	CImg<double> originA4Paper = originImage;
	char text_color[3] = { 128,128,128 };
	for (int i = 0; i < leftUpPointFirstSet.size(); i++) {
		cout << " order ( " << leftUpPointFirstSet[i].x << " , " << leftUpPointFirstSet[i].y << " ) " << endl;
		char text[30] = { 0 };
		sprintf_s(text, "%d. (%d, %d)", i, leftUpPointFirstSet[i].x, leftUpPointFirstSet[i].y);
		//originA4Paper.draw_text(leftUpPointFirstSet[i].x, leftUpPointFirstSet[i].y, text, 300, text_color);
	}
	//originA4Paper.display();

	////// ȷ��A4ֽ���� 
	int dis1 = calDistance(leftUpPointFirstSet[0], leftUpPointFirstSet[1]);
	int dis2 = calDistance(leftUpPointFirstSet[1], leftUpPointFirstSet[2]);
	int dis3 = calDistance(leftUpPointFirstSet[2], leftUpPointFirstSet[3]);
	int dis4 = calDistance(leftUpPointFirstSet[3], leftUpPointFirstSet[0]);
	
	int rankDis[4] = { dis1, dis2, dis3, dis4 };

	sort(rankDis, rankDis + 4);

	//int width = cimg::min(dis1, dis2, dis3, dis4);
	//int height = cimg::max(dis1, dis2, dis3, dis4);
	int width = rankDis[1];
	int height = rankDis[2];
	CImg<double> A4Paper(width, height, 1, 3, 0);

	/// ȷ���任��Ķ�Ӧ��ϵ
	bool directionFlag = false;  // Ϊ����ʾ�ǵ�����Ͻ�˳ʱ��ֲ�
	if (dis1 < dis2) {
		directionFlag = true;    // ��һ��С�ڵڶ��ߣ���Ϊ˳ʱ��
	}

	vector<Point> a4EdgePointSet; // ������洢��leftUpPointFirSetλ�����Ӧ��A4ֽ�ǵ�
	a4EdgePointSet.push_back(Point(0, 0, 0));


	if (directionFlag) {
		a4EdgePointSet.push_back(Point(width-1, 0, 0));
		a4EdgePointSet.push_back(Point(width-1, height-1, 0));
		a4EdgePointSet.push_back(Point(0, height-1, 0));
	}
	else {
		a4EdgePointSet.push_back(Point(0, height-1, 0));
		a4EdgePointSet.push_back(Point(width-1, height-1, 0));
		a4EdgePointSet.push_back(Point(width-1, 0, 0));
	}

//	char text_color[3] = { 128,128,128 };
	for (int i = 0; i < a4EdgePointSet.size(); i++) {
		cout << " order ( " << a4EdgePointSet[i].x << " , " << a4EdgePointSet[i].y << " ) " << endl;
		char text[30] = { 0 };
		sprintf_s(text, "%d. (%d, %d)", i, a4EdgePointSet[i].x, a4EdgePointSet[i].y);
	//	A4Paper.draw_text(a4EdgePointSet[i].x, a4EdgePointSet[i].y, text, 300, text_color);
	}
	//A4Paper.display();

	vector<vector<double>> matrix1;
	vector<vector<double>> matrix2;

	// �õ����Ǳ任����
	Triangle originTriangle1 = *(new Triangle(leftUpPointFirstSet[0], leftUpPointFirstSet[1], leftUpPointFirstSet[2],0, 0, 0));
	Triangle originTriangle2 = *(new Triangle(leftUpPointFirstSet[2], leftUpPointFirstSet[3], leftUpPointFirstSet[0], 0, 0, 0));

	Triangle a4Triangel1 = *(new Triangle(a4EdgePointSet[0], a4EdgePointSet[1], a4EdgePointSet[2], 0, 0, 0));
	Triangle a4Triangel2 = *(new Triangle(a4EdgePointSet[2], a4EdgePointSet[3], a4EdgePointSet[0], 0, 0, 0));

	matrix1 = *(GetTransformMatrixOfTriangle(&a4Triangel1, &originTriangle1, &matrix1));
	matrix2 = *(GetTransformMatrixOfTriangle(&a4Triangel2, &originTriangle2, &matrix2));

	//matrix1 = *(GetTransformMatrixOfTriangle(&originTriangle1, &a4Triangel1, &matrix1));
//	matrix2 = *(GetTransformMatrixOfTriangle(&originTriangle2, &a4Triangel2, &matrix2));

	// ���б任
	cimg_forXY(A4Paper, x, y) {
		double origin_x = 0;
		double origin_y = 0;
		bool In = false;
		if (a4Triangel1.isInTriangle(Point(x, y, 0))) {
			In = true;
			origin_x = double(x * matrix1[0][0] + y * matrix1[0][1] + matrix1[0][2]);
			origin_y = double(y * matrix1[1][0] + y * matrix1[1][1] + matrix1[1][2]);
		}
		else if(a4Triangel2.isInTriangle(Point(x, y, 0))){
			In = true;
			origin_x = double(x * matrix2[0][0] + y * matrix2[0][1] + matrix2[0][2]);
			origin_y = double(y * matrix2[1][0] + y * matrix2[1][1] + matrix2[1][2]);
		}
		if (In) {
			cimg_forC(originImage, c) {
				A4Paper(x, y, c) = originImage.linear_atXY(origin_x, origin_y, c);
			}
		}
	}
	A4Paper.display();
	return A4Paper;
}

void CorrectionForA4Paper::Correction(string inputImageName, string outputImageName)
{
	double start = clock();
	originImage.load(inputImageName.c_str());
	resultImage(originImage._width, originImage._height, 1, 1, 0);

	CImg<double> grayImage = ToGrayImage(originImage);
	//grayImage.display();
	int bestThreshold = getBestThresholdWithOStu(grayImage);
	cout << "best Threshold: " << bestThreshold << endl;
	CImg<double> segmentedImage = getSegmentedImage(grayImage,bestThreshold);
	segmentedImage.display();
	CImg<double> edgeImage = getEdgeImage(segmentedImage);
	edgeImage.display();
	houghSpaceImage = houghTransform(edgeImage);
	//resultImage.save(outputImageName.c_str());
	//CImg<int> dotXY = getLineAndAnglePoint(houghSpaceImage, originImage);
	vector<Point> pointSet =  getLinesFromHoughSpace(houghSpaceImage, originImage, houghThreshold);
	CImg<double> result = correctA4Paper(originImage,pointSet);
	result.save(outputImageName.c_str());
	double end = clock();
	cout << "total time: " << (end - start)/CLOCKS_PER_SEC << endl;
}
  