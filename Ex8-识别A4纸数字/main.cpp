#include <iostream>
#include "Canny.h"
#include "utils.h"
#include "CorrectionForA4Paper.h"
#include "DigitSpilt.h"
#include <direct.h>
#include <fstream>
#include <sstream>
#include "TrainDigit.h"  // ѵ���ӿ�
#include <opencv2/highgui/highgui.hpp>

using namespace std;
cv::Ptr<cv::ml::SVM> MySvm;


// cimg
void cimg2Mat(cv::Mat* mat, CImg<double> img) {
	mat = new cv::Mat(img._width, img._height, CV_32FC1);
	for (int i = 0; i < img._width; i++) {
		for (int j = 0; j < img._height; j++) {
			if (img(i, j) > 0) mat->at<float>(i, j) = 1;
			else 
				mat->at<float>(i, j) = img(i, j);
		}
	}
}
                                                      /// 16340186  phone id
void getNum(string inputImage, string directory,vector<string> labelString) {
		cout << inputImage << endl;
		string studentID = inputImage.substr(inputImage.size() - 12, 8);
		cout << studentID << endl;
		ofstream outFile;
		outFile.open("data.csv", ios::app);
		outFile << ",\n";
		outFile << studentID + ".jpg" << ",";
	
		int totalNum = (labelString[0].size() + labelString[1].size() + labelString[2].size());
		int rightNum = 0;

	try {
	
		Canny c;
		CorrectionForA4Paper correct;
		DigitSpilt ds;

		// ��Եͼ
		CImg<short int> edgeImage = c.canny(inputImage);
		edgeImage.display("canny ����Ե");
		// �������A4ֽ
		CImg<double> a4Paper = correct.correct(inputImage, edgeImage);
		a4Paper.display("������A4ֽ");

		// ����������A4ֽ
		string a4name = "./result/A4/" + studentID;
		_mkdir("./result/A4/");
		a4name = a4name + ".bmp";
		a4Paper.save(a4name.c_str());

		// �ĸ��ǵ�����
		vector<Point> orderPoints = correct.getOrderPoints();
		
		for (int i = 0; i < orderPoints.size(); i++) {
			outFile << "(" << orderPoints[i].x << "  " << orderPoints[i].y << "),";
		}

		// a4 ֽ�ҶȻ�
		CImg<double> grayA4Paper = correct.ToGrayImage(a4Paper);

		// ����Ӧ��ֵ����
		CImg<double> adaptiveImg = ds.adaptiveThreshold(grayA4Paper);
		//adaptiveImg.display("����Ӧ��ֵ����");

		// �������㣬�޸������ַ�
		CImg<double> dilatedImg = ds.dilation(adaptiveImg);
		dilatedImg = ds.dilation(dilatedImg);
		//dilatedImg.display("��������");

		// �����̱�Ե
		CImg<double> pureImg = dilatedImg;
		c.cutShortEdge(&dilatedImg, &pureImg, 3);
		//pureImg.display("ȥ���̱�Ե��");

		// ˮƽͶӰ
		CImg<double> verImage = ds.hor_projection(pureImg);
		//verImage.display();

		// ����������򻮷���
		vector<int> horLineSet = ds.hor_partion(verImage);

		// �ֶ��������ݴ���
		// �õ������ң����ϵ��µĵ��������ַ�
		int orderNum = 0; //  �ַ����ļ��е����
		for (int i = 0; i < horLineSet.size(); i++) {
			if (horLineSet[i] == -1) continue;
			int row = horLineSet[i];
			double color[] = { 0,255,255 };
			for (int j = 0; j < a4Paper._width; j++) {
				a4Paper(j, horLineSet[i], 0) = 255;
				a4Paper(j, horLineSet[i], 1) = 0;
				a4Paper(j, horLineSet[i], 2) = 0;
			}
		}

		double red[] = { 255, 0, 0 };
		//a4Paper.display();
 
		int predictFlag = 0;
		for (int i = 0; i < horLineSet.size(); i = i + 2) {
			cout << horLineSet[i] << " " << horLineSet[i + 1] << endl;
			
			// 0 ѧ��  1 �ֻ��� 2���֤
			string labels;
			if (predictFlag < 3) {
				labels = labelString[predictFlag];
				predictFlag++;
			}
			else {
				break; 
			}
			
			if (horLineSet[i] == -1) continue;

			// ������ͼ
			CImg<double> horizonImage = ds.lineImage(horLineSet[i], horLineSet[i + 1], adaptiveImg);

			// ��ֱͶӰ
			CImg<double> horImage = ds.ver_projection(horizonImage);

			// ��ֱ����, ��ͼ
			vector<int> verLineSet = ds.ver_partion(horImage);
			for (int j = 0; j < verLineSet.size(); j = j + 2) {
				a4Paper.draw_line(verLineSet[j], horLineSet[i], verLineSet[j], horLineSet[i + 1], red);
				a4Paper.draw_line(verLineSet[j + 1], horLineSet[i], verLineSet[j + 1], horLineSet[i + 1], red);
			}

			// ���ֵõ���ͼ
			vector<CImg<double>> digitImage = ds.cutImage(horizonImage, verLineSet);

			string predictNum1 = ""; // Ԥ����ַ��� ѧ��
			string predictNum2 = ""; // Ԥ����ַ��� �ֻ���
			string predictNum3 = ""; // Ԥ����ַ��� ���֤

			for (int j = 0; j < digitImage.size(); j++) {

				if (j > labels.size()) break;

				char savePath[50] = { 0 };
				char dir[50] = { 0 };

				string DIR = "";
				if (predictFlag == 1) DIR = "learnID";
				if (predictFlag == 2) DIR = "phoneNum";
				if (predictFlag == 3) DIR = "identity";

				sprintf_s(dir, "./%s/%s/", directory.c_str(), studentID.c_str());
				_mkdir(dir);
				// �����и��ַ����ļ���
				sprintf_s(dir, "./%s/%s/%s/", directory.c_str(), studentID.c_str(), DIR.c_str());
				_mkdir(dir);
				cout << dir << endl;
				// �����и����ַ�
				sprintf_s(savePath, "./%s/%s/%s/%d.bmp", directory.c_str(), studentID.c_str(), DIR.c_str(), orderNum++);

				CImg<double> img = digitImage[j].resize(28, 28);
				img = ds.dilation(img);
				img = ds.dilation(img);
				img = ds.dilation(img);


				// תΪ mat
				img.save(savePath);
				cv::Mat* mat = nullptr;
				cv::Mat* tmp = nullptr;
				mat = new cv::Mat(1,img._width * img._height, CV_32FC1);
				tmp = new cv::Mat(img._height, img._width, CV_32FC1);

				for (int i = 0; i < img._height; i++) {
					for (int j = 0; j < img._width; j++) {
						if (img(j, i) > 0) {
							mat->at<float>(0, i * img._width + j) = float(1.0);
							tmp->at<float>(i, j) = float(1.0);
						}
						else {
							mat->at<float>(0, i * img._width + j) = float(0.0);
							tmp->at<float>(i, j) = float(0.0);
						}
							
					}
				}
				//cv::namedWindow("digit", CV_WINDOW_AUTOSIZE);
				//cv::imshow("digit", *tmp);
				//cout << tmp->size().width << " " << tmp->size().height << endl;
				//cv::waitKey(0);
				//cv::namedWindow("digit", CV_WINDOW_AUTOSIZE);
				//cv::imshow("digit", *mat);
				//cv::waitKey(0);
				//cout << *mat << endl;
				//cimg2Mat(src_img, tmpImg);
				
				int predictValue = predictWithSVM(*mat, MySvm);
				string predictStr = to_string(predictValue);
				if (predictValue + '0' == labels[j]) rightNum++;

				if (predictFlag == 1) predictNum1 += predictStr;  // ����ÿһ�е�Ԥ��ֵ
				if (predictFlag == 2) predictNum2 += predictStr;  // ����ÿһ�е�Ԥ��ֵ
				if (predictFlag == 3) predictNum3 += predictStr;  // ����ÿһ�е�Ԥ��ֵ
				delete mat;
				mat = NULL;
				cout << "predicted value:" << predictValue << " label:" << labels[j] << endl;
			}
			if (predictFlag == 1) outFile << predictNum1 << ",";   // ����ÿһ�е�Ԥ��ֵ
			if (predictFlag == 2) outFile << predictNum2 << ",";// ����ÿһ�е�Ԥ��ֵ
			if (predictFlag == 3) outFile << predictNum3 << ","; // ����ÿһ�е�Ԥ��ֵ
			
			if (predictFlag == 1) cout << "����Ԥ������Ϊ" << predictNum1 << " ʵ��ֵΪ��" << labels << endl;
			if (predictFlag == 2) cout << "����Ԥ������Ϊ" << predictNum2 << " ʵ��ֵΪ��" << labels << endl;
			if (predictFlag == 3)cout << "����Ԥ������Ϊ" << predictNum3 << " ʵ��ֵΪ��" << labels << endl;
			
		}
		float accuracyRate = rightNum * 1.0 / totalNum;
		cout << "����A4ֽʶ����ȷ����Ϊ " << rightNum << endl;
		cout << "�ܹ��и�õ����ַ���Ϊ:" << orderNum << endl;
		cout << "׼ȷ��Ϊ��" << accuracyRate << endl;
		//a4Paper.display();
		a4name = "./result/spilit/" + studentID;
		a4name = a4name + ".bmp";
		_mkdir("./result/spilit/");
		a4Paper.save(a4name.c_str());

		string Str = to_string(accuracyRate);
		outFile << "\"accuracy rate:" + Str << "\",";
		//outFile << endl;
		outFile.close();

	}
	
	catch (exception e) {
		cout << e.what() << endl;
		throw e;
		float accuracyRate = rightNum * 1.0 / totalNum;
		stringstream ss;
		ss >> accuracyRate;
		string Str;
		ss << Str;
		outFile << "\"ERROR"<<  "\",";
		//outFile << endl;
		outFile.close();
	}
	outFile << ",\n";
}


int main() {
	//���ر�ǩ
	//trainAndPredict();
	MySvm = cv::ml::SVM::create();
	loadSVM(&MySvm);
	
	ifstream labelFile("labels.csv", ios::in);
	string lineStr;
	vector<vector<string>> dataLabels;
	while (getline(labelFile, lineStr)) {
		//cout << lineStr;
		vector<string> lineLabel;
		string num;
		int dotFlag = 0;  // , �ű��
		//cout << lineStr << endl;
		for (int i = 0; i < lineStr.size(); i++) {
			if (dotFlag == 0) {
				if (lineStr[i] == ',') {
					lineLabel.push_back(num);
					num = "";
					dotFlag = 1;
				}
				else {
					num += lineStr[i];
				}
			}
			else if (dotFlag == 1) {
				if (lineStr[i] == ',') {
					lineLabel.push_back(num);
					num = "";
					dotFlag = 2;
				}
				else {
					num += lineStr[i];
				}
			}
			else if (dotFlag == 2) {
				if (lineStr[i] == ',') {
					lineLabel.push_back(num);
					num = "";
					dotFlag = 3;
				}
				else {
					num += lineStr[i];
				}
			}
			else if(dotFlag == 3) {
				break;
			}
		}
		dataLabels.push_back(lineLabel);
	}

	
	vector<string> filesPath;
	// ��ȡԤ��ͼƬ��·��
	for (int i = 0; i < dataLabels.size(); i++) {
		if (dataLabels[i][0] == "") break;
		string path = "./testData/";
		path += dataLabels[i][0] + ".bmp";
		filesPath.push_back(path);
	}
	//getNum("./testData/15331178.bmp", "result", dataLabels[5]);
	vector<string> errorFileList;
	for (int i = 0; i < filesPath.size() - 1; i++) {
		try {
			// ͼƬ·��  ʶ�������Ŀ¼  ���ݱ�ǩ
			cout << "��ʼʶ�� " << filesPath[i] << endl;
			getNum(filesPath[i], "result", dataLabels[i]);
		}
		catch (exception e) {
			cout << e.what() << endl;
			cout << filesPath[i] + "ͼ�������,�����һ�š�" << endl;
			errorFileList.push_back(filesPath[i]);
		}
	}
	
	system("pause");
	
}