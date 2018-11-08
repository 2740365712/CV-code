#include "HoughForCircle.h"

/*���ļ�������ͼƬ*/
void HoughForCircle::saveImage(string fileName, CImg<short int> img)
{
	if (DEBUG) cout << "����ͼƬ" << endl;
	img.save(fileName.c_str());
}


/*************************************************************
* ��������������Ե���ͼ�еı�Ե��洢������
			ͬʱ�洢�ݶȷ���ֵ �����ٷǱ�Ե��ļ���
*			���ռ任ʱ�䣩
* ������edgeImage ---- canny����Ķ�ֵ����Եͼ
*       edgePoint ---- ���б�Ե��������洢�ṹ
**************************************************************/
void HoughForCircle::saveEdgePoint(CImg<short int> edgeImage, vector<Point>* edgePointSet)
{
	if (DEBUG) cout << "�洢��Ե��" << endl;
	Point p;
	cimg_forXY(edgeImage, x, y) {
		if (edgeImage(x, y) != 0) {
			p.x = x;
			p.y = y;
			p.foundedFlag = false;
			p.gradientDirectionValue = detector.angle_radians(deltaXImage(x, y), deltaYImage(x, y));
			(*edgePointSet).push_back(p);
		}
	}
	//cout << "��Ե����Ŀ" << (*edgePointSet).size() << endl;
}


/********************************************
* �����������ж������γɵ�����������Ƿ�Ϊ0��
*			Ϊ0�����ߣ����⸡������������
* ����˵��������(x1,x2) (y1,y2) (z1,z2)
********************************************/
bool HoughForCircle::judgeOnLine(int x1, int x2, int y1, int y2, int z1, int z2)
{
	if (DEBUG) cout << "�ж����㹲��" << endl;
	if(DEBUG) cout << x1 << " " << x2 << " " << y1 << " " << y2 << " " << z1 << " " << z2 << endl;
	if ((x1 * y2 - x2 * y1) + (y1 * z2 - y2 * z1) + (z1 * x2 - z2 * x1) == 0) {
		if (DEBUG) cout << "����" << endl;
		return true;
	}
	if (DEBUG) cout << "������" << endl;
	return false;
}



/*****************************************************
* �����������������������Ե�㣬���ñ�Ե��������γɵ�
*           Բ�Ĳ���������Բ��λ�ã�a,b),�뾶r
* ����˵����begingVerifiedCircle--��ʱ������������
* ѡ��ı�׼�� ������֮��ľ��벻��С����ȷ���Ĳ���dmin��
*             �����ߣ�
****************************************************/
void HoughForCircle::calculate_a_b_r(Circle* beingVerifiedCircle, vector<Point> edgePointSet, vector<Circle> haveVerifiedCircleSet)
{
	int width = edgeImage._width;
	int height = edgeImage._height;
	
	int size = edgePointSet.size();
	int extendNum = 0;
	if (size > RAND_MAX) {
		extendNum = size / RAND_MAX;
	}

	//todo  �����ݶ�ɸѡ
	Point p1 = {}, p2 = {}, p3 = {};
	int pIndex1 = 0;
	int pIndex2 = 0;
	int pIndex3 = 0;
	bool pFlag1 = false;
	bool pFlag2 = false;
	bool pFlag3 = false;
	int k1 = 0, b1 = 0, k2 = 0, b2 = 0, k3 = 0, b3 = 0, centerLine_k = 0, centerLine_b = 0;
	while (1) {
		if (DEBUG) cout << "����Բ�Ĳ���" << endl;

		Point centerCircleDot;

		// �����ѡ������
		// ��һ����͵ڶ�����ͬ��һ��ѭ���ﶨλ
		while (1) {
			//cout << "1" << endl;
			pIndex1 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));
			if (!(pIndex1 < size && p1.foundedFlag == false))
				continue;
			p1.x = edgePointSet[pIndex1].x;
			p1.y = edgePointSet[pIndex1].y;
			p1.gradientDirectionValue = edgePointSet[pIndex1].gradientDirectionValue;
			p1.foundedFlag = edgePointSet[pIndex1].foundedFlag;
			
			while (1) {
				//cout << "2" << endl;
				pIndex2 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));
				if (!(pIndex2 < size && p2.foundedFlag == false))
					continue;
				p2.x = edgePointSet[pIndex2].x;
				p2.y = edgePointSet[pIndex2].y;
				p2.gradientDirectionValue = edgePointSet[pIndex2].gradientDirectionValue;
				p2.foundedFlag = edgePointSet[pIndex2].foundedFlag;
				if (p2.x == p1.x && p1.y == p2.y)
					continue;
				break;
			}
			
			// �������жϵ�һ����͵ڶ�������ݶȷ����ϵ�ֱ���Ƿ�����Բ���д��߽���һ�㣬���У�����ѡ������
			k1 = p1.gradientDirectionValue;
			b1 = p1.y - k1 * p1.x;

			k2 = p2.gradientDirectionValue;
			b2 = p2.y - k2 * p2.x;
			
			centerLine_k = (p2.x - p1.x) * 1.0 / (p2.y - p1.y);
			centerLine_b = (p1.y + p2.y) * 1.0 / 2 + (p1.x * p1.x - p2.x + p2.x) * 1.0 / (2 * (p2.y - p1.y));

			if (k1 == centerLine_k || k2 == centerLine_k) {
				// ƽ�С�������ѡ
				continue;
			}
			int x_1_k = (centerLine_b - b1) / (k1 - centerLine_k);
			int y_1_k = k1 * x_1_k + b1;

			int x_2_k = (centerLine_b - b2) / (k2 - centerLine_k);
			int y_2_k = k1 * x_2_k + b2;

			// �ж�������������Ƿ�������ֵ������������ɹ��ҵ�1��2��
			if ((x_1_k - x_2_k) * (x_1_k - x_2_k) + (y_2_k - y_1_k)*(y_2_k - y_1_k) <= circleCenterDistanceThreshold * circleCenterDistanceThreshold) {
				centerCircleDot.x = (x_1_k + x_2_k) / 2;
				centerCircleDot.y = (y_1_k + y_2_k) / 2;
				break;
			}
			
		}
		
		// ��3����
		while (1) {
			//cout << "3" << endl;

			pIndex3 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));
			if (!(pIndex3 < size && p3.foundedFlag == false))
				continue;

			
			// �жϵ������ݶȷ�������ֱ���Ƿ���1��2���γɵ�Բ���ཻ
			p3.gradientDirectionValue = edgePointSet[pIndex3].gradientDirectionValue;
			p3.foundedFlag = edgePointSet[pIndex3].foundedFlag;
			p3.x = edgePointSet[pIndex3].x;
			p3.y = edgePointSet[pIndex3].y; 
			
			// ���㹲�ߣ���ѡ
			if (judgeOnLine(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y)) {
				continue;
			}


			k3 = p3.gradientDirectionValue;
			b3 = p3.y - k3 * p3.x;

			k2 = p2.gradientDirectionValue;
			b2 = p2.y - k2 * p2.x;

			centerLine_k = (p2.x - p3.x) * 3.0 / (p2.y - p3.y);
			centerLine_b = (p3.y + p2.y) * 3.0 / 2 + (p3.x * p3.x - p2.x + p2.x) * 3.0 / (2 * (p2.y - p3.y));

			if (k3 == centerLine_k || k2 == centerLine_k) {
				// ƽ�С�������ѡ
				continue;
			}
			int x_3_k = (centerLine_b - b3) / (k3 - centerLine_k);
			int y_3_k = k3 * x_3_k + b3;

			int x_2_k = (centerLine_b - b2) / (k2 - centerLine_k);
			int y_2_k = k3 * x_2_k + b2;

			// �ж�������������Ƿ�������ֵ������������ɹ��ҵ�3��
			if ((x_3_k - x_2_k) * (x_3_k - x_2_k) + (y_2_k - y_3_k)*(y_2_k - y_3_k) <= circleCenterDistanceThreshold * circleCenterDistanceThreshold) {
				centerCircleDot.x = (x_3_k + x_2_k) / 2;
				centerCircleDot.y = (y_3_k + y_2_k) / 2;
			//	cout << "3��  ok" << endl;
				break;
			}
		}

		// ��ѡ�������ߵ�����,�������
		int t1 = p2.x * p2.x + p2.y * p2.y - (p1.x * p1.x + p1.y * p1.y);
		int t2 = 2 * (p2.y - p1.y);
		int t3 = p3.x * p3.x + p3.y * p3.y - (p1.x * p1.x + p1.y * p1.y);
		int t4 = 2 * (p3.y - p1.y);
		int t5 = (p2.x - p1.x)*(p3.y - p1.y) - (p3.x - p1.x)*(p2.y - p1.y);
		int t6 = 2 * (p2.x - p1.x);
		int t7 = 2 * (p3.y - p1.y);

		int a = (int)((t1 * t4 - t2 * t3) * 1.0 / (4 * t5)); 
		int b = (int)((t6 * t3 - t7 * t1) * 1.0 / (4 * t5));
	    int r = (int)(0.5 + sqrt((p1.x - beingVerifiedCircle->a)*(p1.x - beingVerifiedCircle->a) +
			(p1.y - beingVerifiedCircle->b)*(p1.y - beingVerifiedCircle->b)));
		

		if (a < 0 || a > width || b  < 0 || b > height) {
			if (DEBUG) {
				cout << "�޳�Բ����ͼ��������  " << endl;
				cout << a << " " << b << " " << r << endl;
			}
			continue;

		}
		cout << a << " " << b << " " << r << endl;

		if (r < 10)
			continue;

		beingVerifiedCircle->a = a;
		beingVerifiedCircle->b = b;
		beingVerifiedCircle->r = r;
	//	cout << a << "---- " << b << " ----" << r << endl;
		if (!chooseCircle(*beingVerifiedCircle, haveVerifiedCircleSet)) {
			/*�õ������Ƶ�Բ���ߵõ����Ƶ�Բ������������upperLimitOfSimiliarCircle*/
			if (DEBUG) cout << "�ҵ����ʵ����Բ������ͶƱ����" << endl;
			break;
		}
	}
	
}

/***************************************************************
* �����������жϼ���õ�����ʱԲ�Ƿ����Ѽ���Բ�ཻ��
*			�ཻ�����棬��ֹ��һ��ͶƱ��Ȼ��ʼ��������һ��Բ
*����˵����
***************************************************************/
bool HoughForCircle::chooseCircle(Circle begingVerifiedCircle, vector<Circle> haveVerifiedCircleSet)
{
	if (DEBUG) cout << "�ж�������ɵ�Բ�Ƿ�����ȷ���õ�Բ����,���ཻ" << endl;
	return  false;
	int a = begingVerifiedCircle.a;
	int b = begingVerifiedCircle.b;
	int r = begingVerifiedCircle.r;
	for (int i = 0; i < haveVerifiedCircleSet.size(); i++) {
		int A = haveVerifiedCircleSet[i].a;
		int B = haveVerifiedCircleSet[i].b;
		int R = haveVerifiedCircleSet[i].r;

		if ((a-A) * (a - A) + (b - B)*(b - B) < (r + R) * (r + R)) {
			//cout << "��Բ�ཻ" << endl;
			return true;
		}
			
		
	}
	return false;
}



void HoughForCircle::vote(int voteNumThreshold, vector<Point> edgePointSet, Circle* beingVerifiedCircle, vector<Circle> haveVerifiedCircle)
{
	
	if (DEBUG) cout << "��ʼͶƱ" << endl;
	int a = (*beingVerifiedCircle).a;
	int b = (*beingVerifiedCircle).b;
	int r = (*beingVerifiedCircle).r;
	int t = 2;
	int voteNum = 0;
	vector<int> voteFlag;
	for (int i = 0; i < edgePointSet.size(); i++) {
		int x = edgePointSet[i].x;
		int y = edgePointSet[i].y;
		if (edgePointSet[i].foundedFlag == true)
			continue;

		if (x > a + r + t || x < a - r - t || y > b + r + t || y < b - r - t) {
			// ����Բ������������⣬����
			continue;
		}
		else {
			if ((r - edgeWidth) * (r - edgeWidth) < (x - a)*(x - a) + (y - b)*(y - b) &&
				(r + edgeWidth) * (r + edgeWidth) > (x - a)*(x - a) + (y - b)*(y - b)) {
				voteFlag.push_back(i);
				voteNum++;  //��Ʊ
			}
		}
	}
	voteNumThreshold = (int)( 2.0 * PI * r *voteNumThresholdRate);

	//cout << " a: " << a << "  b: " << b << " r: " << r << endl
		//<<" voteNumThreshold:" << voteNumThreshold << " voteNum: " << voteNum << endl;
	if (voteNum >= voteNumThreshold) {  //��Բ��������
		if (DEBUG) cout << "��Բ��������-------------------------------------------" << endl;
		haveVerifiedCircleSet.push_back(*beingVerifiedCircle);
		for (int i = 0; i < voteFlag.size(); i++) {
			edgePointSet[i].foundedFlag = true;
		}
		return;
	}

	if (DEBUG) cout << "��Բ����������" << endl;
}

/*����Բ���������������*/
void HoughForCircle::inputCircleEquation(vector<Circle> haveVerifiedCircleSet)
{
	if (DEBUG) cout << "���Բ����" << endl;
	for (int i = 0; i < haveVerifiedCircleSet.size(); i++) {
		cout << "(x - " << haveVerifiedCircleSet[i].a << ")^2 + (y - " << haveVerifiedCircleSet[i].b << " )^2 = "
			<< haveVerifiedCircleSet[i].r * haveVerifiedCircleSet[i].r << endl;
	}
}

void HoughForCircle::drawCircleByEquation(vector<Circle> haveVerifiedCircleSet, CImg<short int>* houghImage)
{
	if (DEBUG) cout << "���ݷ��̻���ͼ��" << endl;
	unsigned char blue[] = { 0,0,255 };
	for (int i = 0; i < haveVerifiedCircleSet.size(); i++) {
		int a = haveVerifiedCircleSet[i].a;
		int b = haveVerifiedCircleSet[i].b;
		int r = haveVerifiedCircleSet[i].r;
		cimg_forXY(*houghImage, x, y) {
			if ((x - a)*(x - a) + (y - b)*(y - b) < (r + edgeWidth)*(r + edgeWidth)
				&& (x - a)*(x - a) + (y - b)*(y - b) > (r - edgeWidth)*(r - edgeWidth)) {
				(*houghImage)(x, y, 0) = 0;
				(*houghImage)(x, y, 1) = 0;
				(*houghImage)(x, y, 2) = 255;
			}
		}
	}
	

}

void HoughForCircle::drawCircleEdge(vector<Point> edgePointSet, vector<Circle> haveVerifiedCircleSet, CImg<short int>* houghImage)
{
	if (DEBUG) cout << "����Բ�ı�Ե��" << endl;
	for (int i = 0; i < edgePointSet.size(); i++) {
		int x = edgePointSet[i].x;
		int y = edgePointSet[i].y;
		if ((*houghImage)(x, y, 2) == 255) {
			(*houghImage)(x, y, 0) = 255;
			(*houghImage)(x, y, 2) = 0;
		}
	}
}


void HoughForCircle::hough(string fileName)
{
	clock_t startTime = clock();
	if (DEBUG) cout << "��ʼ����任" << endl;

	/*����ͼ��*/
	detector.canny(fileName);
	//edgeImage = *detector.getNonMaxSuppImage();
	edgeImage = *detector.getCutShortEdgeImage();
	string cannyFileName = "canny_" + fileName;
	saveImage(cannyFileName.c_str(), edgeImage);
	houghImage.load_bmp(fileName.c_str());
	deltaXImage = *detector.getDeltaXImage();
	deltaYImage = *detector.getDeltaYImage();

	/*�����Ե��*/
	saveEdgePoint(edgeImage, &edgePointSet);
	stopFlag = 0;
	
	height = edgeImage._height;
	width = edgeImage._width;
	maxTimes = maxTimesRate * (height / 3 + width / 3 + edgePointSet.size()/100);

	//cout << "��Ե����Ŀ " << edgePointSet.size() << " h " << height << " w" << width << endl;

	for(int i = 0; i < maxTimes; i++){ // ��ֹ�����ǣ����������������е�Բ�Ĳ���
		/*����õ����ʵ�Բ�Ĳ��� a��b��r*/
		calculate_a_b_r(&beingVerifiedCircle, edgePointSet, haveVerifiedCircleSet);
		/*ִ��ͶƱ*/
		vote(this->voteNumThreshold, edgePointSet, &beingVerifiedCircle, haveVerifiedCircleSet);
	}
	
	/*���Բ����*/
	inputCircleEquation(this->haveVerifiedCircleSet);

	/*����Բ*/
	drawCircleByEquation(this->haveVerifiedCircleSet, &houghImage);

	/*����Բ�ı�Ե��*/
	drawCircleEdge(edgePointSet, haveVerifiedCircleSet, &houghImage);

	string houghFileName = "hough_" + fileName;
	saveImage(houghFileName.c_str(), houghImage);
	houghImage.display();
	cout << "�ݼ�⣬ͼ��Ӳ�Ҹ���Ϊ " << haveVerifiedCircleSet.size() << endl;
	clock_t endTime = clock();
	cout << "�ܹ�����ʱ��Ϊ " << (double)(endTime - startTime)/CLOCKS_PER_SEC << " s"<< endl;
}
