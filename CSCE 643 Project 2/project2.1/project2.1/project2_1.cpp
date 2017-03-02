/* DLT Method*/


#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

Mat img1;	// matrix for figure 1
Mat img2;	// matrix for figure 2
int point;	// point count

// selection on each image
// 1_______2
// |       |
// |       | 
// 4_______3
double selection1[40] = {2155, 614, 2253, 594, 2261, 778, 2161, 795,
						 2337, 578, 2445, 561, 2453, 750, 2343, 769,
						 2351, 840, 2455, 823, 2455, 859, 2353, 871,
						 2169, 862, 2257, 848, 2259, 882, 2169, 893,
						 2273, 617, 2310, 609, 2320, 716, 2257, 724 };

double selection2[40] = {195, 611, 297, 603, 283, 786, 179, 792,
						 381, 600, 484, 594, 474, 781, 367, 786,
						 367, 854, 463, 851, 461, 888, 365, 888,
						 179, 859, 275, 854, 269, 888, 179, 896,
						 312, 623, 355, 623, 349, 730, 308, 730 };

// build A matrix
Mat buildA(double a1, double a2)
{
	Mat A = Mat(40, 9, CV_64F, Scalar(0));	// construct a 40 x 9 matrix w/ 0

	// extract x and y from selection
	double x[20] = {};
	double y[20] = {};
	double xp[20] = {};
	double yp[20] = {};
	int i;
	int num = 0;
	
	for (i = 0; i < 20; i++)
	{
		x[num] = selection1[2 * i];
		y[num] = selection1[2 * i + 1];
		xp[num] = selection2[2 * i];
		yp[num] = selection2[2 * i + 1];
		num++;
	}
	
	
	int ct = 0;
	int ro;
	
	// replace the corresponding terms in A matrix
	for (ro = 0; ro < A.rows; ro++)
	{
		if (ro % 2 == 0)	// reconstruct 2 rows if satisfies the condition
		{			
			A.row(ro).col(3) = -x[ct];
			A.row(ro).col(4) = -y[ct];
			A.row(ro).col(5) = -1;
			A.row(ro).col(6) = x[ct] * yp[ct];
			A.row(ro).col(7) = y[ct] * yp[ct];
			A.row(ro).col(8) = yp[ct];

			A.row(ro + 1).col(0) = x[ct];
			A.row(ro + 1).col(1) = y[ct];
			A.row(ro + 1).col(2) = 1;
			A.row(ro + 1).col(6) = -x[ct] * xp[ct];
			A.row(ro + 1).col(7) = -y[ct] * xp[ct];
			A.row(ro + 1).col(8) = -xp[ct];

			ct++;
		}

	}

	//cout << "\n";
	//cout << "A = " << A << endl;
	
	return(A);

}

//void getCoord(int event, int x, int y, int flags, void* parameter)  // locate pixel coordinates in a loaded image
//
//{
//	if (event == CV_EVENT_LBUTTONDOWN)
//	{
//		point++;
//
//		cout << "Point "<< point <<":" "[" << x << ", " << y << "]" << endl;	// display original coordinates
//		//circle(img1, Point(x, y), 8, CV_RGB(255, 0, 0), -1);					// mark selections
//		//imshow("Figure", img1);
//		circle(img2, Point(x, y), 10, CV_RGB(255, 0, 0), -1);
//		imshow("Figure", img2);
//
//	}
//
//}

void main()
{
	//namedWindow("Figure", CV_WINDOW_NORMAL);
	//resizeWindow("Figure", 800, 650);
	img1 = imread("fig1.jpg");
	//imshow("Figure", img1);
	img2 = imread("fig2.jpg");	
	/*imshow("Figure", img2);
	cvSetMouseCallback("Figure", getCoord);
	waitKey(0);*/

	// calculate H
	Mat A = buildA(selection1[40], selection2[40]);
	Mat U, Dsq, Vt;
	SVD::compute(A, Dsq, U, Vt);

	//cout << "Dsq = " << Dsq << endl;
	//cout << "U = " << U << endl;
	//cout << "Vt = " << Vt << endl;

	// extract the last row of Vt and buld H
	double h[3][3] = { { Vt.at<double>(Vt.rows - 1,0), Vt.at<double>(Vt.rows - 1,1), Vt.at<double>(Vt.rows - 1,2) },
					   { Vt.at<double>(Vt.rows - 1,3), Vt.at<double>(Vt.rows - 1,4), Vt.at<double>(Vt.rows - 1,5) },
				       { Vt.at<double>(Vt.rows - 1,6), Vt.at<double>(Vt.rows - 1,7), Vt.at<double>(Vt.rows - 1,8) } };

	Mat H = Mat(3, 3, CV_64FC1, h);

	cout << "H =" << H << endl;
	
	// find translation matrix
	vector<Point2f> in;
	vector<Point2f> out;

	in.push_back(Point2f(0, 0));
	in.push_back(Point2f(img1.cols, 0));
	in.push_back(Point2f(0, img1.rows));
	in.push_back(Point2f(img1.cols, img1.rows));

	perspectiveTransform(in, out, H);
	/*cout << "\n";
	cout << "in = " << in << endl;
	cout << "out = " << out << endl;*/

	int count;
	double minx = out[0].x;
	double miny = out[0].y;
	for (count = 0; count < 4; count++)
	{
		if (out[count].x < minx) {
			minx = out[count].x;
		}
		if (out[count].y < miny) {
			miny = out[count].y;
		}
	}

	Mat shift = Mat::eye(3, 3, CV_64FC1);
	shift.at<double>(0, 2) = -minx;
	shift.at<double>(1, 2) = -miny;

	cout << "\n";
	cout << "Shift = " << shift << endl;

	// build panorama
	Mat final = shift * H;

	cout << "\n";
	cout << "final = " << final << endl;

	Mat pano;
	//Mat pano(img1.rows, img1.cols, img1.type());			// test img1
	//warpPerspective(img1, pano, final, img1.size()*4);
	
	int img2offx = -minx;
	int img2offy = -miny;
	warpPerspective(img1, pano, final, Size(img1.cols + img2.cols, img1.rows)*4);
	Mat part(pano, Rect(img2offx, img2offy, img2.cols, img2.rows));	// shift img2
	img2.copyTo(part);	// stitch img2 to warped img1
	
	//namedWindow("Pano", CV_WINDOW_NORMAL);
	//resizeWindow("Pano", 1200, 650);
	imshow("Pano", pano);
	imwrite("problem 1.jpg", pano);
	
	return;
}