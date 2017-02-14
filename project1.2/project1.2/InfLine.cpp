/*Problem 2 Step*/

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
vector<Point3d> prodist;
vector<Point3d> affdist;
int points = 0;


/* 5
   1________2        6       9
   |        |
   |        | 
   |        |
   4________3        8       7 */

void getCoord(int event, int x, int y, int flags, void* parameter)  // locate pixel coordinates in a loaded image
{
	
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		points++; 

		cout << "Point: [" << x << ", " << y << "]" << endl;	// display original coordinates
		circle(myImg, Point(x, y), 8, CV_RGB(255, 0, 0), -1);	// mark the selections in red
		
		// connect points with lines, click point 1 as point 5 to connect points
		if (points < 6)	
		{
			prodist.push_back(Point3d(x, y, 1));

			if (points > 1)
			{
				Point3d p1 = prodist[points - 2];
				Point3d p2 = prodist[points - 1];
				line(myImg, Point2d(p1.x, p1.y), Point2d(p2.x, p2.y), CV_RGB(0, 255, 0), 2);
				
			}
		}
		else if (points < 10)
		{
			affdist.push_back(Point3d(x, y, 1));

			if (points % 2 == 1)
			{
				Point3d p1 = affdist[points - 7];
				Point3d p2 = affdist[points - 6];
				line(myImg, Point2d(p1.x, p1.y), Point2d(p2.x, p2.y), CV_RGB(0, 0, 255), 2);
			}
		}

		imshow("Original", myImg);
	}
	
}

Mat remove_dist(Mat myImg, Mat H)
{
	int row, col;

	// create boundaries in image plane
	vector<Point3d> imgcoord;
	imgcoord.push_back(Point3d(0, 0, 1));
	imgcoord.push_back(Point3d(0, myImg.rows - 1, 1));
	imgcoord.push_back(Point3d(myImg.cols - 1, 0, 1));
	imgcoord.push_back(Point3d(myImg.cols - 1, myImg.rows - 1, 1));

	//cout << "imgcoord = " << imgcoord << endl;

	// find real-world coordinates
	Mat m_imgcoord = Mat(imgcoord, true);
	m_imgcoord = m_imgcoord.reshape(1, 4);
	//cout << "Img coord = " << m_imgcoord.t() << endl;	// transpose and display

	Mat m_realcoord = H.inv() * m_imgcoord.t();
	//cout << "Real coord = " << m_realcoord << endl;

	// normalize using 3rd column
	m_realcoord.row(0) = m_realcoord.row(0).mul(1 / m_realcoord.row(2));
	m_realcoord.row(1) = m_realcoord.row(1).mul(1 / m_realcoord.row(2));
	//cout << "Normalized Real coord = " << m_realcoord << endl;

	// find max min coord
	double xmin, xmax, ymin, ymax;
	minMaxLoc(m_realcoord.row(0), &xmin, &xmax, 0, 0);
	minMaxLoc(m_realcoord.row(1), &ymin, &ymax, 0, 0);
	/*cout << "xmin" << xmin << endl;
	cout << "xmax" << xmax << endl;
	cout << "xmin" << ymin << endl;
	cout << "ymax" << ymax << endl;*/

	// calc image size
	double ratio = myImg.cols / (xmax - xmin);
	double height = (int)((ymax - ymin) * ratio);
	//cout << "ratio = " << ratio << endl;

	Mat imgout(height, myImg.cols, CV_8UC3);	//build output image

												// point interpolation
												// temporary real and image coord
	Mat temp_real(3, 1, CV_64FC1);
	Mat temp_img(3, 1, CV_64FC1);

	temp_real.at<double>(2, 0) = 1; // let 3rd component to 1
	double step = 1 / ratio;
	for (col = 0; col < imgout.cols; col++)
	{
		temp_real.at<double>(0, 0) = (double)col*step + xmin;	//set x coord
		for (row = 0; row < imgout.rows; row++)
		{
			double x, y, dx, dy;	// temp vars
			temp_img.at<double>(1, 0) = (double)row*step + ymin;	// set y coord
			temp_img = H * temp_real;	// find coord in image plane

										// normalize
			x = temp_img.at<double>(0, 0) / temp_img.at<double>(2, 0);
			y = temp_img.at<double>(1, 0) / temp_img.at<double>(2, 0);

			// check if out of boundaries
			if (x < 0 || x > myImg.cols - 1 || y < 0 || y > myImg.rows - 1)
			{
				// do nothing
			}

			dx = x - (int)x;
			dy = y - (int)y;
			//cout << "dy = " << dy << endl;
			// interpolate if dx and dy are non zero
			Vec3b idx00 = myImg.at<Vec3b>(int(x), int(y));
			if (dx != 0.0 || dy != 0.0)
			{
				Vec3b idx10 = myImg.at<Vec3b>(int(y), int(x + 1));
				Vec3b idx01 = myImg.at<Vec3b>(int(y + 1), int(x));
				Vec3b idx11 = myImg.at<Vec3b>(int(y + 1), int(x + 1));

				imgout.at<Vec3b>(row, col) = idx00 * (1 - dx)
					* (1 - dy) + idx10 * dx * (1 - dy) + idx01
					* (1 - dx) * dy + idx11 * dx * dy;
			}
			else
			{
				imgout.at<Vec3b>(row, col) = idx00;
			}
		}

	}

	imwrite("remove pro.jpg", imgout);
	//imshow("remove pro.jpg", imgout);
	return(imgout);
}

Point3d normPoint3d(Point3d p)	// normalize 3D point
{
	p.x = p.x / p.z;
	p.y = p.y / p.z;
	p.z = 1;

	return(p);
}


// calculate H for projective trans
Mat pro_hom() 
{
	// calculate line of infinity
	Point3d l1 = prodist[0].cross(prodist[1]);
	Point3d l2 = prodist[3].cross(prodist[2]);
	Point3d l3 = prodist[0].cross(prodist[3]);
	Point3d l4 = prodist[1].cross(prodist[2]);
	Point3d P1 = l1.cross(l2);
	Point3d P2 = l3.cross(l4);
	Point3d Linf = P1.cross(P2);

	Linf = normPoint3d(Linf);
	cout << "Line of Infinity: " << Linf << endl;

	// H matrix for projective correction
	Mat Hp = Mat::eye(3, 3, CV_64FC1);
	Hp.at<double>(2, 0) = Linf.x;
	Hp.at<double>(2, 1) = Linf.y;
	Hp.at<double>(2, 2) = Linf.z;
	cout << "Hp = " << Hp << endl;

	return(Hp);
}

// 3 by 1 matrix to 3D conversion
Point3d mat_to_3D(Mat p) 
{
	Point3d P3D = Point3d(p.at<double>(0), p.at<double>(1), p.at<double>(2));
	return (P3D);
}

// calculate H for affine trans
Mat aff_hom(Mat Hp)
{
	// two set of orthogonal lines
	Point3d L1 = affdist[0].cross(affdist[1]);
	Point3d L2 = affdist[2].cross(affdist[3]);
	Point3d L3 = affdist[1].cross(affdist[2]);
	Point3d L4 = affdist[1].cross(affdist[3]);

	// remove proj dist of real lines
	Mat L1mat = Hp.inv().t() * Mat(L1, true);
	Mat L2mat = Hp.inv().t() * Mat(L2, true);
	Mat L3mat = Hp.inv().t() * Mat(L3, true);
	Mat L4mat = Hp.inv().t() * Mat(L4, true);

	// matrix to 3D conversion
	L1 = mat_to_3D(L1mat);
	L2 = mat_to_3D(L2mat);
	L3 = mat_to_3D(L3mat);
	L4 = mat_to_3D(L4mat);

	/*cout << "L1 = " << L1 << endl;
	cout << "L2 = " << L2 << endl;
	cout << "L3 = " << L3 << endl;
	cout << "L4 = " << L4 << endl;*/

	// --  --  --
	// lm * s = b
	double lm[2][2] = { {L1.x * L2.x, L1.x * L2.y + L1.y * L2.x},
						  {L3.x * L4.x, L3.x * L4.y + L3.y * L4.x} };
	double b[2][1] = { {-L1.y * L2.y}, {-L3.y * L4.y} };
	Mat m_lm = Mat(2, 2, CV_64FC1, lm);
	Mat m_b = Mat(2, 1, CV_64FC1, b);

	//cout << "lm = " << m_lm << endl;
	//cout << "b = " << m_b << endl;

	Mat s = m_lm.inv() * m_b;	// solve for s
	//cout << "s = " << s << endl;

	double Sdata[2][2] = { { s.at<double>(0), s.at<double>(1) },{ s.at< double>(1), 1 } };	// build S
	Mat S = Mat(2, 2, CV_64FC1, Sdata);

	cout << "S = " << S << endl;

	// build C inf
	Mat Cinf;
	double Ccol[2][1] = { { 0 },{ 0 } };
	Mat CCOL = Mat(2, 1, CV_64FC1, Ccol);
	hconcat(S, CCOL, Cinf);

	double Crow[1][3] = { { 0, 0, 1 } };
	Mat CROW = Mat(1, 3, CV_64FC1, Crow);
	vconcat(Cinf, CROW, Cinf);

	cout << "C infinity = " << Cinf << endl;

	// SVD for S
	Mat U, D, Dsquare, UT;
	SVD::compute(S, Dsquare, U, UT, 0);

	cout << "U = " << U << endl;
	cout << "D^2 = " << Dsquare << endl;
	cout << "UT = " << UT << endl;

	pow(Dsquare, 0.5, D);	// calc D
	D = Mat::diag(D);
	cout << "D = " << D << endl;

	Mat A = U * D * U.inv();	// find A
	cout << "A = " << A << endl;

	// find H for affinity;
	Mat Ha;
	double Acol[2][1] = { {0}, {0} };
	Mat ACOL = Mat(2, 1, CV_64FC1, Acol);
	hconcat(A, ACOL, Ha);

	double Arow[1][3] = { {0, 0, 1} };
	Mat AROW = Mat(1, 3, CV_64FC1, Arow);
	vconcat(Ha, AROW, Ha);

	cout << "Ha = " << Ha << endl;

	return(Ha);
}

int main()
{
	namedWindow("Original", CV_WINDOW_NORMAL);  // create window 

	//resizeWindow("Original", 800, 500);		    // resize the window for image 1
	//myImg = imread("project101.jpg");		    // load image

	myImg = imread("project102.jpg");
	resizeWindow("Original", 600, 600);

	imshow("Original", myImg);				    // display the image
	cvSetMouseCallback("Original", getCoord);
	
	// press ESC to exit, r to run after selection
	for (;;)
	{
		uchar key = (uchar) waitKey();

		if (key == 27)
			break;
		if (key == 'r')
		{
			// remove projective distortion
			Mat Hp = pro_hom();
			Mat prorec(myImg.rows, myImg.cols, myImg.type());  // create rectified image
			Size newsize(myImg.cols*2, myImg.rows*2);		   // modify the image size to display the fit the entire image in the window
			
			//warpPerspective(myImg, prorec, Hp, newsize*2);	   // perform transformation img1
			warpPerspective(myImg, prorec, Hp, myImg.size());	   // perform transformation img2
			
			namedWindow("Pro Rectified", CV_WINDOW_NORMAL);		   // create output window
			imshow("Pro Rectified", prorec);					   // diplayed rectified image
			//imwrite("remove pro 1.jpg", prorec);		       // save image 1
			imwrite("remove pro 2.jpg", prorec);			   // save image 2
			
			// remove affine distortion based on projective distortion removal
			Mat Ha = aff_hom(Hp);
			Mat affrec(myImg.rows, myImg.cols, myImg.type());
			Mat final = Hp * Ha.inv();
			cout << "final = " << final << endl;

			//warpPerspective(myImg, affrec, final, newsize);	   // perform transformation img 1
			warpPerspective(myImg, affrec, final, myImg.size());	// trans for img 2

			namedWindow("Aff Rectified", CV_WINDOW_NORMAL);		   // create output window
			imshow("Aff Rectified", affrec);					   // diplayed rectified image
			//imwrite("remove aff 1.jpg", affrec);
			imwrite("remove aff 2.jpg", affrec);
		}
	}
	return (0);
}