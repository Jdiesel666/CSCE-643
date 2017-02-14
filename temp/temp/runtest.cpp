// load image take coordinates and mark the points

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

// locate pixel coordinates from a loaded image
void getCoord(int event, int x, int y, int flags, void* parameter)
{
	int num;
	int in[8] = {};
	int i = 1;
	Point* p = (Point*)parameter;
	p->x = x;
	p->y = y;
	//IplImage* img = (IplImage*)parameter;
	if (i <= 4)
	{
		num = 0;
		if (event == CV_EVENT_LBUTTONDOWN)
		{
			printf("%d , %d\n", x, y);

			in[2 * num] = x;
			in[2 * num + 1] = y;
			num++;
		}
		i++;
	}

}


void main()
{
	Point p;	
	namedWindow("My Image", CV_WINDOW_NORMAL); // create window subject to user adjustment
	resizeWindow("My Image", 800, 500);
	Mat myImg = imread("project101.jpg"); // load image
	imshow("My Image", myImg); // display the image
	cvSetMouseCallback("My Image", getCoord, &p);
	
	cvWaitKey(0);

	// initialize
	int x1;
	int y1; 


	//CvMat* H;
	//CvMat* Hinv;

	//int in[8] = { 1643, 194, 1957, 238, 1630, 1234, 1944, 1127 };  // coordinates in original image
	//int out[8] = { 1640, 200, 1957, 250, 1642, 1240, 1957, 1240 }; // coordinates in output image

	//int ct;
	//for (ct = 0; ct < 4; ct++)
	//{
	//	printf("Enter the original coordinates %d x, y: \n", ct + 1);
	//	scanf_s("%d, %d", &in[2 * ct], &in[2 * ct + 1]);
	//	printf("You entered: %d, %d\n", in[2 * ct], in[2 * ct + 1]);

	//	printf("\nEnter the output coordinates %d x, y: \n", ct + 1);
	//	scanf_s("%d, %d", &out[2 * ct], &out[2 * ct + 1]);
	//	printf("You entered: %d, %d\n\n", out[2 * ct], out[2 * ct + 1]);
	//}

	return;
}




/* temp code

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
} */