#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include <math.H>
using namespace std;
using namespace cv;

#define pivalue 3.14159265358;


Mat img, img1, erosion_dst, dilation_dst, sampleOut;
VideoCapture wc(0);

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;

void contournconvexhull()
{

	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int count = 0, ck = 0;
	findContours(img1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	size_t largestContour = 0;
	for (size_t i = 1; i < contours.size(); i++)
	{
		if (contourArea(contours[i]) > contourArea(contours[largestContour]))
			largestContour = i;
	}
	drawContours(img, contours, largestContour, Scalar(0, 0, 255), 1);
	if (!contours.empty())
	{
		vector<vector<Point> > hull(1);
		convexHull(Mat(contours[largestContour]), hull[0], false);
		drawContours(img, hull, 0, Scalar(0, 255, 0), 3);
		
		if (hull[0].size() > 2)
		{
			vector<int> hullIndexes;
			convexHull(Mat(contours[largestContour]), hullIndexes, true);
			vector<Vec4i> cd;
			convexityDefects(Mat(contours[largestContour]), hullIndexes, cd);



			for (size_t i = 0; i < cd.size(); i++)
			{
				Point p1 = contours[largestContour][cd[i][0]];  //start
				Point p2 = contours[largestContour][cd[i][1]];   //end
				Point p3 = contours[largestContour][cd[i][2]];   //far

				float a, b, c, b2, a2, c2, d;
				a = sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
				b = sqrt(pow((p3.x - p1.x), 2) + pow((p3.y - p1.y), 2));
				c = sqrt(pow((p2.x - p3.x), 2) + pow((p2.y - p3.y), 2));

				a2 = a*a;
				b2 = b*b;
				c2 = c*c;
				d = 2 * b*c;
				float angle = acos(((b2 + c2 - a2) / d));
				float f1, f2;
				f1 = pivalue;
				f2 = pivalue;
				f1 /= 2;
				f2 = (f2 * 4) / 180;
				
				if (angle <= f1 && angle > f2)
				{
					if (ck == 0)
					{
						ck++;
						count = 1;
					}
					
					count += 1;
					line(img, p1, p3, Scalar(255, 0, 0), 2);
					line(img, p3, p2, Scalar(255, 0, 0), 2);

				}
				

			}

		}

		cout << count << endl;
	}



	imshow("hull", img);
}

void Erosion(int, void*)
{
	int erosion_type;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	erode(img1, img1, element);

}


void Dilation(int, void*)
{
	int dilation_type;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	dilate(img1, img1, element);

}

void skincolor()
{

	int minH, maxH, minS, maxS, minV, maxV;
	minH = 0;
	maxH = 20;
	minS = 30;
	maxS = 150;
	minV = 60;
	maxV = 255;

	cvtColor(img, img1, CV_BGR2HSV);
	inRange(img1, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), img1);


	namedWindow("Skin Color image", CV_WINDOW_NORMAL);
	imshow("Skin Color image", img1);

	int blurSize = 5;
	int elementSize = 5;
	medianBlur(img1, img1, blurSize);
	Dilation(0, 0);
	Erosion(0, 0);
	namedWindow("Original image", CV_WINDOW_NORMAL);
	imshow("Original image", img);
	namedWindow("After noise reduction", CV_WINDOW_NORMAL);
	imshow("After noise reduction", img1);

	contournconvexhull();


}
int main()
{

	int choice;
	char ekey = 0;
	cout << "Choose your option";
	cout << "1) Import image";
	cout << "2)Live feed";
	cin >> choice;
	switch (choice)
	{
	case 1:

		img = imread("yo.jpg");
		skincolor();
		while (ekey != 27)
		{
			ekey = waitKey(1);
		}
		break;
	case 2:
		if (wc.isOpened() == false)
		{
			cout << "Web Cam Not working";
			return -1;
		}
		char ekey = 0;
		while (ekey != 27 && wc.isOpened())
		{
			bool frame = wc.read(img);
			if (!frame || img.empty())
			{
				cout << "frame not read from webcam\n";
				break;
			}
			skincolor();
			ekey = waitKey(1);
		}
		break;

	}
	return 0;
}
