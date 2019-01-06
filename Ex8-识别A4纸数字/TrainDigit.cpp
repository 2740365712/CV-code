#include "TrainDigit.h"

double cost_time;
clock_t start_time;
clock_t end_time;


int reverseInt(int i) {
	unsigned char c1, c2, c3, c4;

	c1 = i & 255;
	c2 = (i >> 8) & 255;
	c3 = (i >> 16) & 255;
	c4 = (i >> 24) & 255;

	return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

cv::Mat read_Mnist_Image(string fileName) {
	int magic_number = 0;
	int number_of_images = 0;
	int n_rows = 0;
	int n_cols = 0;

	cv::Mat DataMat;

	ifstream file(fileName, ios::binary);
	if (file.is_open())
	{
		cout << "�ɹ���ͼ�� ... \n";

		file.read((char*)&magic_number, sizeof(magic_number));
		file.read((char*)&number_of_images, sizeof(number_of_images));
		file.read((char*)&n_rows, sizeof(n_rows));
		file.read((char*)&n_cols, sizeof(n_cols));
		//cout << magic_number << " " << number_of_images << " " << n_rows << " " << n_cols << endl;

		magic_number = reverseInt(magic_number);
		number_of_images = reverseInt(number_of_images);
		n_rows = reverseInt(n_rows);
		n_cols = reverseInt(n_cols);
		cout << "MAGIC NUMBER = " << magic_number
			<< " ;NUMBER OF IMAGES = " << number_of_images
			<< " ; NUMBER OF ROWS = " << n_rows
			<< " ; NUMBER OF COLS = " << n_cols << endl;

		//-test-
		//number_of_images = testNum;
		//�����һ�ź����һ��ͼ������ȡ��������
		cv::Mat s = cv::Mat::zeros(n_rows, n_rows * n_cols, CV_32FC1);
		cv::Mat e = cv::Mat::zeros(n_rows, n_rows * n_cols, CV_32FC1);

		cout << "��ʼ��ȡImage����......\n";
		start_time = clock();
		DataMat = cv::Mat::zeros(number_of_images, n_rows * n_cols, CV_32FC1);
		for (int i = 0; i < number_of_images; i++) {
			for (int j = 0; j < n_rows * n_cols; j++) {
				unsigned char temp = 0;
				file.read((char*)&temp, sizeof(temp));

				if (temp > 0) temp = 1.0;
				float pixel_value = float(temp + 0.0);
				//float pixel_value = float((temp + 0.0) / 255.0);
				DataMat.at<float>(i, j) = pixel_value;

				//��ӡ��һ�ź����һ��ͼ������
				if (i == 0) {
					s.at<float>(j / n_cols, j % n_cols) = pixel_value;
				}
				else if (i == number_of_images - 1) {
					e.at<float>(j / n_cols, j % n_cols) = pixel_value;
				}
			}
		}
		end_time = clock();
		cost_time = (end_time - start_time) / CLOCKS_PER_SEC;
		cout << "��ȡImage�������......" << cost_time << "s\n";

	}
	file.close();
	return DataMat;
}

cv::Mat read_Mnist_Label(string fileName) {
	int magic_number;
	int number_of_items;

	cv::Mat LabelMat;

	ifstream file(fileName, ios::binary);
	if (file.is_open())
	{
		cout << "�ɹ���Label�� ... \n";

		file.read((char*)&magic_number, sizeof(magic_number));
		file.read((char*)&number_of_items, sizeof(number_of_items));
		magic_number = reverseInt(magic_number);
		number_of_items = reverseInt(number_of_items);

		cout << "MAGIC NUMBER = " << magic_number << "  ; NUMBER OF ITEMS = " << number_of_items << endl;

		//-test-
		//number_of_items = testNum;
		//��¼��һ��label�����һ��label
		unsigned int s = 0, e = 0;

		cout << "��ʼ��ȡLabel����......\n";
		start_time = clock();
		LabelMat = cv::Mat::zeros(number_of_items, 1, CV_32SC1);
		for (int i = 0; i < number_of_items; i++) {
			unsigned char temp = 0;
			file.read((char*)&temp, sizeof(temp));
			LabelMat.at<unsigned int>(i, 0) = (unsigned int)temp;

			//��ӡ��һ�������һ��label
			if (i == 0) s = (unsigned int)temp;
			else if (i == number_of_items - 1) e = (unsigned int)temp;
		}
		end_time = clock();
		cost_time = (end_time - start_time) / CLOCKS_PER_SEC;
		cout << "��ȡLabel�������......" << cost_time << "s\n";

		cout << "first label = " << s << endl;
		cout << "last label = " << e << endl;
	}
	file.close();
	return LabelMat;
}


/*����ͼƬ��СΪ 28��28*/
int predictWithSVM(cv::Mat digitImage, cv::Ptr<cv::ml::SVM> mySvm) {
	// ���������������ò���
	cout << "Predicting ..." << endl;
	if (mySvm.empty()) {
		mySvm = cv::ml::SVM::create();
		mySvm = cv::ml::SVM::load("svm.xml");
	}
	
	float res = mySvm->predict(digitImage);
	return (int)res;
}


bool loadSVM(cv::Ptr<cv::ml::SVM>* mySvm) {
	try {
		//svm = cv::ml::SVM::create();
		*mySvm = cv::ml::SVM::load("svm.xml");
		return true;
	}
	catch (exception e) {
		cout << e.what() << endl;
		return false;
	}
}

////////// ��mnist ��ѵ���Ͳ���

int trainAndPredict()
{
	cout << "ѵ������������ 1, ֱ��ʹ��ѵ��ģ��Ԥ������2" << endl;
	string flag = "";

	while (1) {
		cin >> flag;
		if (flag == "1" || flag == "2")
			break;
		else {
			cout << "����1��2" << endl;
		}
	}

	// ���������������ò���
	cv::Ptr<cv::ml::SVM> SVM_params = cv::ml::SVM::create();

	if (flag == "1") {
		// ѵ�� ����ģ��
		// ��ȡѵ������������
		cv::Mat trainingDataMat;
		trainingDataMat = read_Mnist_Image("mnist_dataset/train-images.idx3-ubyte");

		//ѵ����������Ӧֵ  
		cv::Mat responsesMat;
		responsesMat =read_Mnist_Label("mnist_dataset/train-labels.idx1-ubyte");

		////===============================����SVMģ��===============================////
	
		SVM_params->setType(cv::ml::SVM::C_SVC);     //C_SVC���ڷ��࣬C_SVR���ڻع�
		SVM_params->setKernel(cv::ml::SVM::RBF);  //LINEAR���Ժ˺�����SIGMOIDΪ��˹�˺���

		// ע�͵����ֶԱ���Ŀ��Ӱ�죬Ӱ������ֻ������
		//SVM_params->setDegree(0);            //�˺����еĲ���degree,��Զ���ʽ�˺���;
		SVM_params->setGamma(0.01);       //�˺����еĲ���gamma,��Զ���ʽ/RBF/SIGMOID�˺���; 
		//SVM_params->setCoef0(0);             //�˺����еĲ���,��Զ���ʽ/SIGMOID�˺�����
		SVM_params->setC(10.0);              //SVM�����������������C-SVC��EPS_SVR��NU_SVR�Ĳ�����
		//SVM_params->setNu(0);                //SVM�����������������NU_SVC�� ONE_CLASS ��NU_SVR�Ĳ����� 
		//SVM_params->setP(0);                 //SVM�����������������EPS_SVR ����ʧ����p��ֵ. 
		//������������ѵ��1000�λ������С��0.01����
		SVM_params->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 1000, 0.01));
		//Mat* responsesTransfer = new Mat(responsesMat->size().height, 1, CV_32FC1);

		//responsesMat->convertTo(*responsesMat, CV_32SC1);      ����ΪCV_32SC1���˴�ʡ������Ϊ��ȡ��ʱ����ָ���ø�ʽ�ˡ�
		//trainingDataMat->convertTo(*trainingDataMat, CV_32F);  �˴���Ҫע��ѵ����������Ϊ CV_32F

		//ѵ�����ݺͱ�ǩ�Ľ��
		cout << "��ʼѵ��" << endl;
		cout << "ѵ�����ݳ���" << trainingDataMat.size().width << " �߶� " << trainingDataMat.size().height << endl;
		cout << "��ǩ���ݳ���" << responsesMat.size().width << " �߶� " << responsesMat.size().height << endl;

		//cv::Ptr<cv::ml::TrainData> tData = TrainData::create(*trainingDataMat, ROW_SAMPLE, *responsesMat);

		// ѵ��������
		SVM_params->train(trainingDataMat, cv::ml::ROW_SAMPLE, responsesMat);//ѵ��
		SVM_params->save("svm.xml");
		
		
		cout << "ѵ�����ˣ�����" << endl;
		
	}
	else if (flag == "2") {
		cout << "ѵ��ģ�Ͳ�������" << endl;
		SVM_params = cv::ml::SVM::load("svm.xml");
		//cout << SVM_params.empty() << endl;
	}
	


	cout << "-------SVM ��ʼԤ��-------------------------------" << endl;

	
	cv::Mat testData;
	cv::Mat tLabel;

	cout << "��ʼ�����������...\n";
	testData = read_Mnist_Image("mnist_dataSet/t10k-images.idx3-ubyte");
	tLabel = read_Mnist_Label("mnist_dataSet/t10k-labels.idx1-ubyte");
	cout << "�ɹ�����������ݣ�����\n";


	float count = 0;
	cout << testData.rows << endl;
	for (int i = 0; i < testData.rows; i++) {
		cv::Mat sample = testData.row(i);
		float res = SVM_params->predict(sample);
		res = std::abs(res - tLabel.at<unsigned int>(i, 0)) <= FLT_EPSILON ? 1.f : 0.f;
		count += res;
	}
	
	cout << "��ȷ��ʶ����� count = " << count << endl;
	cout << "������Ϊ..." << (10000 - count + 0.0) / 10000 * 100.0 << "%....\n";

	
	//system("pause");
	return 0;
}

