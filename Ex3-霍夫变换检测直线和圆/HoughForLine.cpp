#include "HoughForLine.h"

; void HoughForLine::saveImage(string ImageName, CImg<short int> image) {
	if (DEBUG) cout << "����ͼ��" << endl;
	image.save(ImageName.c_str());
	if (DEBUG) cout << "��������ͼ��" << endl;

}

/************************************************************
* �����Ե�����꣬����x��y����ƫ��ͼ�������Ե�㴦���ݶȷ���
**************************************************************/
void HoughForLine::saveEdgePoint(CImg<short int> detectedEdgeImage, CImg<short int> deltaXImage,
	CImg<short int> deltaYImage, vector<dot>* edgeDotsSet)
{
	if (DEBUG) cout << "�����Ե��" << endl;
	dot Dot;
	int num = 0;
	cimg_forXY(detectedEdgeImage, x, y) {
		if (detectedEdgeImage(x, y) == POSSIBLEEDGE) {
			Dot.x = x;
			Dot.y = y;
			num++;
			Dot.gradientDirectionValue = detector.angle_radians(deltaXImage(x, y), deltaYImage(x, y));
			Dot.foundedFlag = false;
			(*edgeDotsSet).push_back(Dot);
		}
	}
	if(DEBUG) cout << num << "  " << (*edgeDotsSet).size() << "  ....." << endl;
	Sleep(3000);
	if (DEBUG) cout << "���������Ե��" << endl;
}

/*ǰ������(x1,y1) (x2, y2)��ɸѡ��ĵ㣬Ҫ�ж�(x3,y3)�Ƿ�������*/
bool HoughForLine::judgeOnTheSameLine(int x1, int y1, int x2, int y2, int x3, int y3)
{
	if (abs(y1 - y2) > abs(x1 - x2)) {
		if ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1) <= slopeThreshold * (y2 - y1) * (y3 - y1)) {
			cout << "����" << endl;
				return true;
		}
	}
	else {
		if ((y2 - y1) * (x3 - x1) - (x2 - x1) * (y3 - y1) <= slopeThreshold * (x2 - x1) * (x3 - x1)) {
			cout << "����" << endl;
				return true;
		}
	}
	
	cout << "������" << endl;
	return false;
}

/***************************************************************
* 1.���ɸѡ�����㣬����������Ҫ����������
*   1.1 �������dmin
*   1.2 �ݶȷ�����С��directionThreshold
*   1.3 �����㲻���Ѽ�����ֱ����
* 2. ɨ��ͼ���ж��������㹲�ߵı�Ե����������������ͶƱ��ֵ��
*    ��Ϊ���㹹��һ����ʵ��ֱ�ߡ�
* 3. ��ֱ�߼���ֱ�߲����ռ䣬ͬʱҪ�ж��Ƿ������϶���ֱ�߽���
* 4. ֱ���ж�����Ҫ��ֱ�߾����ı�Ե��ȥ��
***************************************************************/
void HoughForLine::chooseProperPointForLine(vector<dot>* edgeDotsSet)
{
	// ɸѡ��
	if (DEBUG) cout << "Ѱ�Һ��ʵ���" << endl;
	int p1 = 0, p2 = 0;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, d1 = 0, d2 = 0;
	bool flagP1 = false, flagP2 = false;
	int size = (*edgeDotsSet).size();

	// ....����������RAND_MAX = 2^31�� �㼯��С������ֵ��Ҫ���䡣����
	bool flag_extend = false;
	int extendNum = 0;
	if ((*edgeDotsSet).size() > RAND_MAX) {
		extendNum = (*edgeDotsSet).size() / RAND_MAX;
	}

	while (1) {
		while (1) {
			p1 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));
			//cout << "p1 " <<  p1 << endl;
			if (p1 < size)
				break;
		}
		while (1) {
			p2 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));  //+1��ֹp1����0
			///cout << "p2 " << p2 << endl;

			if (p2 < size)
				break;
		}


	    x1 = (*edgeDotsSet)[p1].x;
	    y1 = (*edgeDotsSet)[p1].y;
		d1 = (*edgeDotsSet)[p1].gradientDirectionValue;
		x2 = (*edgeDotsSet)[p2].x;
		y2 = (*edgeDotsSet)[p2].y;
		d2 = (*edgeDotsSet)[p2].gradientDirectionValue;
		flagP1 = (*edgeDotsSet)[p1].foundedFlag;
		flagP2 = (*edgeDotsSet)[p2].foundedFlag;

		if (flagP1 == true && flagP2 == true) {
			continue;
		}

		int pd = 0;                        // || abs(d1 - d2) > directionThreshold
		if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) <= minDistance || abs(d1 - d2) > directionThreshold) {
			while (1) {
				while (1) {
					p2 = (rand() % size) + RAND_MAX * (rand() % (extendNum + 1));  //+1��ֹp1����0
					//cout << "p2 " << p2 << endl;
					if (p2 < size)
						break;
				}
				x2 = (*edgeDotsSet)[p2].x;
				y2 = (*edgeDotsSet)[p2].y;
				d2 = (*edgeDotsSet)[p2].gradientDirectionValue;
				flagP2 = (*edgeDotsSet)[p2].foundedFlag;
				if (flagP1 == true && flagP2 == true) {
					continue;
				}             ///  || abs(d1 - d2) <= directionThreshold
				if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) > minDistance || abs(d1 - d2) <= directionThreshold) {
					pd = 1;
					break;
				}
			}
		}
		else
			break;
		if (pd == 1)
			break;
		
	}
	rgbLine rl;
	rl.isLine = false;
	rl.k1 = y1 - y2;
	rl.k2 = x1 - x2;
	if (rl.k2 == 0) {
		rl.x0 = x1;
	}
	else {
		rl.b = y1 - (rl.k1 / rl.k2)*x1;
	}
	rl.voteNum = 0;

	Line l; 
	l.sita = atan((y1 - y2) * 1.0 / (x1 - x2));
	l.dm = x1 * sin(l.sita) - y1 * sin(l.sita);
	l.x_sita = (int)(l.sita / (PI / houghWidthUnitNum));
	l.y_dm = (int)(l.dm / (houghHeight / houghHeightUnitNum));
	l.voteNum = 0;
	bool flag = false;
	for (int i = 0; i < houghLineSet.size(); i++) {
		if (abs(houghLineSet[i].x_sita - l.x_sita) + abs(houghLineSet[i].y_dm - l.y_dm) <= 3) {
			houghLineSet[i].voteNum++;
			flag = true;
			rgbLineSet[i].voteNum++;
		}
	}
	if (flag == false) {
		houghLineSet.push_back(l);
		rgbLineSet.push_back(rl);
	}
}

/*���ֱ�߷���*/
void HoughForLine::outputLineEquation()
{
	if (DEBUG) cout << "���ֱ�߷���" << endl;
	cout << "ͶƱ��ֵΪ��" << voteNumThreshold << " ��Ե����Ϊ" << edgeDotsSet.size() << endl;
	for (int i = 0; i < rgbLineSet.size(); i++) {
		if (rgbLineSet[i].isLine == false)
			continue;
		if (rgbLineSet[i].k2 == 0) {
			cout << i + 1 << " : x = " << rgbLineSet[i].x0 << "  ͶƱ��Ϊ " << rgbLineSet[i].voteNum << endl;
 		}
		else if (rgbLineSet[i].k1 == 0) {
			cout << i + 1 << " : y = " << rgbLineSet[i].b << "  ͶƱ��Ϊ " << rgbLineSet[i].voteNum << endl;
		}
		else {
			cout << i + 1 << " :  y = " << rgbLineSet[i].k1 / rgbLineSet[i].k2 << " x + ("<< rgbLineSet[i].b 
				<< ")  ͶƱ��Ϊ " << rgbLineSet[i].voteNum << endl;
		}
	}
	if (DEBUG) cout << "�������ֱ�߷���" << endl;
}


/*���ݷ��̻���ֱ��*/
void HoughForLine::drawLine(vector<rgbLine> rgbLineSet)
{
	if (DEBUG) cout << "���ݷ��̻���ֱ��" << endl;
	unsigned char blue[] = { 0, 0, 255 };
	float k1 = 0.0, k2 = 0.0, b = 0.0;
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	for (int i = 0; i < rgbLineSet.size(); i++) {
		if (rgbLineSet[i].isLine == false)
			continue;
		k1 = rgbLineSet[i].k1;
		k2 = rgbLineSet[i].k2;
		b = rgbLineSet[i].b;
		if (k2 == 0) {
			cimg_forXY(originImage, x, y) {
				if (x == (int) rgbLineSet[i].x0) {
					originImage(x, y, 0) = 0;
					originImage(x, y, 1) = 0;
					originImage(x, y, 2) = 255;
				}
			}
		}
		else if(k1 == 0){
			cimg_forXY(originImage, x, y) {
				if (y ==(int)rgbLineSet[i].b) {
					originImage(x, y, 0) = 0;
					originImage(x, y, 1) = 0;
					originImage(x, y, 2) = 255;
				}
			}
		}
		else {  // todo �������Ż�
			for (int x = 0; x < Width; x++) {
				int y = ceil((k1 / k2)*x + b);
				if (y <= 0 || y >= Height)
					continue;
				else {
					originImage(x, y, 0) = 0;
					originImage(x, y, 1) = 0;
					originImage(x, y, 2) = 255;
					for (int i = 0; i < 20; i++) {
						if (y + i >= Height)
							break;
						if (y + i <= 0)
							continue;
						originImage(x, y + i, 0) = 0;
						originImage(x, y + i, 1) = 0;
						originImage(x, y + i, 2) = 255;
					}
				}
			}

		}
	}
	if (DEBUG) cout << "������ֱ��" << endl;

}

/*����ֱ�߾����ı�Ե�㣬���Ϊ��ɫ*/
void HoughForLine::drawEdge(vector<dot> edgeDotsSet)
{
	if (DEBUG) cout << "����ֱ�߾����ı�Ե��" << endl;
	int x = 0, y = 0;
	for (int i = 0; i < edgeDotsSet.size(); i++) {
		x = edgeDotsSet[i].x;
		y = edgeDotsSet[i].y;	
		//cout << "draw1" << endl;
		if (originImage(x, y, 0) == 0 && originImage(x, y, 1) == 0 && originImage(x, y, 2) == 255) {
			originImage(x, y, 0) == 255;
			originImage(x, y, 1) == 0;
			originImage(x, y, 2) == 0;
			//cout << "draw2" << endl;
		}
	}
	if (DEBUG) cout << "��������ֱ�߾����ı�Ե��" << endl;
}

/*�����ཻ��ֱ�ߣ�����ֱ��б�ʺͽؾ�������*/
void HoughForLine::drawCrossPoint()
{
	if (DEBUG) cout << "����ֱ�߽ǵ�" << endl;
	rgbLine l1 = rgbLineSet[0];
	rgbLine l2 = rgbLineSet[1];
	rgbLine l3 = rgbLineSet[2];
	rgbLine l4 = rgbLineSet[3];

	// ������l1�ߵ������ǵ�
	unsigned char green[] = { 0,255, 0 };
	while (1) {
		float k1 = l1.k1 / l1.k2;
		float k2 = l2.k1 / l1.k2;
		float k3 = l3.k1 / l1.k2;
		float k4 = l4.k1 / l4.k2;
		float b1 = l1.b;
		float b2 = l2.b;
		float b3 = l3.b;
		float b4 = l4.b;
		
		int pd1 = 1, pd2 = 1, pd3 = 1;

		if (k1 * k2 < 0) {
			pd1 += 1;
			int x = ((b2 - b1) / (k1 - k2)) * k1;
			int y = k1 * x + b1;
			originImage.draw_circle(x, y, 100, green, 1);
		}
		if (k1 * k3 < 0) {
			pd2 += 2;
			int x = ((b3 - b1) / (k1 - k3)) * k1;
			int y = k1 * x + b1;
			originImage.draw_circle(x, y, 100, green, 1);
		}
		if (k1 * k4 < 0) {
			pd3 += 3;
			int x = ((b4 - b1) / (k1 - k4)) * k1;
			int y = k1 * x + b1;
			originImage.draw_circle(x, y, 100, green, 1);
		}
		
		if (pd1 != 2) {
			int x1 = ((b3 - b2) / (k2 - k3)) * k2;
			int y1 = k2 * x1 + b2;
			originImage.draw_circle(x1, y1, 100, green, 1);

			int x2 = ((b4 - b2) / (k2 - k4)) * k2;
			int y2 = k2 * x2 + b2;
			originImage.draw_circle(x2, y2, 100, green, 1);
		}
		if (pd2 != 3) {
			int x1 = ((b2 - b3) / (k3 - k2)) * k3;
			int y1 = k3 * x1 + b3;
			originImage.draw_circle(x1, y1, 100, green, 1);

			int x2 = ((b4 - b3) / (k3 - k4)) * k3;
			int y2 = k3 * x2 + b3;
			originImage.draw_circle(x2, y2, 100, green, 1);
		}
		if (pd3 != 4) {
			int x1 = ((b2 - b4) / (k4 - k2)) * k4;
			int y1 = k4 * x1 + b4;
			originImage.draw_circle(x1, y1, 100, green, 1);

			int x2 = ((b3 - b4) / (k4 - k3)) * k4;
			int y2 = k4 * x2 + b4;
			originImage.draw_circle(x2, y2, 100, green, 1);
		}

		break;
	}

	

}


/*ִ����������*/
void HoughForLine::hough(string inputFileName)
{
	clock_t start, end;
	start = clock();
	if (DEBUG) cout << "��ʼ����任" << endl;
	/*1. �õ���Ե���Ķ�ֵͼ*/
	//Canny detector;
	detector.setTLowandTHigh(thigh, tlow);
	detector.setBoost(smoothFactor);
	detector.canny(inputFileName);
	detectedEdgeImage = *detector.getCutShortEdgeImage();
	//detectedEdgeImage.display();
	string name;
	char char_name [100] = {0};
	sprintf_s(char_name, "%d_I1_%s", number, inputFileName.c_str());
	name.assign(char_name);
	//saveImage(name, detectedEdgeImage);
	name.clear();

	deltaXImage = *detector.getDeltaXImage();
	deltaYImage = *detector.getDeltaYImage();

	Width = detectedEdgeImage._width;
	Height = detectedEdgeImage._height;

	originImage.load_bmp(inputFileName.c_str());

	//�����Ե��
	saveEdgePoint(detectedEdgeImage, deltaXImage, deltaYImage, &edgeDotsSet);

	// ����ͼ���ȸ߶��޸Ĳ������ߵ���С����
	minDistance = sqrt(Width * Width + Height * Height) * minDistanceRate;
	// ���ݸ߶���Ŀȷ������������
    MaxChooseLineNum = MaxChooseLineRate * 0.5 * (Height + Width + edgeDotsSet.size()/1000);
	// ���ݱ�Ե����Ŀȷ��ͶƱ��ֵ
	voteNumThreshold = voteNumThresholdRate * 0.5 * (Height + Width);

	cout << minDistance << " " << MaxChooseLineNum << " " << voteNumThreshold << endl;
	start = clock();
	for (int i = 0; i < MaxChooseLineNum; i++) {
		chooseProperPointForLine(&edgeDotsSet);
	}
	for (int i = 0; i < rgbLineSet.size(); i++) {
		cout << rgbLineSet[i].voteNum << "  " << voteNumThreshold << endl;
		if (rgbLineSet[i].voteNum > voteNumThreshold) {
			rgbLineSet[i].isLine = true;
		}
	}

	end = clock();
	//���ֱ�߷���
	outputLineEquation();
	//���ݷ��̻�ֱ��
	drawLine(rgbLineSet);
	// ��ֱ�߾����ı�Ե��
	drawEdge(edgeDotsSet);
	
	char Name[100] = {};
	sprintf_s(Name, "%d_choose%.4f_vote=%.4f__h=%d_l=%d_%s.bmp", number, MaxChooseLineRate, voteNumThresholdRate, thigh, tlow, inputFileName.c_str());
	string strName = string(Name);
	number++;
	saveImage(strName, originImage);
	//originImage.display();
	cout << "total time:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
}
