#pragma once
/****************************************************************

Author: qiuyh

Creating Date: 18/11/27

Description: The main class of stitching pannorama images.

***************************************************************/
#include "Utils.h"
#include <set>
#include <vector>
#include <queue>


#include "vl/generic.h"
#include "vl/sift.h"
#include "vl/kdtree.h"

#define NUM_OF_PAIR 4
#define CONFIDENCE 0.99
#define INLIER_RATIO 0.5
#define RANSAC_THRESHOLD 4.0
#define KEYPOINTS_PAIRS_THRESHOLD 60

// �洢��ͬͼƬ֮���������ƥ���
struct KeyPointPair {
	VlSiftKeypoint vp1;
	VlSiftKeypoint vp2;
	KeyPointPair(VlSiftKeypoint _v1, VlSiftKeypoint _v2) :vp1(_v1), vp2(_v2) {}
};

// �����������ͼƬ�����ͼƬ
void DisplayFeatureImage(map<vector<float>, VlSiftKeypoint> pointSet, CImg<float> img);

class PanoImageStitching
{
	vector<string> imageFileNameGroup;
	vector<CImg<float>> imageGroup;   /*��ƴ�ӵ�ͼ����*/
	int imageNum;                     /*��ƴ�ӵ�ͼ����Ŀ*/

	vector<map<vector<float>, VlSiftKeypoint>> siftFeaturePointsGroup; /*���ͼ��������㼯��*/

	vector<vector<int>> adjacentImages;   /*����KNNȷ��ͼƬƴ��˳��*/
	vector<vector<bool>> adjacentFlag;     /*ƴ��λ�õı�־*/

public:
	/*1. �ú������ڽ����е��ļ�·����ȡ��һ��ͼ������*/
	void ReadAllImage(vector<string> imageFilePath);

	/*2. ��ͼ��ת��Ϊ��������*/
	CImg<float> ImageToSphericalCoordinates(CImg<float> image);

	/*3. ����ɫͼ��תΪRGBͼ��*/
	CImg<float> ToGrayImage(CImg<float> & image);

	/*4. ����ͼ����ȡ������,���������㷵�ش洢*/
	map<vector<float>, VlSiftKeypoint> ExtractSIFTFeaturePoint(CImg<float> grayImage);

	/*5. ��������ͼƬ�������㼯�ϣ�����kd������ƥ������������*/
	vector<KeyPointPair> FindKNearestNeighbor(map<vector<float>, VlSiftKeypoint> &featurePoint1, 
		map<vector<float>, VlSiftKeypoint> &featurePoint2);

	/*6.  �Ӹ����������㼯����ȷ����Ӧ����*/
	HomographyMatrix GetHomographyFromPointPairs(const vector<KeyPointPair> &pair);

	/*7. ������ŵ����Ŀ*/
	vector<int> ComputeInliers(HomographyMatrix HMatrix, vector<KeyPointPair> pair, set<int> randomPairIndex);

	/*8. ���¼������п��ŵ����С���˵ĵ�Ӧ����*/
	HomographyMatrix ComputeLeastSquaresHomographyMatrix(vector<KeyPointPair> pairs, vector<int> inlier_Indexs);

	/*7. ��������Խ���RANSAC�����õ���Ӧ����*/
	HomographyMatrix RANSACForHomographyMatrix(vector<KeyPointPair> featurePointPair);

	/*8. ��RANSAC�Ľ����������ģ�ͣ�������ѧ��֤��ȥ��һЩ��Ч��RANSAC���*/
	bool MathVerification(HomographyMatrix HMatrix);

	/*9. ����ͼ��ƴ�Ӻ�ĳ��ȿ��,�任��ͼ��������С���*/
	CImg<float> CalSizeOfStitchingImage(CImg<float> stitchedImage, CImg<float> adjacentImage, 
		HomographyMatrix transformMatrix);
	
	/*10. ����ͼƬƯ��, ����ƫ����x�� y����ƴ�Ӻõ�ͼƬ�滻����һ��ͼƬ��*/
	void ShiftImageByOffset(CImg<float> &stitchedImg, CImg<float> &dstImg, int offsetX, int offsetY);

	/*11. ���õ�Ӧ�������ͼ��Ť��*/
	void WarpingImageWithHomography(CImg<float> &stitchedImg, CImg<float> &dstImage, 
		HomographyMatrix H, float offsetX, float offsetY);

	/*12. ��һ��ͼƬ��������Լ��Ͻ���Ư�ƣ�ͼ��ƴ��֮��������������Ҫ���и���*/
	void ShifFeaturePointByOffset(map<vector<float>, VlSiftKeypoint> &featurePointSet,
		int offsetX, int offsetY);

	/*13. ���õ�Ӧ�������������������Ť��*/
	void WarpingFeaturePointWithHomography(map<vector<float>, VlSiftKeypoint> &featurePointSet, 
		HomographyMatrix H, int offsetX, int offsetY);

	/*��ÿ�����ӽ��������� �����ᡣ��*/
	void BundleAdjustment();

	/*����  */
	void Straightening();

	/*����*/
	void GainCompensation();

	/*.����������ͼƬ���ж�Ƶ���ں�*/
	CImg<float> blendTwoImages(const CImg<float> &a, const CImg<float> &b);

	/*. ��������ڽӾ��βü�*/
	void Crop(CImg<float> &stichedImg);

	/* ƴ���ܿغ���*/
	void StartStitching();
};