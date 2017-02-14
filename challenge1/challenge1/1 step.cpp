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

void getCoord(int event, int x, int y, int /*flags*/, void*)  // locate pixel coordinates in a loaded image
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
			Point3d p1 = dist[points - 7];
			Point3d p2 = dist[points - 6];
			line(myImg, Point2d(p1.x, p2.y), Point2d(p2.x, p2.y), CV_RGB(0, 255, 0), 2);
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

	cout << "imgcoord = " << imgcoord << endl;

	// find real-world coordinates
	Mat m_imgcoord = Mat(imgcoord, true);
	m_imgcoord = m_imgcoord.reshape(1, 4);
	cout << "Img coord = " << m_imgcoord.t() << endl;	// transpose and display

	Mat m_realcoord = H.inv() * m_imgcoord.t();
	cout << "Real coord = " << m_realcoord << endl;

	// normalize using 3rd column
	m_realcoord.row(0) = m_realcoord.row(0).mul(1 / m_realcoord.row(2));
	m_realcoord.row(1) = m_realcoord.row(1).mul(1 / m_realcoord.row(2));
	cout << "Normalized Real coord = " << m_realcoord << endl;

	// find max min coord
	double xmin, xmax, ymin, ymax;
	minMaxLoc(m_realcoord.row(0), &xmin, &xmax, 0, 0);
	minMaxLoc(m_realcoord.row(1), &ymin, &ymax, 0, 0);
	cout << "xmin" << xmin << endl;
	cout << "xmax" << xmax << endl;
	cout << "xmin" << ymin << endl;
	cout << "ymax" << ymax << endl;

	// calc image size
	double ratio = myImg.cols / (xmax - xmin);
	double height = (int)((ymax - ymin) * ratio);
	cout << "ratio = " << ratio << endl;

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
			cout << "dy = " << dy << endl;
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
	imshow("remove pro.jpg", imgout);
	return(imgout);
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

}
