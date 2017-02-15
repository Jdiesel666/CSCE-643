/*1 step rectification*/

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

Mat myImg;
vector<Point3d> dist;
int points = 0;

void getCoord(int event, int x, int y, int flags, void* parameter)  // locate pixel coordinates in a loaded image
{

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		points++;

		cout << "Point: [" << x << ", " << y << "]" << endl;	// display original coordinates
		circle(myImg, Point(x, y), 8, CV_RGB(255, 0, 0), -1);	// mark the selections in red

		if (points < 21)
		{
			dist.push_back(Point3d(x, y, 1));

			if (points % 2 == 0)
		{
			Point3d p1 = dist[points - 2];
			Point3d p2 = dist[points - 1];
			line(myImg, Point2d(p1.x, p1.y), Point2d(p2.x, p2.y), CV_RGB(0, 255, 0), 2);
		}
		}

		imshow("Original", myImg);
	}

}

Point3d normPoint3d(Point3d p) 
{
	p.x = p.x / p.z; 
	p.y = p.y / p.z; 
	p.z = 1;

	return (p);
}

Mat HOM()
{
	// Compute five sets of orthogonal lines 
	Point3d l1 = dist[0].cross(dist[1]); 
	Point3d m1 = dist[2].cross(dist[3]); 
	Point3d l2 = dist[4].cross(dist[5]); 
	Point3d m2 = dist[6].cross(dist[7]); 
	Point3d l3 = dist[8].cross(dist[9]); 
	Point3d m3 = dist[10].cross(dist[11]); 
	Point3d l4 = dist[12].cross(dist[13]); 
	Point3d m4 = dist[14].cross(dist[15]); 
	Point3d l5 = dist[16].cross(dist[17]); 
	Point3d m5 = dist[18].cross(dist[19]);

	cout << "l1 = " << l1 << endl;
	cout << "m1 = " << m1 << endl;
	cout << "l2 = " << l2 << endl;
	cout << "m2 = " << m2<< endl;
	cout << "l3 = " << l3 << endl;
	cout << "m3 = " << m3 << endl;
	cout << "l4 = " << l4 << endl;
	cout << "m4 = " << m4 << endl;
	cout << "l5 = " << l5 << endl;
	cout << "m5 = " << m5 << endl;

	// build lm and z for lm*z=b
	double lmdata[5][6] = { { l1.x * m1.x, (l1.x * m1.y + l1.y * m1.x) / 2,
		l1.y * m1.y, (l1.x * m1.z + l1.z * m1.x) / 2, (l1.y * m1.z + l1.z * m1.y) / 2, l1.z * m1.z},
		{ l2.x * m2.x, (l2.x * m2.y + l2.y * m2.x) / 2, l2.y * m2.y,
		(l2.x * m2.z + l2.z * m2.x) / 2, (l2.y * m2.z + l2.z * m2.y) / 2, l2.z * m2.z },
		{ l3.x * m3.x, (l3.x * m3.y + l3.y * m3.x) / 2, l3.y * m3.y, (l3.x * m3.z + l3.z * m3.x) / 2,
		(l3.y * m3.z + l3.z * m3.y) / 2, l3.z * m3.z},
		{ l4.x * m4.x, (l4.x * m4.y + l4.y * m4.x) / 2, l4.y * m4.y, (l4.x * m4.z + l4.z * m4.x) / 2,
		(l4.y * m4.z + l4.z * m4.y) / 2, l4.z * m4.z},
		{ l5.x * m5.x, (l5.x * m5.y + l5.y * m5.x) / 2, l5.y * m5.y, (l5.x * m5.z + l5.z * m5.x) / 2,
		(l5.y * m5.z + l5.z * m5.y) / 2, l5.z * m5.z }};
							
	Mat LM = Mat(5, 6, CV_64FC1, lmdata);

	cout << "LM = " << LM << endl;

	Mat W, V, Vt;

	SVD::compute(LM, W, V, Vt, 4);

	cout << " Vt = " << Vt << endl;

	double Cinfdata[3][3] = { { Vt.at<double>(5,0), Vt.at<double>(5,1) / 2, Vt.at<double>(5,3) / 2 },

	{ Vt.at<double>(5,1) / 2, Vt.at<double>(5,2), Vt.at<double>(5,4) / 2 },

	{ Vt.at<double>(5,3) / 2, Vt.at<double>(5,4) / 2, Vt.at<double>(5,5) } };

	if (Cinfdata[2][2] < 0) {

		for (int i = 0; i < 3; i++)

			for (int j = 0; j < 3; j++)

				Cinfdata[i][j] = -Cinfdata[i][j];

	}

	cout << "Vt = " << Vt << endl;
	
	Mat Cinf = Mat(3, 3, CV_64FC1, Cinfdata);
	cout << "C Infinity = " << Cinf << endl;

	// SVD Cinf
	Mat U, D, UT;

	// calc A
	Mat AAt = Cinf(Range(0, 2), Range(0, 2));
	cout << "AAt = " << AAt << endl;

	Mat Dsquare;
	SVD::compute(AAt, Dsquare, U, UT, 0);

	cout << "U = " << U << endl;
	cout << "UT = " << UT << endl;
	// calc D
	pow(Dsquare, 0.5, D);
	D = Mat::diag(D);

	Mat A = U * D * UT;
	cout << "A = " << A << endl;

	// Av = Cinf_half_e_d
	double Cinf_half_e_dnum[2][1] = { {Cinf.at<double>(0, 2)},
									 { Cinf.at<double>(1, 2) } };
	Mat Cinf_half_e_d(2, 1, CV_64FC1, Cinf_half_e_dnum);

	cout << "Cinf Half e and d = " << Cinf_half_e_d << endl;

	Mat v;
	solve(A, Cinf_half_e_d, v, CV_LU);
	
	// create H
	Mat H;

	double Hr[2][1] = { {0}, {0} };
	Mat HR = Mat(2, 1, CV_64FC1, Hr);
	hconcat(A, HR, H);

	double Hd[1][3] = { { v.at<double>(0), v.at<double>(1), 1 } };
	Mat HD = Mat(1, 3, CV_64FC1, Hd);
	vconcat(H, HD, H);

	cout << "H = " << H << endl;

	return(H);

}

int main()
{
	namedWindow("Original", CV_WINDOW_NORMAL);  // create window 

	resizeWindow("Original", 800, 500);		    // resize the window for image 1
	myImg = imread("project101.jpg");		    // load image

	//myImg = imread("project102.jpg");
	//resizeWindow("Original", 600, 600);

	imshow("Original", myImg);				    // display the image
	cvSetMouseCallback("Original", getCoord);

	// press ESC to exit, r to run after selection
	for (;;)
	{
		uchar key = (uchar)waitKey();

		if (key == 27)
			break;
		if (key == 'r')
		{
			// remove distortion
			Mat H = HOM();
			Mat Rec(myImg.rows, myImg.cols, myImg.type());  // create rectified image
			Size newsize(myImg.cols * 2, myImg.rows * 2);		   // modify the image size to display the fit the entire image in the window

			//warpPerspective(myImg, Rec, H, newsize);	   // perform transformation img1
			warpPerspective(myImg, Rec, H, myImg.size());	   // perform transformation img2

			namedWindow("Rec", CV_WINDOW_NORMAL);		   // create output window
			imshow("Rec", Rec);					   // diplayed rectified image
			imwrite("remove dist 1.jpg", Rec);		       // save image 1
			//imwrite("remove dist 2.jpg", Rec);			   // save image 2

													   
		}
	}
	return (0);	
}