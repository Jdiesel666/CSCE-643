/* Sampson Error Method Data Calculation and Write to Files*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void write2file(Mat M, const char* filename);
Mat img1;	// matrix for figure 1
Mat img2;	// matrix for figure 2
int point;	// point count

// selection on each image
// 1_______2
// |       |
// |       | 
// 4_______3
double selection1[40] = { 2155, 614, 2253, 594, 2261, 778, 2161, 795,
						2337, 578, 2445, 561, 2453, 750, 2343, 769,
						2351, 840, 2455, 823, 2455, 859, 2353, 871,
						2169, 862, 2257, 848, 2259, 882, 2169, 893,
						2273, 617, 2310, 609, 2320, 716, 2257, 724 };

double selection2[40] = { 195, 611, 297, 603, 283, 786, 179, 792,
						381, 600, 484, 594, 474, 781, 367, 786,
						367, 854, 463, 851, 461, 888, 365, 888,
						179, 859, 275, 854, 269, 888, 179, 896,
						312, 623, 355, 623, 349, 730, 308, 730 };

Mat findT1(double* m1)
{
	double x[20] = {};
	double y[20] = {};
	double sumx = 0;
	double sumy = 0;
	int i;
	int num = 0;

	// find mean x and y
	for (i = 0; i < 20; i++)
	{
		x[num] = m1[2 * i];
		y[num] = m1[2 * i + 1];
		sumx = sumx + x[num];
		sumy = sumy + y[num];

		num++;
	}

	Mat xm = Mat(1, 20, CV_64FC1, x);
	Mat ym = Mat(1, 20, CV_64FC1, y);
	const char* xcoord = "x.txt";
	const char* ycoord = "y.txt";
	write2file(xm, xcoord);
	write2file(ym, ycoord);

	double meanx = sumx / 20;
	double meany = sumy / 20;

	// calc s
	double s1;
	double denom1 = 0;
	double diffx_sq = 0;
	double diffy_sq = 0;

	int n;

	for (n = 0; n < 20; n++)
	{
		diffx_sq = pow((x[n] - meanx), 2);
		diffy_sq = pow((y[n] - meany), 2);
		denom1 = denom1 + sqrt(diffx_sq + diffy_sq);
	}
	s1 = sqrt(2) * 20 / denom1;

	// calc tx ty
	double tx = -s1 * meanx;
	double ty = -s1 * meany;

	// build T1
	Mat T1 = Mat::eye(3, 3, CV_64FC1);
	T1.at<double>(0, 0) = s1;
	T1.at<double>(0, 2) = tx;
	T1.at<double>(1, 1) = s1;
	T1.at<double>(1, 2) = ty;

	const char* m_T1 = "T1.txt";
	write2file(T1, m_T1);
	//cout << "T1 = " << T1 << endl;
	//cout << "\n";

	return (T1);
}
Mat normX(double* m1, Mat mm1)
{
	double x[20] = {};
	double y[20] = {};
	int i;
	int num = 0;

	// find mean x and y
	for (i = 0; i < 20; i++)
	{
		x[num] = m1[2 * i];
		y[num] = m1[2 * i + 1];

		num++;
	}

	// calc xi hat
	Mat xi = Mat(3, 20, CV_64FC1, Scalar(0));
	int co;

	for (co = 0; co < 20; co++)
	{
		xi.row(0).col(co) = x[co];
		xi.row(1).col(co) = y[co];
		xi.row(2).col(co) = 1;
	}

	Mat Xhat = mm1 * xi;

	const char* xnormed = "X hat.txt";
	write2file(Xhat, xnormed);

	//cout << "Xhat = " << Xhat << endl;

	return (Xhat);

}

Mat findT2(double* m2)
{
	double xp[20] = {};
	double yp[20] = {};
	double sumxp = 0;
	double sumyp = 0;
	int i;
	int num = 0;

	// find mean xp and yp
	for (i = 0; i < 20; i++)
	{
		xp[num] = m2[2 * i];
		yp[num] = m2[2 * i + 1];
		sumxp = sumxp + xp[num];
		sumyp = sumyp + yp[num];

		num++;
	}

	Mat xpm = Mat(1, 20, CV_64FC1, xp);
	Mat ypm = Mat(1, 20, CV_64FC1, yp);
	const char* xpcoord = "xp.txt";
	const char* ypcoord = "yp.txt";
	write2file(xpm, xpcoord);
	write2file(ypm, ypcoord);

	double meanxp = sumxp / 20;
	double meanyp = sumyp / 20;

	// calc s
	double s2;
	double denom2 = 0;
	double diffxp_sq = 0;
	double diffyp_sq = 0;

	int n;

	for (n = 0; n < 20; n++)
	{
		diffxp_sq = pow((xp[n] - meanxp), 2);
		diffyp_sq = pow((yp[n] - meanyp), 2);
		denom2 = denom2 + sqrt(diffxp_sq + diffyp_sq);
	}
	s2 = sqrt(2) * 20 / denom2;

	// calc tx ty
	double txp = -s2 * meanxp;
	double typ = -s2 * meanyp;

	// build T2
	Mat T2 = Mat::eye(3, 3, CV_64FC1);
	T2.at<double>(0, 0) = s2;
	T2.at<double>(0, 2) = txp;
	T2.at<double>(1, 1) = s2;
	T2.at<double>(1, 2) = typ;

	const char* m_T2 = "T2.txt";
	write2file(T2, m_T2);
	//cout << "T2 = " << T2 << endl;
	//cout << "\n";

	return(T2);
}

Mat normXp(double* m2, Mat mm2)
{
	double xp[20] = {};
	double yp[20] = {};
	int i;
	int num = 0;

	// find mean xp and yp
	for (i = 0; i < 20; i++)
	{
		xp[num] = m2[2 * i];
		yp[num] = m2[2 * i + 1];

		num++;
	}
	// calc xi prime hat
	Mat xip = Mat(3, 20, CV_64FC1, Scalar(0));
	int co;

	for (co = 0; co < 20; co++)
	{
		xip.row(0).col(co) = xp[co];
		xip.row(1).col(co) = yp[co];
		xip.row(2).col(co) = 1;
	}

	Mat Xphat = mm2 * xip;

	const char* xpnormed = "Xp hat.txt";
	write2file(Xphat, xpnormed);

	//cout << "Xphat = " << Xphat << endl;

	return (Xphat);

}

// build A matrix
Mat buildA(Mat a1, Mat a2)
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
		x[num] = a1.at<double>(0, i);
		y[num] = a1.at<double>(1, i);
		xp[num] = a2.at<double>(0, i);
		yp[num] = a2.at<double>(1, i);
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

	//cout << "A = " << A << endl;
	//cout << "\n";

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

// save the data to txt file
void write2file(Mat M, const char* filename)
{
	ofstream fout(filename);

	for (int R = 0; R < M.rows; R++)
	{
		for (int C = 0; C < M.cols; C++)
		{
			fout << M.at<double>(R, C) << "\t";
		}
		fout << endl;
	}
	fout.close();
}
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

	// find T1 and T2
	Mat T1 = findT1(selection1);
	Mat T2 = findT2(selection2);
	Mat Xhat = normX(selection1, T1);
	Mat Xphat = normXp(selection2, T2);

	cout << "T1 = " << T1 << endl;
	cout << "\n";
	cout << "T2 = " << T2 << endl;
	/*cout << "\n";
	cout << "Xhat = " << Xhat << endl;
	cout << "\n";
	cout << "Xphat= " << Xphat << endl;*/

	// calculate H hat
	Mat A = buildA(Xhat, Xphat);
	Mat U, Dsq, Vt;
	SVD::compute(A, Dsq, U, Vt);

	//cout << "Dsq = " << Dsq << endl;
	//cout << "U = " << U << endl;
	//cout << "Vt = " << Vt << endl;

	// extract the last row of Vt and buld H
	double h[3][3] = { { Vt.at<double>(Vt.rows - 1,0), Vt.at<double>(Vt.rows - 1,1), Vt.at<double>(Vt.rows - 1,2) },
					{ Vt.at<double>(Vt.rows - 1,3), Vt.at<double>(Vt.rows - 1,4), Vt.at<double>(Vt.rows - 1,5) },
					{ Vt.at<double>(Vt.rows - 1,6), Vt.at<double>(Vt.rows - 1,7), Vt.at<double>(Vt.rows - 1,8) } };

	Mat Hhat = Mat(3, 3, CV_64FC1, h);

	cout << "\n";
	cout << "H hat =" << Hhat << endl;

	Mat H = T2.inv() * Hhat * T1;

	cout << "\n";
	cout << "H = " << H << endl;

	// find translation matrix
	vector<Point2f> in;
	vector<Point2f> out;

	in.push_back(Point2f(0, 0));
	in.push_back(Point2f(img1.cols, 0));
	in.push_back(Point2f(0, img1.rows));
	in.push_back(Point2f(img1.cols, img1.rows));

	perspectiveTransform(in, out, H);
	//cout << "\n";
	//cout << "in = " << in << endl;
	//cout << "out = " << out << endl;

	// build translation matrix
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
	cout << "\n";

	// build panorama
	Mat final = shift * H;

	//cout << "\n";
	cout << "final = " << final << endl;

	// save the data to txt files
	const char* Amatrix = "A matrix.txt";
	const char* Hhatmatrix = "H hat matrix.txt";
	const char* Hmatrix = "H matrix.txt";

	write2file(A, Amatrix);
	write2file(Hhat, Hhatmatrix);
	write2file(H, Hmatrix);

	//Mat pano;
	//Mat pano;
	//Mat pano(img1.rows, img1.cols, img1.type());			// test img1
	//warpPerspective(img1, pano, final, img1.size()*3);

	//int img2offx = -minx;
	//int img2offy = -miny;
	//warpPerspective(img1, pano, final, Size(img1.cols + img2.cols, img1.rows) * 2);
	//Mat part(pano, Rect(img2offx, img2offy, img2.cols, img2.rows));	// shift img2
	//img2.copyTo(part);	// stitch img2 to warped img1

	//namedWindow("Pano", CV_WINDOW_NORMAL);
	//resizeWindow("Pano", 1200, 650);
	//imshow("Pano", pano);
	//imwrite("problem 2.jpg", pano);

	return;
}