/*****************************************
����˵����
	inputFileName:  ��ת���Ĳ�ɫBMPͼ��
����˵����
	����ɫBMPͼ��ת��Ϊ�Ҷ�ͼ
******************************************/

#include "Canny.h"
#include <queue>
#include <algorithm>

int Canny::readGrayImage(string inputFileName)
{
	try
	{
		SrcImage.load(inputFileName.c_str());
	}
	catch (const std::exception&)
	{
		return 0;
	}

	height= SrcImage._height;      // �����ж�Ӧͼ��߶�----------,����ȥ��
	width = SrcImage._width;     // �ж�Ӧͼ����
	CImg<short int> tmp(width, height, 1, 1, 0);
	grayImage = tmp;
	smoothImage = tmp;
	deltaXImage = tmp;
	deltaYImage = tmp;
	magnitudeImage = tmp;
	dirRadianImage = tmp;
	nonMaxSuppImage = tmp;
	resultEdgeImage = tmp;
	cutShortEdgeImage = tmp;

	// �Ҷ�ת��
	cimg_forXY(SrcImage, x, y) {
		// R G B 
		// Gray = R*0.299 + G*0.587 + B*0.114
		//grayImage(x, y) = SrcImage(x, y);
	     grayImage(x, y) = SrcImage(x, y, 0) * 0.299 + SrcImage(x, y, 1) * 0.587 + SrcImage(x, y, 2) * 0.114;

		// grayImage(x, y) = (SrcImage(x, y, 0) * 299 + SrcImage(x, y, 1) * 587 + SrcImage(x, y, 2) * 114 + 500) / 1000;
	}
	if (DEBUG) cout << "����BMP��ת��Ϊ�Ҷ�ͼ�ɹ�" << endl;
	
	return 1;
}



/**************************************
������
	outputFileName: ������Ե���ͼ����
	image: ������Ե���ͼ�����
���ܣ�
	�������ĻҶ�ͼ
****************************************/
int Canny::writeGrayImage(string outputFileName, CImg<short int> image)
{
	image.save(outputFileName.c_str());
	if (DEBUG) cout << "���ͼ���ѱ���" << endl;
	return 0;
}



/***********************************************************************************
������
	image: �����ͼ�����
	sigma: ��˹�ֲ��ı�׼�������˹�˲���ͼ��ƽ���ĳ̶ȣ���˹�˲����ڵ�ͨfilter
		   sigmaԽ�󣬸�˹�˲�����Ƶ����Խ��ƽ���̶Ⱦ�Խ��
	smootheDim: The image after gaussian smoothing.
���ܣ�
	��image������и�˹ƽ�����������smootheDim
*************************************************************************************/
void Canny::gaussianSmooth(CImg<short int>* image, float sigma, CImg<short int>* smootheDim)
{
	int r, c, rr, cc,       /* Counter variables. */
		windowsize,         /* Dimension of the gaussian kernel. */
		center;             /* Half of the windowsize. */
	float *tempim,          /* Buffer for separable filter gaussian smoothing. */
		*kernel,            /* A one dimensional gaussian kernel. */
		dot,                /* Dot product summing variable. */
		sum;                /* Sum of the kernel weights variable. */
	int cols = (*image)._width;                        // ������У�ͼ��Ŀ�
	int rows = (*image)._height;                       // ������У�ͼ��ĳ�
	if (DEBUG) cout << "��˹ģ�� " << endl;
	
	CImg<short int> tempImg(cols, rows, 1, 1, 0);  // ��ʱ�Ҷ�ͼ


	/**********************************
	* 1. ����һά��˹ƽ����
	***********************************/
	makeGaussianKernel(sigma, &kernel, &windowsize);
	center = windowsize / 2;
	

	/*   ��˹�˲��ǿɷ���ģ�2D�ĸ�˹kernel�������x��y�����ȶ�x�����˲����˲������y�����˲�  */
	
	/****************************
	* 2. ��ʼx������˲�ģ�� ,���
	*****************************/
	if (DEBUG) cout << "Blur in the x-direction." << endl;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {
			dot = 0.0;
			sum = 0.0;
			for (cc = (-center); cc <= center; cc++) {
				if ((c + cc >= 0) && ((c + cc) < cols)) {
					dot += (float)(*image)(c+cc,r) * kernel[center + cc];
					sum += kernel[center + cc];
				}
			}
			tempImg(c, r) = dot / sum;
			//cout << r << "  " << c << endl;
		}
	}

	/*******************************
	* 3. ��ʼy�����ϵ��˲�ģ�� ���
	********************************/
	if (DEBUG) cout << "Blur in the y-direction. " << endl;
	for (c = 0; c < cols; c++) {
		for (r = 0; r < rows; r++) {
			sum = 0.0;
			dot = 0.0;
			for (rr = (-center); rr <= center; rr++) {
				if ((r + rr >= 0) && (r + rr < rows)) {
					dot += tempImg(c, r + rr) * kernel[center + rr];
					sum += kernel[center + rr];
				}
			}
			(*smootheDim)(c, r) = (short int)(dot * BOOSTBLURFACTOR / sum + 0.5);
		}
	}
	free(kernel);
	
}

/**********************************************************
������
	sigma: ��˹�ֲ���׼��
	kernel��kernel�����˹�˵�ֵ���ȴ����˹������Ȼ���һ��
	windowsize: ��˹�˳ߴ磬�˲�����ֵ
���ܣ�
	�õ���˹��
*************************************************************/
void Canny::makeGaussianKernel(float sigma, float ** kernel, int * windowsize)
{
	int i, center;
	float x, fx, sum = 0.0;

	*windowsize = 1 + 2 * ceil(2.5 * sigma);  
	center = (*windowsize) / 2;

	//sigmaԽ����ɢ�̶�Խ��������˼�Ȩ�Ĺ����У�����ֵ��Ȩ��Խ�����ɴ˴�����ͼ��Խģ����
	if (DEBUG) printf("      The kernel has %d elements.\n", *windowsize);
	if ((*kernel = (float *)calloc((*windowsize), sizeof(float))) == NULL) {
		fprintf(stderr, "Error callocing the gaussian kernel array.\n");
		exit(1);
	}

	for (i = 0; i<(*windowsize); i++) {
		x = (float)(i - center);
		fx = pow(2.71828, -0.5*x*x / (sigma*sigma)) / (sigma * sqrt(6.2831853));
		(*kernel)[i] = fx;  // ��˹�ˣ�ֱ�ӵ��λ�õ��룬�õ�����ֵ
		sum += fx;
	}

	for (i = 0; i<(*windowsize); i++) (*kernel)[i] /= sum;  // ��˹�˹�һ��

	if (DEBUG) {
		printf("The filter coefficients are:\n");
		for (i = 0; i<(*windowsize); i++)
			printf("kernel[%d] = %f\n", i, (*kernel)[i]);
	}
}

/**********************************************************
������
	smootheDim: ��˹ƽ��������ͼ�����
	delta_x��x������ƫ�����ͼ�����
	delta_y: y������ƫ�����ͼ�����
�������ܣ�
	����x yƫ�����ͼ��
***********************************************************/
void Canny::derrivativeXY(CImg<short int> *smootheDim,
	CImg<short int> * delta_x, CImg<short int>  * delta_y)
{
	int cols = (*smootheDim)._width;                        // ������У�ͼ��Ŀ�
	int rows = (*smootheDim)._height;                       // ������У�ͼ��ĳ�


	if (DEBUG) cout << "���� x �����ƫ��ͼ��" << endl;
	
	/*
	for (int r = 1; r < rows - 1; r++) {
		for (int c = 1; c < cols - 1; c++) {
			(*delta_x)(c, r) = (*smootheDim)(c + 1, r) - (*smootheDim)(c, r);
		}
	}

	if (DEBUG) cout << "���� y�����ƫ��ͼ��" << endl;
	for (int c = 1; c < cols - 1; c++) {
		for (int r = 1; r < rows - 1; r++) {
			(*delta_y)(c, r) = (*smootheDim) (c, r + 1) - (*smootheDim)(c, r);
		}
	}		
	*/

	
	/*sobel ����*/
	
	int a00, a01, a02, a10, a11, a12, a20, a21, a22;
	for (int c = 1; c < cols - 1; c++) {
		for (int r = 1; r < rows - 1; r++) {
			a00 = (*smootheDim)(c - 1, r - 1);
			a01 = (*smootheDim)(c , r - 1);
			a02 = (*smootheDim)(c + 1, r - 1);
			a10 = (*smootheDim)(c - 1, r );
			a11 = (*smootheDim)(c, r);
			a12 = (*smootheDim)(c + 1, r );
			a20 = (*smootheDim)(c - 1, r + 1);
			a21 = (*smootheDim)(c , r + 1);
			a22 = (*smootheDim)(c + 1, r + 1);
			(*delta_x)(c, r) = a20 * 1 + a10 * 2 + a00 + ((-1) * a02 + (-2) * a12 + (-1)*a22);
			(*delta_y)(c, r) = a02 * (1) + a01 * (2) + a00 * (1) + a20 * (-1) + a21 * (-2) + a22 * (-1);
		}
	}
		
	

	

}

/*******************************************************************************
* FUNCTION: angle_radians
* PURPOSE: This procedure computes the angle of a vector with components x and
* y. It returns this angle in radians with the answer being in the range
* 0 <= angle <2*PI.
*******************************************************************************/
double Canny::angle_radians(double x, double y)
{
	///if (DEBUG) cout << "angel radians  ƫ���н�" << endl;

	double xu, yu, ang;

	xu = fabs(x);
	yu = fabs(y);

	if ((xu == 0) && (yu == 0)) return(0);

	ang = atan(yu / xu);

	if (x >= 0) {
		if (y >= 0) return(ang);
		else return(2 * PI - ang);
	}
	else {
		if (y >= 0) return(PI - ang);
		else return(PI + ang);
	}
}

/*******************************************************************************
* Procedure: radian_direction
* Purpose: To compute a direction of the gradient image from component dx and
* dy images. Because not all derriviatives are computed in the same way, this
* code allows for dx or dy to have been calculated in different ways.
*
* FOR X:  xdirtag = -1  for  [-1 0  1]  x ��ķ����־
*         xdirtag =  1  for  [ 1 0 -1]
*
* FOR Y:  ydirtag = -1  for  [-1 0  1]' y��ķ����־
*         ydirtag =  1  for  [ 1 0 -1]'
*        ���� -1  -1 ����ͼ������ϵ����x�ᳯ�ң�y�ᳯ��
* The resulting angle is in radians measured counterclockwise from the
* xdirection. The angleMy Points "up the gradient".

������
	delta_x, delta_y Դͼ���x��y�����ƫ��ͼ����
	dir_radian �ݶȷ���ͼ
	xdirtag, ydirtag ȷ�� dx�� dy�ķ��� 
*******************************************************************************/
void Canny::radianDirection(CImg<short int>* delta_x, CImg<short int>* delta_y, 
	CImg<short int>* dir_radians, int xdirtag, int ydirtag)
{
	if (DEBUG) cout << "�����ݶȷ���ͼ" << endl;

	int cols = (*delta_x)._width;
	int rows = (*delta_x)._height;
	
	double dx, dy;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			dx = (double)(*delta_x)(c, r);
			dy = (double)(*delta_y)(c, r);
			if (xdirtag == 1) dx = -dx;
			if (ydirtag == -1) dy = -dy;
			(*dir_radians)(c, r) =(float)angle_radians(dx, dy);
		}
	}
}

/*
������x����ƫ��ͼ��y����ƫ��ͼ���ݶȷ�ֵͼ
���ܣ������ݶȷ�ֵ = sqrt��x^2 + y^2)
*/
void Canny::magnitudeXY(CImg<short int> * delta_x, CImg<short int> * delta_y, 
	 CImg<short int>* magnitude)
{
	if (DEBUG) cout << "�����ݶȷ�ֵͼ" << endl;
	int cols = (*delta_x)._width;
	int rows = (*delta_x)._height;

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			int sq1 =(*delta_x)(col, row) * (*delta_x)(col, row);
			int sq2 = (*delta_y)(col, row)* (*delta_y)(col, row);
			(*magnitude)(col, row) = (short)(0.5 + sqrt( (float) sq1 + (float) sq2));
		}
	}
}

/**********************************************************************************
�������ݶȷ�ֵͼ��x��y����ƫ��ͼ���ݶȷ���ͼ�� �Ǽ���ֵ����ͼ
���ܣ��Ǽ���ֵ���ƺ����ǣ������ݶȷ�ֵͼ�ϵ�ÿһ����G���Ƚϱ�Ե�����ϵ����������㣬
	 �������㶼��G����ôG��Ϊ0�������С�ڻ����G����ôG = POSSIBLEEDGE������
***********************************************************************************/
void Canny::nonMaxSuppression(CImg<short int>* magnitude, CImg<short int>* delta_x,
	CImg<short int>* delta_y, CImg<short int>* nonMaxSupp)
{
	if (DEBUG) cout << "non max suppression " << endl;
	int ncols = (*delta_x)._width;
	int nrows = (*delta_x)._height;

	/*Ԥ���������ͼ��ı�Ե��Ϊ0*/
	cimg_forXY(*nonMaxSupp, x, y) {
		(*nonMaxSupp)(x, y) = NOTEDGE;
	}

	for (int i = 1; i < ncols; i++) {
		for (int j = 1; j < nrows; j++) {
			/*************************************************************************
			* ɨ���ݶȷ�ֵͼ��ȷ����xMag, yMag)�Ƿ�Ϊ���ݶȷ����ϵļ���ֵ��,
			*   ����������G1, G2��Ļ�����λ�����Ҷ�POSSIBLEEDGE����Ե����������Ϊ0�����Ǳ�Ե��
			**************************************************************************/
			float deltaGx = (float)(*delta_x)(i, j);  // �õ��x����ƫ��
			float deltaGy = (float)(*delta_y)(i, j);  // �õ��y����ƫ��
			float weight = abs(deltaGy / deltaGx);
			if ((*magnitude)(i, j) < 30) continue;
			if (deltaGx == 0) {
				if ((*magnitude)(i, j) >= (*magnitude)(i, j - 1) && (*magnitude)(i, j) >= (*magnitude)(i, j + 1))
					(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
			}
			else if (deltaGy == 0) {
				if ((*magnitude)(i, j) >= (*magnitude)(i - 1, j) && (*magnitude)(i, j) >= (*magnitude)(i + 1, j))
					(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
			}
			else if (deltaGx == deltaGy) {
				if ((*magnitude)(i, j) >= (*magnitude)(i - 1, j - 1) && (*magnitude)(i, j) >= (*magnitude)(i + 1, j + 1))
					(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
			}
			else if (deltaGx == (-1)*deltaGy) {
				if ((*magnitude)(i, j) >= (*magnitude)(i - 1, j + 1) && (*magnitude)(i, j) >= (*magnitude)(i + 1, j - 1))
					(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
			}
			else {
				/*  ��������
				float angle = atan(abs(deltaGy) / deltaGx);
				if (angle <= PI / 8 || angle >= PI * 7.0 / 8) {
					if ((*magnitude)(i, j) > (*magnitude)(i - 1, j) && (*magnitude)(i, j) > (*magnitude)(i + 1, j))
						(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
				}
				else if (angle > PI / 8 && angle < PI * 3 / 8) {
					if ((*magnitude)(i, j) >(*magnitude)(i - 1, j - 1) && (*magnitude)(i, j) > (*magnitude)(i + 1, j + 1))
						(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
				}
				else if (angle >= PI * 3 / 8 && angle <= PI * 5 / 8) {
					if ((*magnitude)(i, j) > (*magnitude)(i, j - 1) && (*magnitude)(i, j) > (*magnitude)(i, j + 1))
						(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
				}
				else if (angle > PI * 5 / 8 && angle < PI * 7 / 8) {
					if ((*magnitude)(i, j) >(*magnitude)(i - 1, j + 1) && (*magnitude)(i, j) > (*magnitude)(i + 1, j - 1))
						(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
				}
				*/
	
				
				// ���Բ�ֵ�� 
				if (deltaGx * deltaGy < 0) {
					if (abs(deltaGx) > abs(deltaGy)) {
						float G1 = weight * (*magnitude)(i + 1, j - 1) + (1 - weight) * (*magnitude)(i + 1, j);
						float G2 = weight * (*magnitude)(i - 1, j + 1) + (1 - weight) * (*magnitude)(i - 1, j);
						if ((*magnitude)(i, j) >= G1 && (*magnitude)(i, j) >= G2)
							(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
					}
					else if(abs(deltaGx) < abs(deltaGy)) {
						float G1 = weight * (*magnitude)(i + 1, j - 1) + (1 - weight) * (*magnitude)(i, j - 1);
						float G2 = weight * (*magnitude)(i - 1, j + 1) + (1 - weight) * (*magnitude)(i, j + 1);
						if ((*magnitude)(i, j) >= G1 && (*magnitude)(i, j) >= G2)
							(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
					}
				}
				else if(deltaGx * deltaGy > 0) {
					if (abs(deltaGx) > abs(deltaGy)) {
						float G1 = weight * (*magnitude)(i - 1, j - 1) + (1 - weight) * (*magnitude)(i - 1, j);
						float G2 = weight * (*magnitude)(i + 1, j + 1) + (1 - weight) * (*magnitude)(i + 1, j);
						if ((*magnitude)(i, j) >= G1 && (*magnitude)(i, j) >= G2)
							(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
					}
					else if(abs(deltaGx) < abs(deltaGy)){
						float G1 = weight * (*magnitude)(i - 1, j - 1) + (1 - weight) * (*magnitude)(i, j - 1);
						float G2 = weight * (*magnitude)(i + 1, j + 1) + (1 - weight) * (*magnitude)(i, j + 1);
						if ((*magnitude)(i, j) >= G1 && (*magnitude)(i, j) >= G2)
							(*nonMaxSupp)(i, j) = POSSIBLEEDGE;
					}
				}
				
				
			}
		}

	}
}

/****************************************************************************
������ �ݶ�ͼmagnitude�����������ͼnonMaxSupp, ��Եͼedge
		����ֵ��tlow
        ����ֵ��thigh
���ܣ���Ե��⣬���ټٱ�Ե����Ŀ�����������ǲ���˫��ֵ����ѡ�������ߵ���ֵ��
	  ���������������£�
		�÷�������ƵĽ����ʼ��edge��ɨ��edge��
		��������ص��ݶ�ֵС��tlow���������Ϊ�Ǳ�Ե���أ�
		��������ص��ݶ�ֵ����thigh���������Ϊ��Ե���أ�
		��������ص��ݶ�ֵ����tlow��thigh֮�䣬��Ҫ��һ���������ص������ڵ�8���㣬
		�����8��������һ�������ϵĵ��ݶȳ�����thigh���������Ϊ��Ե���أ������Ǳ�Ե���ء���
**************************************************************************************/
void Canny::applyHysteresis(CImg<short int>* magnitude, CImg<short int>* nonMaxSupp, 
	   float tlow, float thigh, CImg<short int>* edge)
{
	if (DEBUG) cout << "apply_hystersis ing " << endl;
	int cols = (*magnitude)._width;
	int rows = (*magnitude)._height;

	/*1. �����ֵ���ƽ������ʼ����Եͼ,ͼ��������Ϊ0 */
	cimg_forXY(*nonMaxSupp, x, y) {
		(*edge)(x, y) = (*nonMaxSupp)(x, y);
		if (x == 0 || x == cols || y == 0 || y == rows)
			(*edge)(x, y) = NOTEDGE;
	}


	cimg_forXY(*edge, x, y) {
		// ��������ص��ݶ�ֵС��tlow���������Ϊ�Ǳ�Ե���أ�
		if ((*magnitude)(x, y) < tlow) {
			(*edge)(x, y) = 0;
		}
		//��������ص��ݶ�ֵ����thigh���������Ϊ��Ե���أ�
		else if ((*magnitude)(x, y) > thigh) {
			(*edge)(x, y) = POSSIBLEEDGE;
		}
		//��������ص��ݶ�ֵ����tlow��thigh֮�䣬��Ҫ��һ���������ص������ڵ�8���㣬
		//�����8��������һ�������ϵĵ��ݶȳ�����thigh���������Ϊ��Ե���أ������Ǳ�Ե���ء���
		else {
			//������������8��������
			if (x > 0 && y > 0 && x < cols && y < rows) {
				if ((*magnitude)(x - 1, y - 1) >  thigh|| (*magnitude)(x, y - 1) >   thigh || (*magnitude)(x + 1, y - 1) >  thigh
					|| (*magnitude)(x - 1, y) >   thigh || (*magnitude)(x, y + 1) >   thigh ||
					(*magnitude)(x - 1, y + 1) >  thigh || (*magnitude)(x, y + 1) >  thigh || (*magnitude)(x + 1, y + 1) >   thigh)
					(*edge)(x, y) = POSSIBLEEDGE;
				else
					(*edge)(x, y) = NOTEDGE;
			}
		}
	}

}



/****************************************************************
������ edge ��ֵ������ͼ�����
���ܣ� ɾ��ͼ���н϶̳���С��20�ı�Ե����Ե���ȶ���Ϊ���ص�ĸ���
	   �����ǣ���ĳһ��Ե���й��������ͳ�Ƴ��ȣ���С��20�ͽ���Щ��Ե����ΪNOTEDGE.
	   �ﵽ�޳��̱�Ե��Ŀ�ġ�
******************************************************************/
void Canny::cutShortEdge(CImg<double>* edge, CImg<double>* resultEdgeImage, int cutLen)
{
	if (DEBUG) cout << "  �޳��̱�Ե" << endl;
	int cols = (*resultEdgeImage)._width;
	int rows = (*resultEdgeImage)._height;
	cimg_forXY((*resultEdgeImage), x, y) {
		(*edge)(x, y) = (*resultEdgeImage)(x, y);
	}
	

	// ���ͼ��, ��¼�������ĵ㣬���ûҶ�ֵΪ128
	CImg<double> tmp(cols, rows, 1, 1, 0);
	queue<XY> edgeQueue;   //����һ�����н��й������
	vector<XY>PointSet;   //һ����Ե�ĵ�ļ���
	
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			//cout << "1" << endl;
			if ((*edge)(i, j) == POSSIBLEEDGE && tmp(i, j) != 128) {
				// �Ӹõ���������б�Ե��������ͳ�Ʊ�Ե�㳤��
			//	cout << "0" << endl;
				while(!edgeQueue.empty())
					edgeQueue.pop();   //��ն���
				 PointSet.clear();      //  �������

				 XY Point;              //��ṹ��
				 Point.col = i;         // ��ǰ�� ����
				 Point.row = j;

				edgeQueue.push( Point);  // ����ǰ�ڵ�������
				 PointSet.push_back( Point);  //����ǰ�ڵ�����¼������
				tmp(i, j) = 128;      //  ��ǰλ���ѱ�����

				int ii = i;   // ���������б仯������
				int jj = j;

				while (edgeQueue.size() != 0) {  //  ������Ե���
					ii = edgeQueue.front().col;  // ��������
					jj = edgeQueue.front().row;
					edgeQueue.pop();   //������ͷ
					bool jumpFlag = false;  // �Ż���
					// �����㣨ii��jj���� ����
					for (int iii = -1; iii <= 1; iii++) {
						for (int jjj = -1; jjj <= 1; jjj++) {
							if (iii == 0 && jjj == 0)
								continue;  // ������ʼ��
							 Point.col = ii + iii;
							 Point.row = jj + jjj;

							bool flag = false;
							for (int t = 0; t <PointSet.size(); t++) {
								if ( PointSet[t].col ==Point.col &&PointSet[t].row ==Point.row) {
									flag = true;
									break;
								}
							}
							if (flag) continue;  // �����������ĵ� 

							// �ҵ�δ����������Ϊ��Ե��ĵ�
							if ((*edge)(ii + iii, jj + jjj) == POSSIBLEEDGE){
								tmp(ii + iii, jj + jjj) = 128;
								 PointSet.push_back( Point);
								edgeQueue.push( Point);
							//	cout << "1" << endl;
							 }
							if ( PointSet.size() > 20) {
								jumpFlag = true;
								break;
							}
					
						}

					}
					if (jumpFlag)
						break;
				}
				if ( PointSet.size() <= cutLen) { // ɾ���̱�Ե
					//cout << " 2 " << endl;
					for (int k = 0; k <PointSet.size(); k++) {
						int r = PointSet[k].row;
						int c = PointSet[k].col;
						tmp(c, r) = 128;
						(*edge)(c, r) = NOTEDGE;  
					}
				}
				 PointSet.clear();

			}
		}
	}
	cout << "finish" << endl;
}


/*
�������ļ����б�Ե��⣬Ȼ��õ����ͼ��
*/
CImg<short int> Canny::canny(string inputFileName) {
	cout << "���ڼ��ͼ���Ե..." << endl;
	/*1. ��ȡͼ�񣬲�ת��Ϊ�Ҷ�ͼ*/
	readGrayImage(inputFileName);
	/*2. ��˹ƽ��*/
	gaussianSmooth(&grayImage, this->sigma, &smoothImage);
	/*3. ��xy�����ƫ��ͼ*/
	derrivativeXY(&smoothImage, &deltaXImage, &deltaYImage);
	/*4. ���ݶȷ���ͼ*/
	radianDirection(&deltaXImage, &deltaYImage, &dirRadianImage, -1, -1);  //x�ᳯ�� -1 ,y�ᳯ�� -1 
	/*5. ���ݶȷ�ֵͼ*/
	magnitudeXY(&deltaXImage, &deltaYImage, &magnitudeImage);
	/*6. ���������*/
	nonMaxSuppression(&magnitudeImage, &deltaXImage, &deltaYImage, &nonMaxSuppImage);
	/*7. �ͺ���ֵ����*/
	applyHysteresis(&magnitudeImage, &nonMaxSuppImage, tLow, tHigh, &resultEdgeImage);
	/*8.  �޳��̱�Ե*/
	//cutShortEdge(&cutShortEdgeImage,&resultEdgeImage, 30);

	/*9.�������ͼ��*/
	string name;  // �淶����

	cout << "-------------" << endl;
	/*��ͼ�񱣴�*/
	/*1.  �Ҷ�ͼ */
	
	sprintf_s(output, "%d_Gray_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.assign(output);
	name += inputFileName;
//	writeGrayImage(name, grayImage);
	
	/*2.��˹ģ��ͼ*/
	sprintf_s(output, "%d_Smoothe_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name, smoothImage);

	/*3.x y����ƫ��ͼ */
	sprintf_s(output, "%d_DeltaX_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s",number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name,deltaXImage);
	sprintf_s(output, "%d_DeltaY_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s",number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name, deltaYImage);

	/*4. �ݶȷ���ͼ*/
	sprintf_s(output, "%d_DirRadian_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, this->BOOSTBLURFACTOR,this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name, dirRadianImage);

	/*5. �ݶȷ�ֵͼ*/
	sprintf_s(output, "%d_Magnitude_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh,this->BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName; 
	//writeGrayImage(name,  magnitudeImage);

	/*6. ���������ͼ*/
	sprintf_s(output, "%d_NonMaxSupp_sigma=%.2f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name, nonMaxSuppImage);

	/*7.�ͺ���ֵ����õ�����ͼ��*/
	sprintf_s(output, "%d_resultEdge_sigma=%.3f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
//	writeGrayImage(name, resultEdgeImage);
	//cout << name.c_str() << endl;

	/*8. �̱�Ե���*/
	sprintf_s(output, "%d_cutShortEdge_sigma=%.3f_low=%.2f_high=%.2f_bf=%.4f_%s", number, this->sigma, this->tLow, this->tHigh, BOOSTBLURFACTOR, this->inputFileName.c_str());
	name.clear();
	name.assign(output);
	name += inputFileName;
	//writeGrayImage(name, cutShortEdgeImage);

	// ��ӡͼ��
	if (DEBUG) {
		//grayImage.display("Gray source image");
		//smoothImage.display("Smooth image");
		//deltaXImage.display("delta x image");
		//deltaYImage.display("delta y image");
		//dirRadianImage.display("direction radian image.");
		magnitudeImage.display("magnitude image.");
		nonMaxSuppImage.display("non max suppression image");
		resultEdgeImage.display("the result of edge image");
		cutShortEdgeImage.display("cut the short edge of the image");
	}
	
	number++; // �ļ�����ŵ���
	//return cutShortEdgeImage;
	return resultEdgeImage;
}
